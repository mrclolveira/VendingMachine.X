/*
 * File:   Sensors.c
 * Author: mrclolveira
 *
 * Created on 11 de Mar�o de 2018, 16:10
 */


#include "Sensors.h"

#define SensorInLine0 PORTEbits.RE4
#define SensorInLine1 PORTGbits.RG15
#define SensorInLine2 PORTEbits.RE5
#define SensorInLine3 PORTEbits.RE6
#define SensorInLine4 PORTCbits.RC1

#define SensorOutColumn0(x) LATAbits.LATA7 = x
#define SensorOutColumn1(x) LATEbits.LATE0 = x
#define SensorOutColumn2(x) LATEbits.LATE1 = x
#define SensorOutColumn3(x) LATGbits.LATG14 = x
#define SensorOutColumn4(x) LATGbits.LATG12 = x
#define SensorOutColumn5(x) LATGbits.LATG13 = x
#define SensorOutColumn6(x) LATEbits.LATE2 = x
#define SensorOutColumn7(x) LATEbits.LATE3 = x

#define SensorElevatorLevel PORTAbits.RA3
#define SensorElevatorEndLimit PORTAbits.RA2

#define SensorDispenser PORTBbits.RB3

#define SensorPresence PORTFbits.RF3

void SensorsInit(void) {
    TRISEbits.TRISE4 = 1;
    TRISGbits.TRISG15 = 1;
    TRISEbits.TRISE5 = 1;
    TRISEbits.TRISE6 = 1;
    TRISCbits.TRISC1 = 1;

    TRISAbits.TRISA7 = 0;
    ANSAbits.ANSA7 = 0;
    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISGbits.TRISG14 = 0;
    TRISGbits.TRISG12 = 0;
    TRISGbits.TRISG13 = 0;
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE3 = 0;
    SetOnSensor(0xFF, false);

    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA2 = 1;

    TRISBbits.TRISB3 = 1;
    ANSBbits.ANSB3 = 0;
    TRISGbits.TRISG3 = 1;

    TRISFbits.TRISF3 = 1;

    I2C2CONbits.I2CEN = 0;
    I2C3CONbits.I2CEN = 0;
    PMCON1bits.PMPEN = 0;
}


void SetOnSensor(const uint8_t column, uint8_t on) {
    switch (column) {
        case 0:
            SensorOutColumn0(on);
            break;
        case 1:
            SensorOutColumn1(on);
            break;
        case 2:
            SensorOutColumn2(on);
            break;
        case 3:
            SensorOutColumn3(on);
            break;
        case 4:
            SensorOutColumn4(on);
            break;
        case 5:
            SensorOutColumn5(on);
            break;
        case 6:
            SensorOutColumn6(on);
            break;
        case 7:
            SensorOutColumn7(on);
            break;
        default:
            SensorOutColumn0(false);
            SensorOutColumn1(false);
            SensorOutColumn2(false);
            SensorOutColumn3(false);
            SensorOutColumn4(false);
            SensorOutColumn5(false);
            SensorOutColumn6(false);
            SensorOutColumn7(false);
            break;
    }
}

uint8_t IsSensorLineActive(const uint8_t line) {
    uint8_t result = false;
    switch (line) {
        case 0:
            if (SensorInLine0 == false) {
                result = true;
            } else {
                result = false;
            }
            break;
        case 1:
            if (SensorInLine1 == false) {
                result = true;
            } else {
                result = false;
            }
            break;
        case 2:
            if (SensorInLine2 == false) {
                result = true;
            } else {
                result = false;
            }
            break;
        case 3:
            if (SensorInLine3 == false) {
                result = true;
            } else {
                result = false;
            }
            break;
        case 4:
            if (SensorInLine4) {
                result = true;
            } else {
                result = false;
            }
            break;
        default:
            result = false;
            break;
    }
    return result;
}

uint8_t IsDispenserOpen(void) {
    if (SensorDispenser == false) {
        return true;
    }
    return false;
}

uint8_t IsPresenceSensorActive(void) {
    if (SensorPresence == true) {
        return true;
    }
    return false;
}

uint8_t IsElevatorSensorActive(const ElevatorSensors sensor) {
    uint8_t result = false;

    if (sensor == kLevel) {
        if (SensorElevatorLevel == false) {
            result = true;
        } else {
            result = false;
        }
    }
    if (sensor == kEndLimit) {
        if (SensorElevatorEndLimit == false) {
            result = true;
        } else {
            result = false;
        }
    }
    return result;
}