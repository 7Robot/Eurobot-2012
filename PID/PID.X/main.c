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
//#define TRMAX 3
#define Kd 0
#define Kp 5
#define Ki 2

#define XTAL 8000000
#define TCY 4.0*1000000/XTAL /* Durée d'un cycle d'horloge en µs. */

/////*PROTOTYPES*/////
void high_isr(void);
void low_isr(void);
void GsetDC(int dc);
void DsetDC(int dc);
float calcVitesse(long cyclesTimer);
void calcPID( void );

/////*VARIABLES GLOBALES*/////

char TRMAX = 5 ;
int  k=0, v=0, tps=0;
long cyclesTimer = 0;
char chronoON = 0, tour=0, sens=1, err=0 ;
float Gvitesse = 0, Dvitesse =0, vitesse =0;

int Gconsigne = 0, Gerreur=0, GlastErreur=0;
int GPerreur=0, GDerreur=0, GIerreur=0, Gpwm=0;

int Dconsigne = 0, Derreur=0, DlastErreur=0;
int DPerreur=0, DDerreur=0, DIerreur=0, Dpwm=0;


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
                WriteTimer1(0);
                cyclesTimer = 0;
                INTCONbits.TMR0IF = 0;
            }
            else
            {
                cyclesTimer = cyclesTimer + ReadTimer0();
                if(GchB) sens = 1;
                else sens = -1;
                if(INTCONbits.TMR0IF) err = 1 ; /* Test débordement juste avant mesure. */
                if(tour >= TRMAX)
                {
                    Gvitesse = calcVitesse(cyclesTimer);
                    INTCONbits.INT0IE = 0;
                    INTCON3bits.INT1IE = 1;
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
                    Dvitesse = calcVitesse(cyclesTimer);
                    INTCON3bits.INT1IE = 0;
                    tour = 0;
                    tps = ReadTimer1();
                    calcPID();
                    INTCONbits.INT0IE = 1;
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

    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF) // METTRE TIMER POUR dt constant
    {
        if(INTCONbits.INT0IE || INTCON3bits.INT1IE || err)
        {
           if(INTCONbits.INT0IE) Gvitesse = 0;
           if(INTCON3bits.INT1IE) Dvitesse = 0;
           INTCONbits.INT0IE = 0;
           INTCON3bits.INT1IE = 0;
           err = 0;
           Delay10KTCYx(40); //20ms
           calcPID();
           INTCONbits.INT0E = 1;
        }
        INTCONbits.TMR0IF = 0;
        WriteTimer0(0);
    }

}

#pragma interrupt low_isr
void low_isr(void)
{
    


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
            printf("T1 = %d\n",tps);
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

    /* Configuration Timer1. */
    OpenTimer1(TIMER_INT_OFF & T1_16BIT_RW & T1_SOURCE_INT & T1_PS_1_4
                & T1_OSC1EN_OFF & T1_SYNC_EXT_OFF);

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

  
    GsetDC(0);
    DsetDC(0);
    
    Delay10KTCYx(255);
    Gconsigne = 50;
    Dconsigne = -50;
    
    INTCONbits.INT0E = 1;

    while(1)
    {
       
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

float calcVitesse(long cyclesTimer)
{
   vitesse = cyclesTimer*2*TCY ; /* Durée entre N cycles en us. */
   vitesse = vitesse/(TRMAX - TRMIN) ; /* Moyenne entre deux cycles. */
   vitesse = vitesse*6.0*3.8118; /* Durée d'un tour moteur. */
   vitesse = 60000000/vitesse ;
   if(sens == -1) vitesse = -vitesse ;
   return vitesse ;
}

void calcPID( void )
{
    led = led^1;

    /* Calcul du temps écoulé depuis la dernière mesure. */
    tps = tps/500 ;
    if(tps > 8 && TRMAX > 2) TRMAX-- ;
    if(tps < 12 && TRMAX < 13) TRMAX++ ;

    

    /* Calcul du PID */

    Gerreur = Gconsigne - Gvitesse/60; /* Vitesse en tours par sec. */
    GPerreur = (Kp*Gerreur);
    GDerreur = Kd*(Gerreur - GlastErreur);
    GIerreur = GIerreur + (Ki*Gerreur);
    Gpwm = GPerreur + GDerreur + GIerreur ;

    Derreur = Dconsigne - Dvitesse/60; /* Vitesse en tours par sec. */
    DPerreur = (Kp*Derreur);
    DDerreur = Kd*(Derreur - DlastErreur);
    DIerreur = DIerreur + (Ki*Derreur);
    Dpwm = DPerreur + DDerreur + DIerreur ;

    GsetDC(Gpwm);
    DsetDC(Dpwm);

 //  printf("e : %d tps : %u ms\n",Gerreur,tps);





}
