/*
 * File:   Task.c
 * Author: mrclolveira
 *
 * Created on 13 de Março de 2018, 00:18
 */

#include "Task.h"
#include "Actuators.h"
#include "System.h"
#include "Sensors.h"
#include "LED.h"
#include <libpic30.h>

void InitLedThread(void) {
    __builtin_write_OSCCONL( 0x02 );
    __builtin_write_RTCWEN();
    RCFGCALbits.RTCSYNC = 1;
    RCFGCALbits.RTCOE = 0;
    RCFGCALbits.CAL = 0;
    RCFGCALbits.RTCEN = 1;

    ALCFGRPTbits.ALRMEN = 1;
    ALCFGRPTbits.CHIME = 1;
    ALCFGRPTbits.AMASK = 0b0001;
    ALCFGRPTbits.ARPT = 0b11111111;

    IFS3bits.RTCIF = 0;
    IPC15bits.RTCIP = 6;
    IEC3bits.RTCIE = 1;

    RCFGCALbits.RTCWREN = 0;    //Disabled RTCC value write
}

void __attribute__((__interrupt__, auto_psv )) _ISR _RTCCInterrupt(void) {
    if (close_dispenser_ || IFS0bits.U1RXIF) {
        return;
    }

    SetLedOn(!LATFbits.LATF5);
    IFS3bits.RTCIF = 0;
}

void UnlockDoor(const Payload *value) {
    SetLockOn(true);

    uint8_t buffer[4] = { value->Byte_4, value->Byte_3, value->Byte_2,
                          value->Byte_1 };
    uint32_t time = *(uint32_t *)&buffer;
    __delay_ms(time);
    SetLockOn(false);
}

bool AlignActuators() {
    int line, column;
    for (line = 0; line < 5; line++) {
        for (column = 0; column < 8; column++) {
            if (!ActuateAt(true, line, column, 0xFF)) {
                return false;
            }
        }
    }
    return true;
}

bool SingleActuator(const Payload *value) {
    if (!ReturnElevatorToTop()) {
        return false;
    }
    if (!DownToLine(value->Byte_4, kLimitTopLIne)) {
        return false;
    }
    __delay_ms(100);
    if (!ActuateAt(true, value->Byte_4, value->Byte_3, 0xFF)) {
        return false;
    }
    __delay_ms(200);
    if (!DownToLine(kLimitBottomLine, value->Byte_4)) {
        return false;
    }
    __delay_ms(300);
    if (!OpenDispenser()) {
        return false;
    }
    if (!ReturnElevatorToTop()) {
        return false;
    }
    return true;
}

bool DoubleActuator(const Payload *value) {
    if (!ReturnElevatorToTop()) {
        return false;
    }
    if (!DownToLine(value->Byte_4, kLimitTopLIne)) {
        return false;
    }
    __delay_ms(100);
    if (!ActuateAt(false, value->Byte_4, value->Byte_3, value->Byte_2)) {
        return false;
    }
    __delay_ms(200);
    if (!DownToLine(kLimitBottomLine, value->Byte_4)) {
        return false;
    }
    __delay_ms(300);
    if (!OpenDispenser()) {
        return false;
    }
    if (!ReturnElevatorToTop()) {
        return false;
    }
    return true;
}

bool ReturnElevatorToTop(void) {
    uint16_t timeout = 10000;
    uint16_t time = 0;
    SetElevatorOn(kUp, true);

    while (!IsElevatorSensorActive(kEndLimit) && (++time<timeout)) {
        __delay_ms(1);
    }

    SetElevatorOn(kUp, false);
    return (time < timeout);
}

bool DownToLine(const uint8_t line, uint8_t current_line) {
    if (line == current_line) {
        return true;
    }

    SetElevatorOn(kDown, true);
    uint16_t timeout = 10000;
    uint16_t time = 0;
    uint16_t timeout_internal = 1000;
    uint16_t time_internal = 0;
    while (current_line > line && (++time<timeout)) {
        __delay_ms(1);
        if (IsElevatorSensorActive(kLevel)) {
            --current_line;
            if (current_line > line) {
                while (IsElevatorSensorActive(kLevel) && (++time_internal<timeout_internal)) {
                    __delay_ms(1);
                }
                time_internal = 0;
            }
            if (time_internal > timeout_internal) {
                break;
            }
        }
        if (time > timeout) {
            break;
        }
    }
    SetElevatorOn(kDown, false);
    return (time < timeout) || (time_internal > timeout_internal);
}

bool ActuateAt(bool single, const uint8_t line, const uint8_t row_one, const uint8_t row_two) {
    SetLedOn(true);
    SetOnSensor(row_one, true);
    if (!single) {
        SetOnSensor(row_two, true);
    }

    SetLineMotorOn(line, true);
    SetColumnMotorOn(row_one, true);
    if (!single) {
        SetColumnMotorOn(row_two, true);
    }

    bool started = false;
    while (!started) {
        started = !IsSensorLineActive(line);
    }

    if (!single) {
        SetOnSensor(0xFF, false);
    }

    bool ready = false;
    bool ready_one = false;
    bool ready_two = false;
    uint16_t timeout = 10000;
    uint16_t time = 0;
    while (!ready && (++time<timeout)) {
        if (single) {
            if (IsSensorLineActive(row_one)) {
                SetColumnMotorOn(row_one, false);
                ready = true;
            }
            __delay_ms(1);
        } else {
            SetOnSensor(row_one, true);
            __delay_ms(1);
            if (IsSensorLineActive(line)) {
                SetColumnMotorOn(row_one, false);
                ready_one = true;
            }
            SetOnSensor(row_one, false);

            SetOnSensor(row_two, true);
            __delay_ms(1);
            if (IsSensorLineActive(line)) {
                SetColumnMotorOn(row_two, false);
                ready_two = true;
            }
            SetOnSensor(row_two, false);

            if (ready_one && ready_two) {
                ready = true;
            }
        }
    }

    SetOnSensor(0xFF, false);
    SetColumnMotorOn(0xFF, false);
    SetLineMotorOn(0xFF, false);
    SetLedOn(false);
    return (time < timeout);
}

bool OpenDispenser(void) {
    SetDispenserOn(true);
    uint16_t timeout = 10000;
    uint16_t time = 0;
    while (!IsDispenserOpen() && (++time<timeout)) {
        __delay_ms(1);
    }
    SetDispenserOn(false);
    close_dispenser_ = true;
    return (time < timeout);
}

bool CloseDispenser(void) {
    SetLedOn(true);
    uint16_t timeout = 5000;
    uint16_t time = 0;
    while (++time<timeout) {
        __delay_ms(1);
    }
    if (IsPresenceSensorActive()) {
        timeout = 7000;
        while (++time<timeout) {
            __delay_ms(1);
        }
    }
    SetDispenserOn(true);
    __delay_ms(100);
    SetDispenserOn(false);
    close_dispenser_ = false;
    SetLedOn(false);
    return true;
}