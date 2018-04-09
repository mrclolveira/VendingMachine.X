
#ifndef ACTUATORS_H
#define	ACTUATORS_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdbool.h>

void ActuatorsInit(void);

void SetLineMotorOn(const uint8_t line, bool on);
void SetColumnMotorOn(const uint8_t column, bool on);
void SetLockOn(bool on);
void SetDispenserOn(bool on);

enum Direction {
    kUp,
    kDown,
    kStoped
};
typedef enum Direction ElevatorDirection;
void SetElevatorOn(const ElevatorDirection direction, bool rele);

#endif	// ACTUATORS_H

