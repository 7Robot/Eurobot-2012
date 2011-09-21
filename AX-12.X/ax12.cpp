/*
   Martin d'Allens, starting 2011-09-20.
 */

#include "ax12.h"

void SetupAX(/*TODO speed*/) {
    OpenUSART(USART_TX_INT_OFF & USART_RX_INT_OFF /*TODO*/ & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW,
            12);
}

int RegisterLen(byte address) {
    switch (address) {
        case 2: case 3: case 4: case 5: case 11: case 12: case 13: case 16:
        case 17: case 18: case 19: case 24: case 25: case 26: case 27: case 28:
        case 29: case 42: case 43: case 44: case 46: case 47:
            return 1;
        case 0: case 6: case 8: case 14: case 20: case 22: case 30: case 32:
        case 34: case 36: case 38: case 40: case 48:
            return 2;
    }
    return -1;
}

byte PutAX(AX12 ax, byte address, int value) {
    return WriteAX(ax, address, RegisterLen(address), &value /* C18 and AX12 are little-endian */);
}

int GetAX(AX12 ax, byte address) {
    int len = RegisterLen(address);
    int value;

    byte ret = ReadAX(ax, address, len, &value);
    // TODO : check error

    return value;
}

void PushUSART(byte b) {
    while (BusyUSART());
    WriteUSART(b);
}

byte WriteAX(AX12 ax, byte address, int len, byte* buf) {
    int i;
    int sum;

    PushUSART(0xFF);
    PushUSART(0xFF);
    PushUSART(ax.id);
    PushUSART(len + 3);
    PushUSART(AX_INST_WRITE_DATA);
    PushUSART(address);

    sum = ax.id + len + 3 + AX_INST_WRITE_DATA + address;

    ///TODO : PushBufSumAX(buf, sum)
    for (i = 0; i < len; i++) {
        PushUSART(buf[i]);
        sum += buf[i];
    }
    PushUSART(~sum);

    //TODO : read return packet
    return 0;
}

byte PopUSART() {
    int i = 0;
    for (; i < 32767 && !DataRdyUSART(); i++);
    return ReadUSART(); // TODO : negative error
}

byte ReadAX(AX12 ax, byte address, int len, byte* buf) {
    byte b;
    int sum;
    int i;
    //TODO: timeout failsafe ?

    PushUSART(0xFF);
    PushUSART(0xFF);
    PushUSART(ax.id);
    PushUSART(4);
    PushUSART(AX_INST_READ_DATA);
    PushUSART(address);
    PushUSART(len);
    PushUSART(~(ax.id + 4 + AX_INST_READ_DATA + address + len));

    while ((b = PopUSART()) != 0xFF);
    if (PopUSART() != 0xFF /* There should have been two in a row. */
            || PopUSART() != ax.id
            || PopUSART() != 2 + len)
        return 0; //-1

    if ((b = PopUSART()) != 0) // Error is zero.
        return b; // TODO: sens d'une erreur ?

    sum = ax.id + 2 + len /* + error */;

    for (i = 0; i < len; i++) {
        buf[i] = PopUSART();
        sum += buf[i];
    }
    if (~sum != PopUSART())
        return 0; //-1

    return 0;
}
