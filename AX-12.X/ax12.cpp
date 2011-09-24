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

    // TODO : reduce the delay in the middle of packets.
    // TODO : check status packet avec la LED
    // The maximum Return Delay Time is of 254 * 2µs so we should wait more than
    // 0.5 ms, corresponing to 1250 clock cycles at 8MHz. Maybe.
    //for (; i < 2000 && !DataRdyUSART(); i++);

    b = ReadUSART(); // Non blocking.
    checksumAX += b;
    return b; // Errors will be detected by the checksum.
}


void SetupAX(/*TODO speed*/) {
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE
            & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW,
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
    return 0;
}

void PushHeaderAX(AX12 ax, int len, byte inst) {
    SetTX();
    
    PushUSART(0xFF);
    PushUSART(0xFF);

    checksumAX = 0; // The first two bytes don't count.
    PushUSART(ax.id);
    PushUSART(len + 2); // Bytes to go : instruction + buffer (len) + checksum.
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

int PopReplyAX(AX12 ax, int len, byte* buf) {
    int i;
    
    while (BusyUSART()); // Wait for the data to be sent.

    SetRX();

    for (i = 0; i < 20 && PopUSART() != 0xFF; i++); // Find a frame start.

    if (PopUSART() != 0xFF) // There should have been two in a row.
        return 2;

    checksumAX = 0;

    if(PopUSART() != ax.id  /* For ID changes the old ID is returned. */
            && ax.id != AX_BROADCAST)
        return 3;
    if(PopUSART() != 2 + len)
        return 4;
    *((byte*)&ax.errorbits) = PopUSART(); // Error field of the status packet.
    
    for (i = 0; i < len; i++) {
        buf[i] = PopUSART();
    }

    if (~checksumAX != PopUSART())
        return 5;

    return 0; // Success.
}


int PingAX(AX12 ax) {
    PushHeaderAX(ax, 2, AX_INST_PING);
    PushFooterAX();

    return PopReplyAX(ax, 0, NULL); // Ping always triggers a status packet.
}

int ReadAX(AX12 ax, byte address, int len, byte* buf) {
    PushHeaderAX(ax, 2, AX_INST_READ_DATA);
    PushUSART(address);
    PushUSART(len);
    PushFooterAX();

    return PopReplyAX(ax, len,  buf); // Might work with AX_BROADCAST.
}

int WriteAX(AX12 ax, byte address, int len, byte* buf) {
    PushHeaderAX(ax, 1 + len, AX_INST_WRITE_DATA);
    PushUSART(address);
    PushBufferAX(len, buf);
    PushFooterAX();

    if(ax.id == AX_BROADCAST)
        return 0;
    else
        return PopReplyAX(ax, 0, NULL);
}

int RegWriteAX(AX12 ax, byte address, int len, byte* buf) {
    PushHeaderAX(ax, 1 + len, AX_INST_REG_WRITE);
    PushUSART(address);
    PushBufferAX(len, buf);
    PushFooterAX();

    if(ax.id == AX_BROADCAST)
        return 0;
    else
        return PopReplyAX(ax, 0, NULL);
}

int ActionAX(AX12 ax) {
    PushHeaderAX(ax, 0, AX_INST_ACTION);
    PushFooterAX();

    if(ax.id == AX_BROADCAST) // Most likely.
        return 0;
    else
        return PopReplyAX(ax, 0, NULL);
}

int ResetAX(AX12 ax) {
    PushHeaderAX(ax, 0, AX_INST_RESET);
    PushFooterAX();

    if(ax.id == AX_BROADCAST)
        return 0;
    else
        return PopReplyAX(ax, 0, NULL);
}



int PutAX(AX12 ax, byte address, int value) {
    int i = RegisterLenAX(address);
    return WriteAX(ax, address, RegisterLenAX(address),
                   (byte*)&value /* C18 and AX12 are little-endian */);
}

int GetAX(AX12 ax, byte address) {
    int value;
    int len = RegisterLenAX(address);
    
    byte err = ReadAX(ax, address, len, (byte*)&value);
    if(err)
        return -1;

    return value;
}
