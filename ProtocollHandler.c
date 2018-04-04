/*
 * File:   ProtocollHandler.c
 * Author: mrclolveira
 *
 * Created on 11 de Março de 2018, 16:58
 */

#include <xc.h>
#include <libpic30.h>

#include "ProtocollHandler.h"
#include "Sensors.h"
#include "RGB.h"
#include "Task.h"
#include "Uart.h"
#include "CRC.h"

bool Handle(Protocoll *cmd) {
    if (cmd->header_.preamble_ != kFixedPreamble) {
        return false;
    }

    if (cmd->header_.property_ == kGetProperty) {
        return HandleGetValues(&cmd->variable_);
    } else if (cmd->header_.property_ == kSetProperty) {
        return HandleSetValues(&cmd->variable_);
    }

    return false;
}

bool HandleGetValues(Variable *var) {
    bool known = false;
    switch (var->address_) {
        case kAddressPresenceSensor:
            var->value_.Byte_4 = (uint8_t) IsPresenceSensorActive();
            known = true;
            break;
        case kAddressDispenserSensor:
            var->value_.Byte_4 = (uint8_t) IsDispenserOpen();
            known = true;
            break;
        default:
            known = false;
            break;
    }
    return known;
}

bool HandleSetValues(const Variable *var) {
    bool known = false;
    switch (var->address_) {
        case kAddressRGB:
            SetRGB(var->value_.Byte_1, var->value_.Byte_2, var->value_.Byte_3);
            known = true;
            break;
        case kAddressDoorLock:
            UnlockDoor(&var->value_);
            known = true;
            break;
        case kAddressAlignActuators:
            AlignActuators();
            known = true;
            break;
        case kAddressSingleActuator:
            SingleActuator(&var->value_);
            known = true;
            break;
        case kAddressDoubleActuator:
            DoubleActuator(&var->value_);
            known = true;
            break;
        case kAddressReset:
            __asm__ volatile ("reset");
            known = true;
            break;
        default:
            known = false;
            break;
    }
    return known;
}

void SendPresenceStatus() {
    Protocoll status;
    status.header_.preamble_ = kFixedPreamble;
    status.header_.property_ = kAckProperty;
    status.variable_.address_ = kAddressPresenceSensor;
    status.variable_.value_.Byte_4 = (uint8_t) IsPresenceSensorActive();

    Integer crc;
    crc.integer = CRC_HW_calculate(status.bytes, protocoll_size_ - 2);
    status.crc_ = crc;
    UartWrite(&status);
}

void SendDispenserClosed() {
    Protocoll status;
    status.header_.preamble_ = kFixedPreamble;
    status.header_.property_ = kAckProperty;
    status.variable_.address_ = kAddressDispenserSensor;
    status.variable_.value_.Byte_4 = (uint8_t) IsDispenserOpen();

    Integer crc;
    crc.integer = CRC_HW_calculate(status.bytes, protocoll_size_ - 2);
    status.crc_ = crc;
    UartWrite(&status);
}