/*
 * File:   Uart.c
 * Author: mrclolveira
 *
 * Created on 11 de Março de 2018, 14:00
 */

#include "Uart.h"
#include "CRC.h"
#include "ProtocollHandler.h"

// TODO: check pins
void UartInit(void) {
    TRISFbits.TRISF2 = 1;
    TRISFbits.TRISF8 = 0;
    TRISFbits.TRISF4 = 0;
    LATFbits.LATF4 = 1;

    __builtin_write_OSCCONL(OSCCON & 0xbf);
    RPINR18bits.U1RXR = 30;
    RPOR7bits.RP15R = 3;
    __builtin_write_OSCCONL(OSCCON | 0x40);

    U1MODE = 0x8080;
    U1STA = 0x9400;
    U1BRG = 51;

    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;
    IPC2bits.U1RXIP = 7;
    IEC0bits.U1RXIE = 1;
}

void UartWrite(Protocoll *protocoll) {
    protocoll->crc_.integer = CRC_HW_calculate(protocoll->bytes, protocoll_size_ - 2);

    uint8_t it = 0;
    do {
        while(!U1STAbits.TRMT);
        U1Send(protocoll->bytes[it]);
    } while (++it < protocoll_size_);
}

void UartWriteByte(const unsigned char msg[], int size) {
    do {
        while (!U1STAbits.TRMT);
        U1Send(*msg);
    } while (*++msg);
}

void UartWriteASCII(const char *msg) {
    do {
        while (!U1STAbits.TRMT);
        U1Send(*msg);
    } while (*++msg);
}

void SendAck(const Variable *var) {
    Protocoll ack;
    ack.header_.preamble_ = kFixedPreamble;
    ack.header_.property_ = kAckProperty;
    ack.variable_ = *var;

    UartWrite(&ack);
}

void SendNack(void) {
    unsigned char nack[] = {kFixedPreamble, kFaultProperty, 0x00, 0x00};

    Integer crc;
    crc.integer = CRC_HW_calculate(nack, sizeof(nack) - 2);
    nack[sizeof(nack) - 2] = crc.High;
    nack[sizeof(nack) - 1] = crc.Low;

    UartWriteByte(nack, sizeof(nack));
}

void SendUnknownAddress(void) {
    unsigned char uad[] = {kFixedPreamble, 0xFD, 0x00, 0x00};

    Integer crc;
    crc.integer = CRC_HW_calculate(uad, sizeof(uad) - 2);
    uad[sizeof(uad) - 2] = crc.High;
    uad[sizeof(uad) - 1] = crc.Low;

    UartWriteByte(uad, sizeof(uad));
}

void __attribute__((__interrupt__, no_auto_psv )) _ISR _U1RXInterrupt (void) {
    Protocoll incoming_msg;
    bool error = false;
    uint16_t timeout = 0;
    uint8_t i;

    for (i = 0; i < sizeof(incoming_msg); i++) {
        while(!U1STAbits.URXDA && (++timeout<1000));
        if (U1STAbits.URXDA) {
            incoming_msg.bytes[i] = U1RXREG;
        } else {
            incoming_msg.bytes[i] = 0;
            error = true;
        }
    }

    if (incoming_msg.crc_.integer != CRC_HW_calculate(incoming_msg.bytes, sizeof(incoming_msg) - 2)) {
        error = true;
    }

    if (!error) {
        if (!Handle(&incoming_msg)) {
            SendUnknownAddress();
        } else {
            SendAck(&incoming_msg.variable_);
        }
    } else {
        SendNack();
    }

    IFS0bits.U1RXIF = 0;
}