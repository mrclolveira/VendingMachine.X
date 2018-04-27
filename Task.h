
#ifndef TASK_H
#define	TASK_H

#include <xc.h>
#include <stdbool.h>
#include "definitions.h"

#define kLastLineOnTop 4
#define kLimitBottomLine 0
#define kDefaultTimeToClose 8
#define kNumberOfLines 5
#define kNumberOfColumns 7

void InitLedThread(void);
void __attribute__((__interrupt__, auto_psv )) _ISR _RTCCInterrupt(void);
void MainTask(void);
uint8_t SetSendPresenceStatus(const Payload *value);
void UnlockDoor(const Payload *value);
uint8_t AlignActuators();
uint8_t SingleActuator(const Payload *value);
uint8_t DoubleActuator(const Payload *value);
uint8_t ReturnElevatorToTop(uint8_t rele);
uint8_t GoElevatorToDown(uint8_t rele);
uint8_t DownToLine(const uint8_t line, uint8_t current_line, uint8_t rele);
uint8_t ActuateSingleAt(const uint8_t line, const uint8_t row);
uint8_t ActuateDoubleAt(const uint8_t line, const uint8_t row_one, const uint8_t row_two);
uint8_t OpenDispenser(uint8_t rele, uint8_t turn_off_elevator);
uint8_t CloseDispenser(uint8_t time_to_close);

#endif	// TASK_H

