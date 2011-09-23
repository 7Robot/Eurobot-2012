/*
   Martin d'Allens, starting 2011-09-20.
 */

#include "ax12.h"

byte checksumAX;

void SetTX() {
    PORTCbits.RC0 = 1;
    PORTCbits.RC1 = 0;
}

void SetRX() {
    PORTCbits.RC0 = 0;
    PORTCbits.RC1 = 1;
}

void PushUSART(byte b) {
    while (BusyUSART());
    WriteUSART(b);
    checksumAX += b;
}

byte PopUSART() {
    int i = 0;
    byte b;

    for (; i < 32767 && !DataRdyUSART(); i++);
    //while(!DataRdyUSART());

    b = ReadUSART();
    checksumAX += b;
    return b; // TODO : negative error ?
}


void SetupAX(/*TODO speed*/) {
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0; // TODO : NOT gate
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF /*TODO*/ & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW,
            12);//9600
}

int RegisterLenAX(byte address) {
    switch (address) {
        case  2: case  3: case  4: case  5: case 11: case 12: case 13: case 16:
        case 17: case 18: case 19: case 24: case 25: case 26: case 27: case 28:
        case 29: case 42: case 43: case 44: case 46: case 47:
            return 1;
        case  0: case  6: case  8: case 14: case 20: case 22: case 30: case 32:
        case 34: case 36: case 38: case 40: case 48:
            return 2;
    }
    return -1;
}

void PushHeaderAX(AX12 ax, int len, byte inst) {
    SetTX();
    
    PushUSART(0xFF);
    PushUSART(0xFF);

    checksumAX = 0;
    PushUSART(ax.id);
    PushUSART(len + 2);
    PushUSART(inst);
}

void PushBufferAX(int len, byte* buf) {
    int i;
    for (i = 0; i < len; i++) {
        PushUSART(buf[i]);
    }
}

void PushFooterAX() {
    PushUSART(~checksumAX);
}

int PopHeaderAX(AX12 ax, int len, byte* buf) {
    byte b;
    int i;
    
    while (BusyUSART()); // Wait for the data to be sent.

    // TODO : ptit delay
    SetRX();

    while (PopUSART() != 0xFF); // Find a frame start.

    if (PopUSART() != 0xFF) // There should have been two in a row.
        return 1;

    checksumAX = 0;

    if(PopUSART() != ax.id) // For ID changes the old ID is returned.
        return 2;
    if(PopUSART() != 2 + len)
        return 3;
    ax.error = PopUSART(); // Error field of the status packet.
    
    for (i = 0; i < len; i++) {
        buf[i] = PopUSART();
    }

    if (~checksumAX != PopUSART())
        return 4;

    return 0; // Success.
}


int PingAX(AX12 ax) {
    PushHeaderAX(ax, 2, AX_INST_PING);
    PushFooterAX();
    // TODO : read
}

int ReadAX(AX12 ax, byte address, int len, byte* buf) {
    //TODO: timeout failsafe ?

    PushHeaderAX(ax, 2, AX_INST_READ_DATA);
    PushUSART(address);
    PushUSART(len);
    PushFooterAX();

//TODO read
    return 0;
}

int WriteAX(AX12 ax, byte address, int len, byte* buf) {
    PushHeaderAX(ax, 1 + len, AX_INST_WRITE_DATA);
    PushUSART(address);
    PushBufferAX(len, buf);
    PushFooterAX();

    return PopHeaderAX(ax, 0, NULL);
}

int RegWriteAX(AX12 ax, byte address, int len, byte* buf) {
    PushHeaderAX(ax, 1 + len, AX_INST_REG_WRITE);
    PushUSART(address);
    PushBufferAX(len, buf);
    PushFooterAX();

    //TODO : read return packet
    return 0;
}

int ActionAX(AX12 ax) {
    PushHeaderAX(ax, 0, AX_INST_ACTION);
    PushFooterAX();
}

int ResetAX(AX12 ax) {
    PushHeaderAX(ax, 0, AX_INST_RESET);
    PushFooterAX();
}



int PutAX(AX12 ax, byte address, int value) {
    return WriteAX(ax, address, RegisterLenAX(address), (byte*)&value /* C18 and AX12 are little-endian */);
}

int GetAX(AX12 ax, byte address) {
    int len = RegisterLenAX(address);
    int value;

    byte ret = ReadAX(ax, address, len, (byte*)&value);
    // TODO handle errors

    return value;
}
