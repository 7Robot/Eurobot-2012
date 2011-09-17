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
#define chA PORTBbits.RB0
#define chB PORTAbits.RA0

#define Kd 0
#define Kp 50
#define Ki 1

#define XTAL 8000000
#define TCY 4.0*1000/XTAL /* Durée d'un cycle d'horloge en µs. */

/////*PROTOTYPES*/////
void high_isr(void);
void low_isr(void);
void setDC(int dc);

/////*VARIABLES GLOBALES*/////

unsigned int chrono = 0;
char chronoON = 0;
int tick = 0;

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
        if(chronoON) /* On releve sa valeur et on le reinitialise. */
        {
            chrono = ReadTimer0();
            chronoON = 0; /* Il faudra attendre un tour pour le relancer. */
        }
        else chronoON = 1;
        
        WriteTimer0(0);
        INTCONbits.INT0IF = 0;
    }
}

#pragma interrupt low_isr
void low_isr(void)
{
    led = led^1;

    if(INTCONbits.TMR0IE && INTCONbits.TMR0IF)
    {
        printf("Le timer a debordé\n");
        INTCONbits.TMR0IF = 0;
        WriteTimer0(0);
    }

     if(PIE1bits.RCIE && PIR1bits.RCIF)
     {
         char x;
         x = ReadUSART();

         if(x=='v')
         {
            printf("chrono : %d\n", chrono);
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
    TRISA   = 0b11110011 ;
    TRISB   = 0b01111111 ;

    /* Interruption Timer0 */
    OpenTimer0(TIMER_INT_ON & T0_SOURCE_INT & T0_16BIT & T0_PS_1_1);
    INTCON2bits.TMR0IP = 0;

    /* Interruption RB0. */
    OpenRB0INT( PORTB_CHANGE_INT_ON & RISING_EDGE_INT & PORTB_PULLUPS_OFF);

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

    setDC(1023);
    
    while(1);
}


/////*FONCTIONS*/////

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
