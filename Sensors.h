
#ifndef SENSORS_H
#define	SENSORS_H

#include <xc.h>
#include <stdbool.h>

void SensorsInit(void);

void SetOnSensor(const uint8_t column, bool on);
bool IsSensorLineActive(const uint8_t line);
bool IsDispenserOpen(void);
bool IsPresenceSensorActive(void);

enum sensor_elevator {
    kLevel,
    kEndLimit
};
typedef enum sensor_elevator ElevatorSensors;
bool IsElevatorSensorActive(const ElevatorSensors sensor);

#endif	// SENSORS_H

