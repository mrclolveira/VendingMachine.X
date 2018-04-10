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

uint8_t Handle(Protocoll *cmd) {
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

uint8_t HandleGetValues(Variable *var) {
    uint8_t known = false;
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

uint8_t HandleSetValues(const Variable *var) {
    uint8_t known = false;
    switch (var->address_) {
        case kAddressRGB:
            SetRGB(var->value_.Byte_1, var->value_.Byte_2, var->value_.Byte_3);
            known = true;
            break;
        case kAddressDoorLock:
            SendRun(var);
            UnlockDoor(&var->value_);
            known = true;
            break;
        case kAddressAlignActuators:
            SendRun(var);
            known = AlignActuators();
            break;
        case kAddressSingleActuator:
            SendRun(var);
            known = SingleActuator(&var->value_);;
            break;
        case kAddressDoubleActuator:
            SendRun(var);
            known = DoubleActuator(&var->value_);
            break;
        case kAddressSendPresenceStatus:
            send_presence_status_ = var->value_.Byte_4;
            known = true;
            break;
        case kAddressReturnElevator:
            SendRun(var);
            known = ReturnElevatorToTop(var->value_.Byte_4);
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
    Variable status;
    status.address_ = kAddressPresenceSensor;
    status.value_.Byte_1 = 0;
    status.value_.Byte_2 = 0;
    status.value_.Byte_3 = 0;
    status.value_.Byte_4 = (uint8_t) IsPresenceSensorActive();

    SendAck(&status);
}

void SendDispenserClosed() {
    Variable status;
    status.address_ = kAddressDispenserSensor;
    status.value_.Byte_1 = 0;
    status.value_.Byte_2 = 0;
    status.value_.Byte_3 = 0;
    status.value_.Byte_4 = (uint8_t) IsDispenserOpen();

    SendAck(&status);
}