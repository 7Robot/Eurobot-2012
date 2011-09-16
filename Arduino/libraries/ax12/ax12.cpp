/*
  ax12.cpp - arbotiX Library for AX-12 Servos
  Copyright (c) 2008,2009 Michael E. Ferguson.  All right reserved.
  Modificada el 15/11/09 por Pablo Gindel.
  versión 2.0 - 10/02/10
*/

#include "wiring.h"         // we need this for the serial port defines
#include "ax12.h"
#include <avr/interrupt.h>

/******************************************************************************
 * Hardware Serial Level, this uses the same stuff as Serial, therefore
 *  you should not use the Arduino Serial library.
 ******************************************************************************/

/** helper functions to emulate half-duplex */
void AX12::setTX () {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
    bitClear(UCSR0B, RXCIE0);    // deshabilita la interrupción de recepción
    bitClear(UCSR0B, RXEN0);     // deshabilila la recepción
    bitSet(UCSR0B, TXEN0);       // habilita la trasmisión
#elif defined (__AVR_ATmega1280__) || defined (__AVR_ATmega128__)    
    bitClear(UCSR1B, RXCIE1);    // deshabilita la interrupción de recepción
    bitClear(UCSR1B, RXEN1);     // deshabilila la recepción
    bitSet(UCSR1B, TXEN1);       // habilita la trasmisión
#elif defined (__AVR_ATmega8__)
    bitClear(UCSRB, RXCIE);    // deshabilita la interrupción de recepción
    bitClear(UCSRB, RXEN);     // deshabilila la recepción
    bitSet(UCSRB, TXEN);       // habilita la trasmisión
#endif    
}

void AX12::setRX () {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
    bitClear(UCSR0B, TXEN0);   // deshabilita la trasmisión
    bitSet(UCSR0B, RXEN0);     // habilita la recepción
    bitSet(UCSR0B, RXCIE0);    // habilita la interrupción de recepción
#elif defined (__AVR_ATmega1280__) || defined (__AVR_ATmega128__)
    bitClear(UCSR1B, TXEN1);   // deshabilita la trasmisión
    bitSet(UCSR1B, RXEN1);     // habilita la recepción
    bitSet(UCSR1B, RXCIE1);    // habilita la interrupción de recepción
#elif defined (__AVR_ATmega8__)
    bitClear(UCSRB, TXEN);   // deshabilita la trasmisión
    bitSet(UCSRB, RXEN);     // habilita la recepción
    bitSet(UCSRB, RXCIE);    // habilita la interrupción de recepción 
#endif    
    ax_rx_Pointer = 0;         // resetea el puntero del buffer
}

void AX12::setNone () {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
    bitClear(UCSR0B, RXCIE0);    // deshabilita la interrupción de recepción
    bitClear(UCSR0B, RXEN0);     // deshabilila la recepción
    bitClear(UCSR0B, TXEN0);     // deshabilita la trasmisión
#elif defined (__AVR_ATmega1280__) || defined (__AVR_ATmega128__)
    bitClear(UCSR1B, RXCIE1);    // deshabilita la interrupción de recepción
    bitClear(UCSR1B, RXEN1);     // deshabilila la recepción
    bitClear(UCSR1B, TXEN1);     // deshabilita la trasmisión
#elif defined (__AVR_ATmega8__)
    bitClear(UCSRB, RXCIE);    // deshabilita la interrupción de recepción
    bitClear(UCSRB, RXEN);     // deshabilila la recepción
    bitClear(UCSRB, TXEN);     // deshabilita la trasmisión 
#endif    
}

/** Sends a character out the serial port */
byte AX12::ax12writeB (byte data) {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
    while (bit_is_clear(UCSR0A, UDRE0));    // espera que el micro esté pronto para trasmitir
    UDR0 = data;                            // escribe el byte a trasmitir
#elif defined (__AVR_ATmega1280__) || defined (__AVR_ATmega128__)
    while (bit_is_clear(UCSR1A, UDRE1));    // espera que el micro esté pronto para trasmitir
    UDR1 = data;                            // escribe el byte a trasmitir
#elif defined (__AVR_ATmega8__)
    while (bit_is_clear(UCSRA, UDRE));    // espera que el micro esté pronto para trasmitir
    UDR = data;                            // escribe el byte a trasmitir  
#endif     
    return data;
}

/** We have a one-way recieve buffer, which is reset after each packet is receieved.
    A wrap-around buffer does not appear to be fast enough to catch all bytes at 1Mbps. */
byte AX12::ax_rx_buffer[AX12_BUFFER_SIZE];
volatile byte AX12::ax_rx_Pointer;
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
ISR (USART_RX_vect) {
    AX12::ax_rx_buffer[(AX12::ax_rx_Pointer++)] = UDR0;    // esta es la rutina de interrupción de recepción
}
#elif defined (__AVR_ATmega1280__) || defined (__AVR_ATmega128__)
ISR (USART1_RX_vect) {
    AX12::ax_rx_buffer[(AX12::ax_rx_Pointer++)] = UDR1;    // esta es la rutina de interrupción de recepción
}                                                          // lo que hace es meter el byte recibido en el buffer
#elif defined (__AVR_ATmega8__)
SIGNAL (SIG_UART_RECV) {
    AX12::ax_rx_buffer[(AX12::ax_rx_Pointer++)] = UDR;    // esta es la rutina de interrupción de recepción
} 
#endif    

/** initializes serial transmit at baud, 8-N-1 */
void AX12::init (long baud) {
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
    bitClear (UCSR0A, U2X0); 
    UBRR0H = ((F_CPU / 16 + baud / 2) / baud - 1) >> 8;
    UBRR0L = ((F_CPU / 16 + baud / 2) / baud - 1);        // setea la velocidad del USART
#elif defined (__AVR_ATmega1280__) || defined (__AVR_ATmega128__)
    bitClear (UCSR1A, U2X1); 
    UBRR1H = ((F_CPU / 16 + baud / 2) / baud - 1) >> 8;
    UBRR1L = ((F_CPU / 16 + baud / 2) / baud - 1);        // setea la velocidad del USART
#elif defined (__AVR_ATmega8__)
    bitClear (UCSRA, U2X); 
    UBRRH = ((F_CPU / 16 + baud / 2) / baud - 1) >> 8;
    UBRRL = ((F_CPU / 16 + baud / 2) / baud - 1);        // setea la velocidad del USART
#endif    
    ax_rx_Pointer = 0;
    // deshabilita tanto recepción como trasmisión
    setNone();
}

/******************************************************************************
 * Constructores, etc.
 ******************************************************************************/

void AX12::autoDetect (int* list_motors, byte num_motors) {
    byte index = 0;
    byte *data;
    int _id, _error, _data;
    for (byte i=0; i<255; i++) {
      ax12SendPacket (i, 0, AX_PING, data);
      ax12ReadPacket (&_id, &_error, &_data);
      if (_id==i) {
        list_motors[index] = i;
        index++;
        if (index==num_motors) {break;}
      }
    }
    for (byte i=index; i<num_motors; i++) {list_motors[i]=-1;}
}

/** constructors */
AX12::AX12 (long baud, byte motor_id, boolean inv) {
    id = motor_id;
    inverse = inv;
    init (baud);
}

AX12::AX12 (byte motor_id, boolean inv) {
    id = motor_id;
    inverse = inv;
}

AX12::AX12 (long baud, byte motor_id) {
    id = motor_id;
    inverse = false;
    init (baud);
}

AX12::AX12 (byte motor_id) {
    id = motor_id;
    inverse = false;
}

AX12::AX12 () {
    id = 0xFD;
    inverse = false;
}

/******************************************************************************
 * Packet Level
 ******************************************************************************/

/** send instruction packet */
void AX12::ax12SendPacket (byte _id, byte datalength, byte instruction, byte* data) {
    byte checksum = 0;
    setTX();
    ax12writeB(0xFF);
    ax12writeB(0xFF);
    checksum += ax12writeB(_id);
    checksum += ax12writeB(datalength + 2);
    checksum += ax12writeB(instruction);
    for (byte f=0; f<datalength; f++) {     // data = parámetros
      checksum += ax12writeB(data[f]);
    }
    // checksum =
    ax12writeB(~checksum);
    setRX();
}

/** read status packet
/** retorna el código interno de error; 0 = OK */
byte AX12::ax12ReadPacket (int* status_id, int* status_error, int* status_data) {
    unsigned long ulCounter;
    byte timeout, error, status_length, checksum, offset;
    byte volatile bcount;
    // primero espera que llegue toda la data
    offset = 0; timeout = 0; bcount = 0;
    while(bcount < 13){        // 10 es el largo máximo que puede tener un packet
        ulCounter = 0;
        while((bcount + offset) == ax_rx_Pointer){
            if(ulCounter++ > 1000L){  // was 3000
                timeout = 1;
                break;
            }
        }
        if (timeout) break;
        if ((bcount == 0) && (ax_rx_buffer[offset] != 0xff)) offset++;
        else bcount++;
    }
    setNone();
    // ahora decodifica el packet
    // corrección de cabecera
    error = 0;                                             // código interno de error
    do {
        error++;
        offset++;
        bcount--;
    } while (ax_rx_buffer[offset] == 255);
    if (error > 1) error =0;                               // prueba de cabecera
    // offset = primer byte del mensaje (sin cabecera)
    // bcount = largo del mensaje leido (sin cabecera)
    status_length = 2 + ax_rx_buffer[offset+1];            // largo del mensaje decodificado
    if (bcount != status_length) error+=2;                 // prueba de coherencia de data
    checksum = 0;                                          // cálculo de checksum
    for (byte f=0; f<status_length; f++)
        checksum += ax_rx_buffer[offset+f];
    if (checksum != 255) error+=4;                          // prueba de checksum
    if (error == 0) {
        *status_id = ax_rx_buffer[offset];
        *status_error = ax_rx_buffer[offset+2];
        switch (status_length) {
            case 5: *status_data = ax_rx_buffer[offset+3]; break;
            case 6: *status_data = makeInt (ax_rx_buffer[offset+3], ax_rx_buffer[offset+4]); break;
            default: *status_data = offset+3;
        }
    } else {
        *status_id = -1;
        *status_error = -1;
        *status_data = -1;
    }
    return error;
}

/******************************************************************************
 * Instruction Level
 ******************************************************************************/

/** ping */
byte AX12::ping () {
     byte* data;
     ax12SendPacket (id, 0, AX_PING, data);
     return ax12ReadPacket(&status_id, &status_error, &status_data);
}

/** reset */
byte AX12::reset () {
     byte* data;
     ax12SendPacket (id, 0, AX_RESET, data);
     return ax12ReadPacket(&status_id, &status_error, &status_data);
}

/** action */
byte AX12::action () {
     byte *data;
     ax12SendPacket (id, 0, AX_ACTION, data);
     return ax12ReadPacket(&status_id, &status_error, &status_data);
}

/** read data */
byte AX12::readData (byte regstart, byte reglength) {
    byte data [2];
    data [0] = regstart; data [1] = reglength;
    ax12SendPacket (id, 2, AX_READ_DATA, data);
    return ax12ReadPacket(&status_id, &status_error, &status_data);
}

/** write data */
byte AX12::writeData (byte regstart, byte reglength, int value) {
    byte data [reglength+1];
    data [0] = regstart; data [1] = value&0xFF;
    if (reglength > 1) {data[2] = highByte(value);}
    ax12SendPacket (id, reglength+1, AX_WRITE_DATA, data);
    return ax12ReadPacket(&status_id, &status_error, &status_data);
}

/** reg write */
byte AX12::regWrite (byte regstart, byte reglength, int value) {
    byte data [reglength+1];
    data [0] = regstart; data [1] = value&0xFF;
    if (reglength > 1) {data[2] = highByte(value);}
    ax12SendPacket (id, reglength+1, AX_REG_WRITE, data);
    return ax12ReadPacket(&status_id, &status_error, &status_data);
}

/* falta implementar:
                     SYNC WRITE
                     decodificador de error
                     alto nivel
*/

/******************************************************************************
 * High Level
 ******************************************************************************/

/** "intelligent" read data */
byte AX12::readInfo (byte regstart) {
    byte reglength = 0;
    switch (regstart) {
      case 2: case 3: case 4:
      case 5: case 11: case 12: case 13: case 16:
      case 17: case 18: case 19: case 24: case 25:
      case 26: case 27: case 28: case 29: case 42:
      case 43: case 44: case 46: case 47: reglength = 1; break;
      case 0: case 6: case 8: case 14: case 20: case 22:
      case 30: case 32: case 34: case 36: case 38:
      case 40: case 48: reglength = 2; break;
    }
    if (reglength==0) {return 255;}
    byte data [2];
    data [0] = regstart; data [1] = reglength;
    ax12SendPacket (id, 2, AX_READ_DATA, data);
    byte err = ax12ReadPacket(&status_id, &status_error, &status_data);
    if (regstart==36 && inverse) {    // este if se va a completar con todo aquello que deba ser invertido
      status_data = 1023-status_data;
    }
    return err;
}

/** "intelligent" write data */
byte AX12::writeInfo (byte regstart, int value) {
    byte reglength = 0;
    switch (regstart) {
      case 3: case 4: case 5: case 11: case 12:
      case 13: case 16: case 17: case 18: case 19:
      case 24: case 25: case 26: case 27: case 28:
      case 29: case 44: case 47: reglength = 1; break;
      case 6: case 8: case 14: case 30: case 32:
      case 34: case 48: reglength = 2; break;
    }
    if (reglength==0) {return 255;}
    if (regstart==30 && inverse) {    // este if se va a completar con todo aquello que deba ser invertido
      value = 1023-value;
    }
    byte data [reglength+1];
    data [0] = regstart; data [1] = value&0xFF;
    if (reglength > 1) {data[2] = (value&0xFF00)>>8;}
    ax12SendPacket (id, reglength+1, AX_WRITE_DATA, data);
    return ax12ReadPacket(&status_id, &status_error, &status_data);
}

void AX12::setEndlessTurnMode (boolean onoff) {
    writeInfo (CW_ANGLE_LIMIT, 0);
    if (onoff) {
        writeInfo (CCW_ANGLE_LIMIT, 0);
    } else {
        writeInfo (CCW_ANGLE_LIMIT, 1023);
    }
}

void AX12::endlessTurn (int velocidad) {
    boolean direccion = sign2bin (velocidad);
    writeInfo (MOVING_SPEED, abs(velocidad)|((direccion^inverse)<<10));
}

byte AX12::presentPSL (int* PSL) {                                // lee position, speed & load de una sola vez
    byte err = readData (PRESENT_POSITION, 6);
    for (byte f=0; f<3; f++) {
        PSL [f] = makeInt (ax_rx_buffer[status_data+2*f], ax_rx_buffer[status_data+1+2*f]);
        if (f>0) {
            PSL[f] = (PSL[f]&0x03FF) * bin2sign((PSL[f]>0x03FF)^inverse);
        }
    }
    if (inverse) {PSL[0] = 1023-PSL[0];}
    return err;
}


/******************************************************************************
 * Misc.
 ******************************************************************************/

boolean sign2bin (int numero) {         // numero > 0 --> true; numero <= 0 --> false
    return (numero > 0);
}

char bin2sign (boolean var) {           // var = 0 --> sign = -1; var = 1 --> sign = 1
    return 2*var - 1;
}

int makeInt (byte l, byte h) {
    return (h << 8) | l;
}
