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


void high_isr(void);
void low_isr(void);

#pragma code high_vector=0x08

void high_interrupt(void) {
    _asm GOTO high_isr _endasm
}
#pragma code low_vector=0x18

void low_interrupt(void) {
    _asm GOTO low_isr _endasm
}

#pragma code

#pragma interrupt high_isr

void high_isr(void) {
    InterruptServo();
}

#pragma interrupt low_isr

void low_isr(void) {

}

#pragma code

void main(void) {
    int angle = 0;
    int dir = 1;

    CMCON = 0b00000111; /* Désactive les comparateurs. */
    ADCON0 = 0b00000000;
    ADCON1 = 0b00001111;
    WDTCON = 0;
    OSCCON = 0b01111111;
    UCON = 0; /* Désactive l'USB. */
    UCFG = 0b00001000;

    OpenServo(2);

    while (1) {
        WriteServo(0, angle);
        WriteServo(1, angle / 2);

        Delay10KTCYx(2);

        angle += dir;

        if (angle >= 180 || angle <= 0) {
            dir = -dir;
            Delay10KTCYx(255);
        }
    }
}
