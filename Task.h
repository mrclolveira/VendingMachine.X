
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
bool AlignActuators();
bool SingleActuator(const Payload *value);
bool DoubleActuator(const Payload *value);
bool ReturnElevatorToTop(bool rele);
bool DownToLine(const uint8_t line, uint8_t current_line, bool rele);
bool ActuateAt(bool single, const uint8_t line, const uint8_t row_one, const uint8_t row_two);
bool OpenDispenser(void);
bool CloseDispenser(void);

extern bool close_dispenser_;

#endif	// TASK_H

