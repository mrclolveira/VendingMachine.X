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
            var->value_.Integer = (uint64_t) IsPresenceSensorActive();
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
            break;
        default:
            known = false;
            break;
    }
    return known;
}

