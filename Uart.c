/*
 * File:   Uart.c
 * Author: mrclolveira
 *
 * Created on 11 de Março de 2018, 14:00
 */

#include <stdio.h>

#include "Uart.h"
#include "CRC.h"
#include "ProtocollHandler.h"
#include "Task.h"

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
    int i = 0;
    for (i = 0; i < size; i++) {
        while (!U1STAbits.TRMT);
        U1Send(msg[i]);
    }
}

void UartWriteASCII(const char *msg) {
    do {
        while (!U1STAbits.TRMT);
        U1Send(*msg);
    } while (*++msg);
}

void UartWriteInt(const unsigned long num) {
    char buf[100];
    unsigned char i;

    sprintf(buf, " %lu ", num);
    i = 0;
    while (buf[i]!='\0')
    {
        U1Send(buf[i++]);
        while(!U1STAbits.TRMT);
    }
}

void SendAck(const Variable *var) {
    Protocoll ack;
    ack.header_.preamble_ = kFixedPreamble;
    ack.header_.property_ = kAckProperty;
    ack.variable_ = *var;

    Integer crc;
    crc.integer = CRC_HW_calculate(ack.bytes, protocoll_size_ - 2);
    ack.crc_ = crc;
    UartWrite(&ack);
}

void SendRun(const Variable *var) {
    Protocoll run;
    run.header_.preamble_ = kFixedPreamble;
    run.header_.property_ = kRunProperty;
    run.variable_ = *var;

    Integer crc;
    crc.integer = CRC_HW_calculate(run.bytes, protocoll_size_ - 2);
    run.crc_ = crc;
    UartWrite(&run);
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
    uint8_t error = false;
    uint16_t timeout = 0;
    uint8_t i;

    for (i = 0; i < protocoll_size_; i++) {
        incoming_msg.bytes[i] = 0;
    }

    for (i = 0; i < protocoll_size_; i++) {
        while(!U1STAbits.URXDA && (++timeout<1000));
        if (U1STAbits.URXDA) {
            incoming_msg.bytes[i] = U1RXREG;
        } else {
            incoming_msg.bytes[i] = 0;
        }
        timeout = 0;
    }

    // this is just temporary
    if (incoming_msg.bytes[0] == 0x01) {
        ReturnElevatorToTop(true);
    } else if (incoming_msg.bytes[0] == 0x02) {
        ReturnElevatorToTop(false);
    } else if (incoming_msg.bytes[0] == 0x03) {
        DownToLine(2, 4, true);
    } else if (incoming_msg.bytes[0] == 0x04) {
        DownToLine(0, 2, true);
    } else if (incoming_msg.bytes[0] == 0x05) {
        ActuateSingleAt(incoming_msg.bytes[1], incoming_msg.bytes[2]);
    } else if (incoming_msg.bytes[0] == 0x06) {
        ActuateDoubleAt(incoming_msg.bytes[1], incoming_msg.bytes[2], incoming_msg.bytes[3]);
    } else if (incoming_msg.bytes[0] == 0x07) {
        OpenDispenser(true, incoming_msg.bytes[1]);
    } else if (incoming_msg.bytes[0] == 0x08) {
        CloseDispenser();
    } else if (incoming_msg.bytes[0] == 0x09) {
        AlignActuators();
    }

    if (incoming_msg.bytes[0] != kFixedPreamble) {
        error = true;
    }

    Integer crc;
    crc.High = incoming_msg.bytes[protocoll_size_ - 2];
    crc.Low = incoming_msg.bytes[protocoll_size_ - 1];

    if (crc.integer != CRC_HW_calculate(incoming_msg.bytes, protocoll_size_ - 2)) {
        error = true;
    }

    if (error == false) {
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