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
#include "ProtocollHandler.h"
#include "Uart.h"

uint8_t close_dispenser_ = false;
uint8_t send_presence_status_ = false;
uint8_t time_to_close_dispenser_ = kDefaultTimeToClose;

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

void MainTask(void) {
    if (close_dispenser_) {
        SendDispenserClosed();
        CloseDispenser(time_to_close_dispenser_);
    }

    if (send_presence_status_) {
        SendPresenceStatus();
    }

    if (IsButtonUpActive()) {
        ReturnElevatorToTop(true);
    }

    if (IsButtonDownActive()) {
        GoElevatorToDown(true);
    }

    SetLedOn(!LATFbits.LATF5);
    __delay_ms(500);
}

uint8_t SetSendPresenceStatus(const Payload *value) {
    if (value->Byte_4 == false || value->Byte_4 == true) {
        send_presence_status_ = value->Byte_4;
        return true;
    }
    return false;
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
    for (line = 0; line < kNumberOfLines; line++) {
        for (column = 0; column < kNumberOfColumns; column++) {
            if (!ActuateSingleAt(line, column)) {
                return false;
            }
        }
    }
    return true;
}

uint8_t SingleActuator(const Payload *value) {
    if (value->Byte_4 >= kNumberOfLines || value->Byte_3 >= kNumberOfColumns) {
        return false;
    }

    if (ReturnElevatorToTop(true) == false) {
        return false;
    }
    if (DownToLine(value->Byte_4, kLastLineOnTop, true) == false) {
        return false;
    }
    __delay_ms(500);
    if (ActuateSingleAt(value->Byte_4, value->Byte_3) == false) {
        return false;
    }
    __delay_ms(2000);
    if (GoElevatorToDown(true) == false) {
        return false;
    }
    __delay_ms(2000);
    if (OpenDispenser(true, false) == false) {
        return false;
    }
    if (value->Byte_4 > 0) {
        time_to_close_dispenser_ = value->Byte_4;
    } else {
        time_to_close_dispenser_ = kDefaultTimeToClose;
    }
    return true;
}

uint8_t DoubleActuator(const Payload *value) {
    if (value->Byte_4 >= kNumberOfLines || value->Byte_3 >= kNumberOfColumns || value->Byte_2 >= kNumberOfColumns) {
        return false;
    }

    if (ReturnElevatorToTop(true) == false) {
        return false;
    }
    if (DownToLine(value->Byte_4, kLastLineOnTop, true) == false) {
        return false;
    }
    __delay_ms(500);
    if (ActuateDoubleAt(value->Byte_4, value->Byte_3, value->Byte_2) == false) {
        return false;
    }
    __delay_ms(2000);
    if (GoElevatorToDown(true) == false) {
        return false;
    }
    __delay_ms(2000);
    if (OpenDispenser(true, false) == false) {
        return false;
    }
    if (value->Byte_4 > 0) {
        time_to_close_dispenser_ = value->Byte_4;
    } else {
        time_to_close_dispenser_ = kDefaultTimeToClose;
    }
    return true;
}

uint8_t ReturnElevatorToTop(uint8_t rele) {
    if (IsElevatorSensorActive(kEndLimit)) {
        return true;
    }

    uint16_t timeout = 30000;
    uint16_t time = 0;
    SetElevatorOn(kUp, rele);

    while (!IsElevatorSensorActive(kEndLimit) && (++time<timeout)) {
        __delay_ms(1);
    }

    SetElevatorOn(kStoped, rele);
    if (time >= timeout) {
        return false;
    }
    return true;
}

uint8_t GoElevatorToDown(uint8_t rele) {
    if (IsElevatorSensorActive(kEndLimitDown)) {
        return true;
    }

    uint16_t timeout = 30000;
    uint16_t time = 0;
    SetElevatorOn(kDown, rele);

    while (!IsElevatorSensorActive(kEndLimitDown) && (++time<timeout)) {
        __delay_ms(1);
    }

    SetElevatorOn(kStoped, rele);
    if (time >= timeout) {
        return false;
    }
    return true;
}

uint8_t DownToLine(const uint8_t line, uint8_t current_line, uint8_t rele) {
    if (line == current_line) {
        return true;
    }

    uint16_t timeout = 30000;
    uint16_t time = 0;
    uint16_t timeout_internal = 1500;
    uint16_t time_internal = 0;

    SetElevatorOn(kDown, rele);
    while (IsElevatorSensorActive(kLevel) && (++time_internal<timeout_internal)) {
        __delay_ms(1);
    }

    time_internal = 0;
    timeout_internal = 3;
    while (current_line > line && (++time<timeout)) {
        __delay_ms(1);
        if (IsElevatorSensorActive(kLevel)) {
            --current_line;
            if (current_line > line) {
                while (IsElevatorSensorActive(kLevel) && (++time_internal<timeout_internal)) {
                    __delay_ms(1000);
                    if (IsElevatorSensorActive(kEndLimitDown)) {
                        break;
                    }
                }
            }
            if (time_internal > timeout_internal) {
                break;
            }
            time_internal = 0;
        }
        if (IsElevatorSensorActive(kEndLimitDown)) {
            break;
        }
        if (time > timeout) {
            break;
        }
    }
    SetElevatorOn(kStoped, rele);
    if (time >= timeout || time_internal >= timeout_internal) {
        return false;
    }
    return true;
}

uint8_t ActuateSingleAt(const uint8_t line, const uint8_t row) {
    SetLedOn(true);
    SetOnSensor(row, true);

    uint16_t timeout = 3000;
    uint16_t time = 0;

    SetLineMotorOn(line, true);
    SetColumnMotorOn(row, true);

    __delay_ms(450);
    while (IsSensorLineActive(line) == true) {
        __delay_ms(1);
    }

    while (IsSensorLineActive(line) == false && (++time<timeout)) {
        __delay_ms(1);
    }

    SetColumnMotorOn(0xFF, false);
    SetLineMotorOn(0xFF, false);
    SetOnSensor(0xFF, false);
    SetLedOn(false);
    if (time >= timeout) {
        return false;
    }
    return true;
}

uint8_t ActuateDoubleAt(const uint8_t line, const uint8_t row_one, const uint8_t row_two) {
    SetLedOn(true);
    SetOnSensor(row_one, true);
    SetOnSensor(row_two, true);

    uint8_t ready = false;
    uint8_t ready_one = false;
    uint8_t ready_two = false;
    uint16_t timeout = 3000;
    uint16_t time = 0;

    SetLineMotorOn(line, true);
    SetColumnMotorOn(row_one, true);
    SetColumnMotorOn(row_two, true);

    __delay_ms(450);
    while (IsSensorLineActive(line) == true) {
        __delay_ms(1);
    }

    SetOnSensor(0xFF, false);

    while (!ready && (++time < timeout)) {
        SetOnSensor(row_one, true);
        __delay_us(100);
        if (IsSensorLineActive(line) == true) {
            SetColumnMotorOn(row_one, false);
            ready_one = true;
        }
        SetOnSensor(row_one, false);

        SetOnSensor(row_two, true);
        __delay_us(100);
        if (IsSensorLineActive(line) == true) {
            SetColumnMotorOn(row_two, false);
            ready_two = true;
        }
        SetOnSensor(row_two, false);

        if (ready_one && ready_two) {
            ready = true;
        }
    }

    SetOnSensor(0xFF, false);
    SetColumnMotorOn(0xFF, false);
    SetLineMotorOn(0xFF, false);
    SetLedOn(false);
    if (time >= timeout) {
        return false;
    }
    return true;
}

uint8_t OpenDispenser(uint8_t rele, uint8_t turn_off_elevator) {
    uint16_t timeout = 10000;
    uint16_t time = 0;
    uint16_t timeout_internal = 1000;
    uint16_t time_internal = 0;

    uint8_t current_line = 0;
    uint8_t line = 1;

    SetElevatorOn(kUp, rele);
    while (IsElevatorSensorActive(kLevel) && (++time_internal<timeout_internal)) {
        __delay_ms(1);
    }

    time_internal = 0;
    while (current_line < line && (++time<timeout)) {
        __delay_ms(1);
        if (IsElevatorSensorActive(kLevel)) {
            ++current_line;
            while (IsElevatorSensorActive(kLevel) && (++time_internal<timeout_internal)) {
                if (IsElevatorSensorActive(kEndLimit)) {
                    break;
                }
                __delay_ms(1);
            }
            if (time_internal > timeout_internal) {
                break;
            }
            time_internal = 0;
        }
        if (IsElevatorSensorActive(kEndLimit)) {
            break;
        }
        if (time > timeout) {
            break;
        }
    }
    if (turn_off_elevator) {
        SetElevatorOn(kStoped, rele);
    }

    time = 0;
    SetDispenserOn(true);
    while (IsDispenserOpen() == false && (++time<timeout)) {
        __delay_ms(1);
    }

    SetDispenserOn(false);
    close_dispenser_ = true;
    if (time >= timeout || time_internal >= timeout_internal) {
        return false;
    }
    return true;
}

uint8_t CloseDispenser(uint8_t time_to_close) {
    SetLedOn(true);
    uint16_t timeout = (time_to_close * 1000);
    uint16_t time = 0;

    uint8_t is_elevator_on_top = false;
    SetElevatorOn(kUp, true);

    while (++time<timeout) {
        __delay_ms(1);
        if (IsElevatorSensorActive(kEndLimit) && is_elevator_on_top == false) {
            SetElevatorOn(kStoped, true);
            is_elevator_on_top = true;
        }
    }

    if (IsPresenceSensorActive()) {
        timeout += 2000;
        while (++time < timeout) {
            __delay_ms(1);
            if (IsElevatorSensorActive(kEndLimit) && is_elevator_on_top == false) {
                SetElevatorOn(kStoped, true);
                is_elevator_on_top = true;
            }
        }
    }

    SetDispenserOn(true);
    __delay_ms(500);
    SetDispenserOn(false);

    if (is_elevator_on_top == false) {
        time = 0;
        timeout = 25000;
        while (!IsElevatorSensorActive(kEndLimit) && (++time<timeout)) {
            __delay_ms(1);
        }
        SetElevatorOn(kStoped, true);
    }

    close_dispenser_ = false;
    SetLedOn(false);
    return true;
}
