
#ifndef UART_H
#define	UART_H

#include <xc.h> // include processor files - each processor file is guarded.
#include "definitions.h"

#define U1Send(x) U1TXREG = x & 0x00FF

void UartInit(void);
void UartWrite(Protocoll *protocoll);
void UartWriteByte(const unsigned char msg[], int size);
void UartWriteASCII(const char *msg);
void UartWriteInt(const unsigned long num);
void SendAck(const Variable *var);
void SendNack(void);
void SendUnknownAddress(void);
void __attribute__((__interrupt__, auto_psv )) _ISR _U1RXInterrupt(void);


#endif	// UART_H

