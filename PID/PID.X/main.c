#include <stdio.h>
#include <stdlib.h>
#include <p18f2550.h>
#include <timers.h>
#include <portb.h>
#include <usart.h>
#include <pwm.h>
#include <math.h>


/////*CONFIGURATION*/////
#pragma config FOSC = HS
#pragma config FCMEN = ON
#pragma config IESO = OFF
#pragma config PWRT = OFF
#pragma config BOR = ON
#pragma config BORV = 2
#pragma config VREGEN = ON
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config LPT1OSC = OFF
#pragma config PBADEN = OFF
#pragma config CCP2MX = ON
#pragma config LVP = OFF
#pragma config DEBUG = OFF
#pragma config PLLDIV = 4

#define led PORTCbits.RC0
#define osc PORTBbits.RB7
#define chA PORTBbits.RB0
#define chB PORTAbits.RA0

#define Kd 0
#define Kp 50
#define Ki 1

#define ticksMin 20
#define XTAL 8000000
#define TCY 4.0*1000/XTAL /* Durée d'un cycle d'horloge en µs. */

/////*PROTOTYPES*/////
void calcdt(void);
void high_isr(void);
void low_isr(void);
void setDC(int dc);
void setPresc(int ticks);
void writeFloat(float f);

/////*VARIABLES GLOBALES*/////
int t0init = 0;
int i=0, rounds=0;
int vitesse=0, erreur=0, lastErreur=0, consigne=0, Derreur=0, Ierreur=0, Perreur=0;

long tempdt;
unsigned int dt;
int ticks = 0;

char test;

/////*INTERRUPTIONS*/////

#pragma code high_vector=0x08
void high_interrupt(void)
{
     _asm GOTO high_isr _endasm
}
#pragma code low_vector=0x18
void low_interrupt(void)
{
     _asm GOTO low_isr _endasm
}
#pragma code

#pragma interrupt high_isr
void high_isr(void)
{
  
    if(INTCONbits.INT0IE && INTCONbits.INT0IF)
    {
        if(chB) ticks--;
        else ticks++;
        INTCONbits.INT0IF = 0;
    }

}

#pragma interrupt low_isr
void low_isr(void)
{
    led = led^1;

    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF)
    {
        vitesse = ticks;
        erreur = consigne - vitesse;
     
        Perreur = Kp*erreur;
        Derreur = (Kd*(erreur - lastErreur));
        Ierreur = Ierreur + Ki*erreur;

        setDC(Perreur + Derreur + Ierreur);
        
        
        ticks = 0;
        lastErreur = erreur;
        INTCONbits.TMR0IF = 0;
    }

     if(PIE1bits.RCIE && PIR1bits.RCIF)
     {
         char x;
         x = ReadUSART();

         if(x=='v')
         {
            printf("P:%d I:%d D:%d v:%d e:%d\n",Perreur,Ierreur,Derreur,vitesse,erreur);          
         }
         if(x=='e')
         {
             consigne=0;
         }
     }

}


/////*PROGRAMME PRINCIPAL*/////
void main (void)
{
//initialisations
    CMCON   =  0b00000111; /* Désactive les comparateurs. */
    ADCON0  = 0b00000000;
    ADCON1  = 0b00001111;
    WDTCON  = 0 ;
    OSCCON  = 0b01111100;
    UCON    = 0 ;           /* Désactive l'USB. */
    UCFG    = 0b00001000 ;
    TRISC   = 0b11111000 ;
    TRISA   = 0b11110011 ;
    TRISB   = 0b01111111 ;

    /* Interruption Timer0 */
    OpenTimer0(TIMER_INT_ON & T0_SOURCE_INT & T0_8BIT & T0_PS_1_128);
    calcdt();
    INTCON2bits.TMR0IP = 0;

    /* Interruption RB0. */
    OpenRB0INT( PORTB_CHANGE_INT_ON & RISING_EDGE_INT & PORTB_PULLUPS_OFF);

    /* Module USART pour remontée d'infos. */
    OpenUSART( USART_TX_INT_OFF &USART_RX_INT_ON & USART_ASYNCH_MODE
                & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 103 );//51 //103
    IPR1bits.RCIP=0 ;

    /* Configuration du PWM1 */
    OpenTimer2(TIMER_INT_OFF & T2_PS_1_1 & T2_POST_1_1);
    OpenPWM1(0xF0);
    OpenPWM2(0xF0);
    SetDCPWM1(0);
    SetDCPWM2(0);

    printf("Demarrage du programme !\n");

    led = 0;
    RCONbits.IPEN = 1;
    INTCONbits.GIE = 1; /* Autorise interruptions haut niveau. */
    INTCONbits.PEIE = 1; /*Autorise interruptions bas niveau. */

    while(1);
}


/////*FONCTIONS*/////

void calcdt(void)
{
    /* Renvoie le temps entre deux debordements du timer 0. */
    char presc;
    dt = 0;
    tempdt = 0;

    if(T0CONbits.T08BIT) tempdt = 256 - t0init ;
    else tempdt = 0xFFFF - t0init ;

    if(!T0CONbits.PSA) /* Si prescaler assigné. */
    {
        presc = T0CON & 0b00000111;
        while(presc >= 0)
        {
         tempdt = 2*tempdt;
         presc--;
        }
    }
    dt = tempdt*TCY;
}

void setDC(int dc)
{
    if(dc >= 0)
    {
        PORTAbits.RA2 = 0;
        PORTAbits.RA3 = 1;
    }
    else
    {
        PORTAbits.RA2 = 1;
        PORTAbits.RA3 = 0;
        dc = -dc;
    }
    if(dc <= 1023) SetDCPWM1(dc);
    else SetDCPWM1(1023);
}

void setPresc(int ticks)
{
    char masque, pre;

    pre = T0CON & 0b00000111;

    if(ticks < 0) ticks = - ticks ;

    if(ticks >= ticksMin) /* On diminue la fréquence pour augmenter la résolution. */
    {
        if(T0CONbits.PSA == 0 & pre == 0) /* PSA = 0 si prescaler activé */
        {
            T0CONbits.PSA = 1;
        }
        else if(T0CONbits.PSA == 0)
        {
            pre--;
        }
    }
    else
    {
        if(T0CONbits.PSA == 1)
        {
            T0CONbits.PSA = 0;
            pre = 0;
        }
        else if(T0CONbits.PSA == 0 & pre < 7)
        {
            pre ++;
        }
    }

    masque = T0CON & 0b11111000 ;
    T0CON = masque | pre ;
    
    calcdt();
}
void writeFloat(float f)
{
    /* Envoie le float non formaté sur la liaison serie. */
    char tab[4],k;
    char * p = &f;
    tab[4]='\0'; /* Pour la fonction putsUSART*/
    for(k=0;k<=3;k++)
    {
        tab[k]=*p;
        p++;
    }
    putsUSART(tab);
}


