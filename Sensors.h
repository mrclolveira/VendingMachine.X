
#ifndef SENSORS_H
#define	SENSORS_H

#include <xc.h>
#include <stdbool.h>

void SensorsInit(void);

void SetOnSensor(const uint8_t column, uint8_t on);
uint8_t IsSensorLineActive(const uint8_t line);
uint8_t IsDispenserOpen(void);
uint8_t IsPresenceSensorActive(void);

enum sensor_elevator {
    kLevel,
    kEndLimit,
    kEndLimitDown
};
typedef enum sensor_elevator ElevatorSensors;
uint8_t IsElevatorSensorActive(const ElevatorSensors sensor);
uint8_t IsSensorDropOffActive();
uint8_t IsButtonUpActive();
uint8_t IsButtonDownActive();

#endif	// SENSORS_H

