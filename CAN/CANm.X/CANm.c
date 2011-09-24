#include <stdio.h>
#include <p18cxxx.h>
#include <delays.h>

#define led PORTCbits.RC0

#pragma config OSC = HS
#pragma config FCMEN = ON
#pragma config IESO = OFF
#pragma config PWRT = OFF
#pragma config WDT = OFF
#pragma config WDTPS = 32768
#pragma config MCLRE = ON
#pragma config LPT1OSC = OFF
#pragma config PBADEN = OFF
#pragma config DEBUG = OFF
#pragma config XINST = OFF
#pragma config BBSIZ = 4096
#pragma config LVP = OFF
#pragma config STVREN = ON
#pragma config CPB = OFF


/////VARIABLES GLOBALES ////
char x=0;

/////VARIABLES GLOBALES ////
void high_isr(void);
void low_isr(void);


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


}

#pragma interrupt low_isr
void low_isr(void)
{


}

////////*PROGRAMME PRINCIPAL*////////

void main (void)
{
//initialisations
        ADCON1 = 0x0F ;
	ADCON0 = 0b00000000;
	WDTCON = 0 ;
	OSCCON = 0b01111100;
        


        /* Direction des ports I in, O out*/
	TRISA = 0b11111111 ;
	TRISB = 0b01111111 ;
	TRISC = 0b11111110;
	
        /* Etat des sorties */
        PORTA = 0b11111111 ;
	PORTB = 0b11111111 ;
	PORTC = 0b11111111 ;

        
        INTCONbits.GIE=1;

	while(1)
        {
            Delay10KTCYx(255);
            led = led^1;
        }
        
}

