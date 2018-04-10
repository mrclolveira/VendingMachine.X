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

#define ActuatorElevatorUp(x) LATAbits.LATA4 = x
#define ActuatorElevatorDown(x) LATAbits.LATA5 = x
#define ActuatorElevatorReleUp(x) LATDbits.LATD10 = x
#define ActuatorElevatorReleDown(x) LATDbits.LATD9 = x

void ActuatorsInit(void) {
    TRISDbits.TRISD7 = 0;
    ANSDbits.ANSD7 = 0;
    TRISFbits.TRISF0 = 0;
    ANSFbits.ANSF0 = 0;
    TRISFbits.TRISF1 = 0;
    TRISGbits.TRISG1 = 0;
    TRISGbits.TRISG0 = 0;
    SetLineMotorOn(0xFF, false);

    TRISDbits.TRISD1 = 0;
    TRISDbits.TRISD2 = 0;
    TRISDbits.TRISD3 = 0;
    TRISDbits.TRISD12 = 0;
    TRISDbits.TRISD13 = 0;
    TRISDbits.TRISD4 = 0;
    TRISDbits.TRISD5 = 0;
    TRISDbits.TRISD6 = 0;
    ANSDbits.ANSD6 = 0;
    SetColumnMotorOn(0xFF, false);

    TRISDbits.TRISD0 = 0;
    ActuatorLock(false);

    TRISDbits.TRISD11 = 0;
    ActuatorDispenser(false);

    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;
//    SetElevatorOn(kStoped, false);

    TRISDbits.TRISD10 = 0;
    TRISDbits.TRISD11 = 0;
    SetElevatorOn(kStoped, true);

    CM3CONbits.CEN = 0;
    IEC0bits.INT0IE = 0;
}


void SetLineMotorOn(const uint8_t line, uint8_t on) {
    uint8_t on_invert = false;
    if (on == false) {
        on_invert = true;
    } else {
        on_invert = false;
    }
    switch (line) {
        case 0:
            ActuatorLine0(on_invert);
            break;
        case 1:
            ActuatorLine1(on_invert);
            break;
        case 2:
            ActuatorLine2(on_invert);
            break;
        case 3:
            ActuatorLine3(on_invert);
            break;
        case 4:
            ActuatorLine4(on_invert);
            break;
        default:
            ActuatorLine0(true);
            ActuatorLine1(true);
            ActuatorLine2(true);
            ActuatorLine3(true);
            ActuatorLine4(true);
            break;
    }
}

void SetColumnMotorOn(const uint8_t column, uint8_t on) {
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

void SetLockOn(uint8_t on) {
    ActuatorLock(on);
}

void SetDispenserOn(uint8_t on) {
    ActuatorDispenser(on);
}

void SetElevatorOn(const ElevatorDirection direction, uint8_t rele) {
    if (direction == kUp) {
        if (rele) {
            ActuatorElevatorReleUp(true);
            ActuatorElevatorReleDown(false);
        } else {
            ActuatorElevatorUp(true);
            ActuatorElevatorDown(false);
        }
    } else if (direction == kDown) {
        if (rele) {
            ActuatorElevatorReleUp(false);
            ActuatorElevatorReleDown(true);
        } else {
            ActuatorElevatorUp(false);
            ActuatorElevatorDown(true);
        }
    } else {
        if (rele) {
            ActuatorElevatorReleUp(false);
            ActuatorElevatorReleDown(false);
        } else {
            ActuatorElevatorUp(false);
            ActuatorElevatorDown(false);
        }
    }
}