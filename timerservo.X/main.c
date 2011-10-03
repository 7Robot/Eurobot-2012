#include <stdio.h>
#include <stdlib.h>
#include <p18f2550.h>
#include <delays.h>


#include "servo.c"


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
#pragma config DEBUG = ON
#pragma config PLLDIV = 2


/////*PROTOTYPES*/////

void high_isr(void);
void low_isr(void);


/////*VARIABLES GLOBALES*/////


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
void high_isr(void) //Timer counting 20ms then high then "angle" then low
{
    InterruptServo();
}

#pragma interrupt low_isr
void low_isr(void)
{


}

#pragma code
void main (void)
{
//initialisations
    CMCON   = 0b00000111; /* Désactive les comparateurs. */
    ADCON0  = 0b00000000;
    ADCON1  = 0b00001111;
    WDTCON  = 0 ;
    OSCCON  = 0b01111111;
    UCON    = 0 ;           /* Désactive l'USB. */
    UCFG    = 0b00001000 ;

    OpenServo(2);

//Variables Globales
//Début Programme
    while(1){
        WriteServo(0, 180);
        WriteServo(1, 40);
        //Delay100TCYx(1);
    }
}


/////*FONCTIONS*/////
