
#ifndef TASK_H
#define	TASK_H

#include <xc.h>
#include <stdbool.h>
#include "definitions.h"

#define kLimitTopLIne 4
#define kLimitBottomLine 0

void InitLedThread(void);
void __attribute__((__interrupt__, auto_psv )) _ISR _RTCCInterrupt(void);
void UnlockDoor(const Payload *value);
uint8_t AlignActuators();
uint8_t SingleActuator(const Payload *value);
uint8_t DoubleActuator(const Payload *value);
uint8_t ReturnElevatorToTop(uint8_t rele);
uint8_t DownToLine(const uint8_t line, uint8_t current_line, uint8_t rele);
uint8_t ActuateAt(uint8_t single, const uint8_t line, const uint8_t row_one, const uint8_t row_two);
uint8_t OpenDispenser(uint8_t rele);
uint8_t CloseDispenser(void);

extern uint8_t close_dispenser_;

#endif	// TASK_H

