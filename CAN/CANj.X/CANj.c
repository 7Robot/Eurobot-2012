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
int cp=0;
long id=0;
char data[8]="";
char len=0;
enum CAN_RX_MSG_FLAGS flag ;



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
    if (PIE3bits.RXB1IE & PIR3bits.RXB1IF)
    {
        cp=0;
       
       while(CANIsRxReady()){
        CANReceiveMessage(&id,data,&len,&flag);
        cp=cp++;
        if (cp==1){
            led=led;
            cp=cp;
        }

        }
        
        led = led^1;
        PIR3bits.RXB1IF=0;
        PIR3bits.ERRIF=0;

    }
        if (PIE3bits.RXB0IE & PIR3bits.RXB0IF)
    {

        while(CANIsRxReady()){
        CANReceiveMessage(&id,data,&len,&flag);
        }

        PIR3bits.RXB0IF=0;
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
	TRISB = 0b01111011 ;//canTX en sortie
	TRISC = 0b11111110;
	
        /* Etat des sorties */
        PORTA = 0b11111111 ;
	PORTB = 0b11111111 ;
	PORTC = 0b11111111 ;

        led =0;
        /* USART */
        OpenUSART( USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE
                & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 77 );

        //INTCONbits.GIE=1;

        CANInitialize(1,2,6,3,2,CAN_CONFIG_VALID_STD_MSG );//jeje modif
        Delay10KTCYx(200);

         /*Config interupt CAN- Buffeur 1*/

        IPR3bits.RXB1IP=1;// : priorité haute par defaut du buff 1
        PIE3bits.RXB1IE=1;//autorise int sur buff1
        PIR3bits.RXB1IF=0;//mise a 0 du flag

                 /*Config interupt CAN- Buffeur 0*/

        IPR3bits.RXB0IP=1;// : priorité haute par defaut du buff 1
        PIE3bits.RXB0IE=1;//autorise int sur buff1
        PIR3bits.RXB0IF=0;//mise a 0 du flag

        /*Config interupt General*/
        INTCONbits.GIE=1;
        INTCONbits.PEIE=1;

        //confif des mask et filtres
        // Set CAN module into configuration mode
        CANSetOperationMode(CAN_OP_MODE_CONFIG);
        // Set Buffer 1 Mask value
        CANSetMask(CAN_MASK_B1, 0b1111,CAN_CONFIG_STD_MSG);
        // Set Buffer 2 Mask value
        CANSetMask(CAN_MASK_B2, 0b1111,CAN_CONFIG_STD_MSG );
        // Set Buffer 1 Filter values
        CANSetFilter(CAN_FILTER_B1_F1,0b0011,CAN_CONFIG_STD_MSG );
        CANSetFilter(CAN_FILTER_B1_F2,0b0000,CAN_CONFIG_STD_MSG );
        CANSetFilter(CAN_FILTER_B2_F1,0b1100,CAN_CONFIG_STD_MSG );
        CANSetFilter(CAN_FILTER_B2_F2,0b0000,CAN_CONFIG_STD_MSG );
        CANSetFilter(CAN_FILTER_B2_F3,0b0000,CAN_CONFIG_STD_MSG );
        CANSetFilter(CAN_FILTER_B2_F4,0b0000,CAN_CONFIG_STD_MSG );

        // Set CAN module into Normal mode
        CANSetOperationMode(CAN_OP_MODE_NORMAL);

        Delay10KTCYx(200);

        printf("J-Yes you CAN !\n");

	while(1)
        {
        
        }
        
}

