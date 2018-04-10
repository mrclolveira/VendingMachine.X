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

uint8_t AlignActuators() {
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

uint8_t SingleActuator(const Payload *value) {
    ReturnElevatorToTop(value->Byte_1);
    DownToLine(value->Byte_4, kLimitTopLIne, value->Byte_1);
    __delay_ms(100);
    ActuateAt(true, value->Byte_4, value->Byte_3, 0xFF);
    __delay_ms(200);
    DownToLine(kLimitBottomLine, value->Byte_4, value->Byte_1);
    __delay_ms(300);
    OpenDispenser(value->Byte_1);
    ReturnElevatorToTop(value->Byte_1);
    return true;
}

uint8_t DoubleActuator(const Payload *value) {
    ReturnElevatorToTop(value->Byte_1);
    DownToLine(value->Byte_4, kLimitTopLIne, value->Byte_1);
    __delay_ms(100);
    ActuateAt(false, value->Byte_4, value->Byte_3, value->Byte_2);
    __delay_ms(200);
    DownToLine(kLimitBottomLine, value->Byte_4, value->Byte_1);
    __delay_ms(300);
    OpenDispenser(value->Byte_1);
    ReturnElevatorToTop(value->Byte_1);
    return true;
}

uint8_t ReturnElevatorToTop(uint8_t rele) {
    uint16_t timeout = 10000;
    uint16_t time = 0;
    SetElevatorOn(kUp, rele);

    while (!IsElevatorSensorActive(kEndLimit) && (++time<timeout)) {
        __delay_ms(1);
    }

    SetElevatorOn(kStoped, rele);
    return true;
}

uint8_t DownToLine(const uint8_t line, uint8_t current_line, uint8_t rele) {
    if (line == current_line) {
        return true;
    }

    SetElevatorOn(kDown, rele);
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
    SetElevatorOn(kStoped, rele);
    return true;
}

uint8_t ActuateAt(uint8_t single, const uint8_t line, const uint8_t row_one, const uint8_t row_two) {
    SetLedOn(true);
    SetOnSensor(row_one, true);
    if (single == false) {
        SetOnSensor(row_two, true);
    }

    SetLineMotorOn(line, true);
    SetColumnMotorOn(row_one, true);
    if (single == false) {
        SetColumnMotorOn(row_two, true);
    }

    uint8_t started = false;
    while (!started) {
        started = IsSensorLineActive(line);
    }

    if (single == false) {
        SetOnSensor(0xFF, false);
    }

    uint8_t ready = false;
    uint8_t ready_one = false;
    uint8_t ready_two = false;
    uint16_t timeout = 10000;
    uint16_t time = 0;
    while (!ready && (++time<timeout)) {
        if (single == true) {
            if (IsSensorLineActive(line)) {
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
    return true;
}

uint8_t OpenDispenser(uint8_t rele) {
    uint16_t timeout = 10000;
    uint16_t time = 0;

    uint8_t current_line = 0;
    uint8_t line = 1;

    SetElevatorOn(kUp, rele);
    while (current_line < line && (++time<timeout)) {
        __delay_ms(1);
        if (IsElevatorSensorActive(kLevel)) {
            ++current_line;
        }
        if (time > timeout) {
            break;
        }
    }
    SetElevatorOn(kStoped, rele);

    timeout = 0;
    SetDispenserOn(true);
    while (!IsDispenserOpen() && (++time<timeout)) {
        __delay_ms(1);
    }
    SetDispenserOn(false);
    close_dispenser_ = true;
    return true;
}

uint8_t CloseDispenser(void) {
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