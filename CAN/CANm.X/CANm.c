#include <stdio.h>
#include <p18f2680.h>
#include <delays.h>
#include <usart.h>
#include "../libcan/can18xx8.h"

#define XTAL 10000000
#define led PORTCbits.RC0

#pragma config OSC = HS
#pragma config FCMEN = OFF
#pragma config IESO = OFF
#pragma config PWRT = OFF

#pragma config WDT = OFF

#pragma config MCLRE = ON



#pragma config LPT1OSC = OFF
#pragma config PBADEN = OFF
#pragma config DEBUG = OFF
#pragma config XINST = OFF
#pragma config BBSIZ = 1024
#pragma config LVP = OFF





/////VARIABLES GLOBALES ////
char x=0;
int i=0;

char message1[8]="Jeremie";
unsigned long id1=0b0011;
char message2[8]="7Robot";
unsigned long id2=0b1100;
char message3[8]="Foy";
unsigned long id3=0b1111;
char message4[8]="Gears3";
unsigned long id4=0b1111;

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
    if(PIE1bits.RCIE && PIR1bits.RCIF)
     {
         char x;
         x = ReadUSART();
         WriteUSART(x);
         PIR1bits.RCIF = 0;
     }


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
        
        /* Direction des ports I in, O out*/
	TRISA = 0b11111111 ;
	TRISB = 0b01111011 ;
	TRISC = 0b11111110;
	
        /* Etat des sorties */
        PORTA = 0b11111111 ;
	PORTB = 0b11111111 ;
	PORTC = 0b11111111 ;

        /* USART */
        OpenUSART( USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE
                & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 77 );

        INTCONbits.GIE=1;
        INTCONbits.PEIE=1;

        /* CAN */
        CANInitialize(1, 2, 6, 3, 2, CAN_CONFIG_ALL_MSG );//1, 4, 5, 4, 2,

        printf("M start\n");
        Delay10KTCYx(20);
       
    
        led=0;
	while(1)
        {
            if(CANIsTxReady())
            {
                //printf("mPret\n");
                CANSendMessage(id1, message1,7,CAN_TX_PRIORITY_0 & CAN_TX_STD_FRAME & CAN_TX_NO_RTR_FRAME );
                led = led^1;
                Delay10KTCYx(200);
                Delay10KTCYx(200);
                Delay10KTCYx(200);
            }
            if(CANIsTxReady())
            {
                //printf("mPret\n");
                CANSendMessage(id2, message2,7,CAN_TX_PRIORITY_0 & CAN_TX_STD_FRAME & CAN_TX_NO_RTR_FRAME );
                led = led^1;
                Delay10KTCYx(200);
                Delay10KTCYx(200);
                Delay10KTCYx(200);
            }
            if(CANIsTxReady())
            {
                //printf("mPret\n");
                CANSendMessage(id3, message3,7,CAN_TX_PRIORITY_0 & CAN_TX_STD_FRAME & CAN_TX_NO_RTR_FRAME );
                led = led^1;
                Delay10KTCYx(200);
                Delay10KTCYx(200);
                Delay10KTCYx(200);
            }
            if(CANIsTxReady())
            {
                //printf("mPret\n");
                CANSendMessage(id4, message4,7,CAN_TX_PRIORITY_0 & CAN_TX_STD_FRAME & CAN_TX_NO_RTR_FRAME );
                led = led^1;
                Delay10KTCYx(200);
                Delay10KTCYx(200);
                Delay10KTCYx(200);
            }


        }
        
}

