
#ifndef ACTUATORS_H
#define	ACTUATORS_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdbool.h>

void ActuatorsInit(void);

void SetLineMotorOn(const uint8_t line, uint8_t on);
void SetColumnMotorOn(const uint8_t column, uint8_t on);
void SetLockOn(uint8_t on);
void SetDispenserOn(uint8_t on);

enum Direction {
    kUp,
    kDown,
    kStoped
};
typedef enum Direction ElevatorDirection;
void SetElevatorOn(const ElevatorDirection direction, uint8_t rele);
void SetDropOffOn(uint8_t on);

#endif	// ACTUATORS_H

