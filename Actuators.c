/*
 * File:   Actuators.c
 * Author: mrclolveira
 *
 * Created on 11 de Março de 2018, 16:25
 */


#include "Actuators.h"

#define ActuatorLine0(x) LATDbits.LATD7 = x
#define ActuatorLine1(x) LATFbits.LATF0 = x
#define ActuatorLine2(x) LATFbits.LATF1 = x
#define ActuatorLine3(x) LATGbits.LATG1 = x
#define ActuatorLine4(x) LATGbits.LATG0 = x

#define ActuatorColumn0(x) LATDbits.LATD1 = x
#define ActuatorColumn1(x) LATDbits.LATD2 = x
#define ActuatorColumn2(x) LATDbits.LATD3 = x
#define ActuatorColumn3(x) LATDbits.LATD12 = x
#define ActuatorColumn4(x) LATDbits.LATD13 = x
#define ActuatorColumn5(x) LATDbits.LATD4 = x
#define ActuatorColumn6(x) LATDbits.LATD5 = x
#define ActuatorColumn7(x) LATDbits.LATD6 = x

#define ActuatorLock(x) LATDbits.LATD0 = x

#define ActuatorDispenser(x) LATDbits.LATD11 = x

#define ActuatorElevatorUp(x) LATDbits.LATD10 = x
#define ActuatorElevatorDown(x) LATDbits.LATD9 = x

void ActuatorsInit(void) {
    TRISDbits.TRISD7 = 0;
    ANSDbits.ANSD7 = 0;
    TRISFbits.TRISF0 = 0;
    ANSFbits.ANSF0 = 0;
    TRISFbits.TRISF1 = 0;
    TRISGbits.TRISG1 = 0;
    TRISGbits.TRISG0 = 0;

    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD12 = 0;
    TRISDbits.TRISD13 = 0;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
    ANSDbits.ANSD6 = 0;

    TRISDbits.TRISD0 = 0;

    TRISDbits.TRISD11 = 0;

    TRISDbits.TRISD10 = 0;
    TRISDbits.TRISD9 = 0;

    CM3CONbits.CEN = 0;
    IEC0bits.INT0IE = 0;
}


void SetLineMotorOn(const uint8_t line, bool on) {
    switch (line) {
        case 0:
            ActuatorLine0(on);
            break;
        case 1:
            ActuatorLine1(on);
            break;
        case 2:
            ActuatorLine2(on);
            break;
        case 3:
            ActuatorLine3(on);
            break;
        case 4:
            ActuatorLine4(on);
            break;
        default:
            ActuatorLine0(false);
            ActuatorLine1(false);
            ActuatorLine2(false);
            ActuatorLine3(false);
            ActuatorLine4(false);
            break;
    }
}

void SetColumnMotorOn(const uint8_t column, bool on) {
    switch (column) {
        case 0:
            ActuatorColumn0(on);
            break;
        case 1:
            ActuatorColumn1(on);
            break;
        case 2:
            ActuatorColumn2(on);
            break;
        case 3:
            ActuatorColumn3(on);
            break;
        case 4:
            ActuatorColumn4(on);
            break;
        case 5:
            ActuatorColumn5(on);
            break;
        case 6:
            ActuatorColumn6(on);
            break;
        case 7:
            ActuatorColumn7(on);
            break;
        default:
            ActuatorColumn0(false);
            ActuatorColumn1(false);
            ActuatorColumn2(false);
            ActuatorColumn3(false);
            ActuatorColumn4(false);
            ActuatorColumn5(false);
            ActuatorColumn6(false);
            ActuatorColumn7(false);
            break;
    }
}

void SetLockOn(bool on) {
    ActuatorLock(on);
}

void SetDispenserOn(bool on) {
    ActuatorDispenser(on);
}

void SetElevatorOn(const ElevatorDirection direction, bool on) {
    if (direction == kUp) {
        ActuatorElevatorUp(on);
    } else {
        ActuatorElevatorDown(on);
    }
}