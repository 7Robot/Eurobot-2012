#include <stdio.h>
#include <stdlib.h>
#include <p18f2550.h>
#include <timers.h>
#include <delays.h>
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
#define GchA PORTBbits.RB0
#define GchB PORTAbits.RA0
#define DchA PORTBbits.RB1
#define DchB PORTAbits.RA1

#define TRMIN 2
#define TRMAX 4
#define Kd 0
#define Kp 50
#define Ki 1

#define XTAL 8000000
#define TCY 4.0*1000000/XTAL /* Durée d'un cycle d'horloge en µs. */

/////*PROTOTYPES*/////
void high_isr(void);
void low_isr(void);
void GsetDC(int dc);
void DsetDC(int dc);

/////*VARIABLES GLOBALES*/////


int  k=0, v=0;
long cyclesTimer = 0;
char chronoON = 0, tour=0, sens=1, err=0 ;
float Gvitesse = 0, Dvitesse =0;

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
        if(chronoON) /* Si chrono comptait. */
        {
            tour++;
            if(tour <= TRMIN) /* Mesures foireuses non comptabilisées. */
            {
                cyclesTimer = 0;
                INTCONbits.TMR0IF = 0;
                if(GchB) sens = 1;
                else sens = -1;
            }
            else
            {
                cyclesTimer = cyclesTimer + ReadTimer0();
                if(INTCONbits.TMR0IF) err = 1 ; /* Test débordement juste avant mesure. */
                if(tour >= TRMAX)
                {
                   // Gvitesse = sens*16*64*(TRMAX - TRMIN);
                   // Gvitesse = Gvitesse/(cyclesTimer*TCY);
                    Gvitesse = cyclesTimer*2*TCY ; /* Durée entre N cycles en us. */
                    Gvitesse = Gvitesse/(TRMAX - TRMIN) ; /* Moyenne entre deux cycles. */
                    Gvitesse = Gvitesse*6; /* Durée d'un tour moteur. */
                    Gvitesse = 60000000/Gvitesse ;
                    if(sens == -1) Gvitesse = -Gvitesse ;
                    INTCONbits.INT0IE = 0;
                    tour = 0;
                }
            }
        }
        else
        {
            chronoON = 1;
        }

        WriteTimer0(0);
        INTCONbits.INT0IF = 0;
    }

    if(INTCON3bits.INT1IE && INTCON3bits.INT1IF)
    {
        if(chronoON) /* Si chrono comptait. */
        {
            tour++;
            if(tour <= TRMIN) /* Mesures foireuses non comptabilisées. */
            {
                cyclesTimer = 0;
                INTCONbits.TMR0IF = 0;
                if(DchB) sens = 1;
                else sens = -1;
            }
            else
            {
                cyclesTimer = cyclesTimer + ReadTimer0();
                if(INTCONbits.TMR0IF) err = 1 ; /* Test débordement juste avant mesure. */
                if(tour >= TRMAX)
                {
                   // Gvitesse = sens*16*64*(TRMAX - TRMIN);
                   // Gvitesse = Gvitesse/(cyclesTimer*TCY);
                    Dvitesse = cyclesTimer*2*TCY ; /* Durée entre N cycles en us. */
                    Dvitesse = Dvitesse/(TRMAX - TRMIN) ; /* Moyenne entre deux cycles. */
                    Dvitesse = Dvitesse*6; /* Durée d'un tour moteur. */
                    Dvitesse = 60000000/Dvitesse ;
                    if(sens == -1) Dvitesse = -Dvitesse ; /* Foireux sinon... */
                    INTCON3bits.INT1IE = 0;
                    tour = 0;
                }
            }
        }
        else
        {
            chronoON = 1;
        }

        WriteTimer0(0);
        INTCON3bits.INT1IF = 0;
}

    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF)
    {
        if(INTCONbits.INT0IE || INTCON3bits.INT1IE || err)
        {
           INTCONbits.INT0IE = 0;
           INTCON3bits.INT1IE = 0;
           err = 0;
           Gvitesse = 0;
           Dvitesse = 0;
        }
        INTCONbits.TMR0IF = 0;
        WriteTimer0(0);
    }

}

#pragma interrupt low_isr
void low_isr(void)
{
    led = led^1;


     if(PIE1bits.RCIE && PIR1bits.RCIF)
     {
         char x;
         x = ReadUSART();

         if(x=='g')
         {
             printf("Mesure Gauche...\n");
             INTCONbits.INT0IE = 1;
         }
        if(x=='d')
         {
            printf("Mesure Droite...\n");
            INTCON3bits.INT1IE = 1;
         }
        if(x=='t')
         {
            k = k+10;
            GsetDC(k);
            printf("pwm = %d\n",k);
            INTCON3bits.INT1IE = 1;
         }
         if(x=='v')
         {
             if(Gvitesse)
             {
                 v = Gvitesse;
                 printf("Gauche : %d   ", v);
                 Gvitesse = 0;
             }
             if(Dvitesse)
             {
                 printf("Droite : %d", Dvitesse);
                 Dvitesse =0;
             }
             printf("\n");
         }
         
         PIR1bits.RCIF = 0;
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
    TRISA   = 0b11000011 ;
    TRISB   = 0b01111111 ;

    /* Interruption Timer0 */
    OpenTimer0(TIMER_INT_ON & T0_SOURCE_INT & T0_16BIT & T0_PS_1_4);
    INTCON2bits.TMR0IP = 1;

    /* Interruption RB0. */
    OpenRB0INT( PORTB_CHANGE_INT_ON & RISING_EDGE_INT & PORTB_PULLUPS_OFF);

    /* Interruption RB0. */
    OpenRB1INT( PORTB_CHANGE_INT_ON & RISING_EDGE_INT & PORTB_PULLUPS_OFF);
    INTCON3bits.INT1IP = 1 ; /* Haute priorite. */

    /* Module USART pour remontée d'infos. */
    OpenUSART( USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE
                & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 51 );//51 //103
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

    INTCONbits.INT0E = 0;
    INTCON3bits.INT1IE = 0;


    GsetDC(100);
    DsetDC(100);
    while(1)
    {
       
       /* INTCONbits.INT0IE=1;
        Delay10KTCYx(100);
        v = Gvitesse;
        printf("Gauche : %d \n", v);
        Gvitesse = 0;
       /* GsetDC(k);
        Delay10KTCYx(10);
        INTCONbits.INT0IE=1;
        Delay10KTCYx(10);
        v = Gvitesse ;
        printf("%d %d\n",k,v);
        k=k+10;
        while(k>1020) k=1024 ;*/
    }
}


/////*FONCTIONS*/////

void GsetDC(int dc)
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

void DsetDC(int dc)
{
    if(dc >= 0)
    {
        PORTAbits.RA4 = 0;
        PORTAbits.RA5 = 1;
    }
    else
    {
        PORTAbits.RA4 = 1;
        PORTAbits.RA5 = 0;
        dc = -dc;
    }
    if(dc <= 1023) SetDCPWM2(dc);
    else SetDCPWM2(1023);
}
