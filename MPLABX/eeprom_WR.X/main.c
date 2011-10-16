#include <stdio.h>
#include <stdlib.h>
#include <p18f2550.h>
#include <delays.h>


/////*CONFIGURATION*/////
#pragma config FOSC = HS
#pragma config FCMEN = ON
#pragma config IESO = ON
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
#pragma config PLLDIV = 1

#define led PORTCbits.RC0



/////*PROTOTYPES*/////

void high_isr(void);
void low_isr(void);


/////*VARIABLES GLOBALES*/////

unsigned char i;
unsigned char tab[100];
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

unsigned char eeplit(unsigned char ad) //lit à une adresse de l'eeprom
{
EEADR=ad;
EECON1bits.EEPGD=0;
EECON1bits.RD=1;
return(EEDATA);
}

void eepecr(unsigned char ad,unsigned char c) // ecrit ds l'eeprom
{
EEADR=ad;
EEDATA=c;

EECON1bits.EEPGD=0;
EECON1bits.WREN=1;
EECON2=0x55;
EECON2=0xAA;

EECON1bits.WR=1;
EECON1bits.WREN=0;
}

void my_eepecr(unsigned char ad,unsigned char c){
    EEADR=ad;
    EEDATA=c;

    EECON1bits.EEPGD=0;
    EECON1bits.CFGS=0;

    INTCONbits.GIE=0;//desactive les interuptions

    EECON1bits.WREN=1; //sequence obligatoire voir datasheet
    EECON2=0x55;
    EECON2=0xAA;
    EECON1bits.WR=1; //lance ecriture
    while( EECON1bits.WR==1) // attend
    {
    }
    EECON1bits.WREN=0;
    INTCONbits.GIE=1; //autorise les interruptions
}

unsigned char my_eeplit(unsigned char ad) //lit à une adresse de l'eeprom
{
EEADR=ad;
EECON1bits.EEPGD=0;
EECON1bits.CFGS=0;
EECON1bits.RD=1;//lance la lecture 
while( EECON1bits.RD==1) // attend
    {
    }

return(EEDATA);
}


void main (void)
{
//initialisations
    CMCON   = 0b00000111; /* DÃ©sactive les comparateurs. */
    ADCON0  = 0b00000000;
    ADCON1  = 0b00001111;
    WDTCON  = 0 ;
    OSCCON  = 0b01010100;
    UCON    = 0 ;           /* DÃ©sactive l'USB. */
    UCFG    = 0b00001000 ;
    TRISC   = 0b11111000 ;
    TRISA   = 0b11110011 ;
    TRISB   = 0b01111111 ;

    for (i=0;i<100;i++)
    {
        my_eepecr(i,i);
       //Delay10KTCYx( 5 );
    }
    for (i=0;i<90;i++)
    {
        tab[i]=my_eeplit(i);
    }
//DÃ©but Programme 
    while(1){

        led=led^1;
        Delay10KTCYx( 200 ); //200*10 cycles d'horloge (Fcycle=Fquartz/4)
        
    }
}


/////*FONCTIONS*/////

