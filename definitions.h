
#ifndef DEFINITIONS_H
#define	DEFINITIONS_H

#include <xc.h>
#include <stdbool.h>

typedef union {
    struct {
        uint8_t Low;
        uint8_t High;
    };
    uint16_t integer;
} Integer;

typedef struct {
    uint8_t Byte_1;
    uint8_t Byte_2;
    uint8_t Byte_3;
    uint8_t Byte_4;
}Payload;

typedef struct {
    unsigned char preamble_;
    unsigned char property_;
} Header;

typedef struct {
    uint8_t address_;
    Payload value_;
} Variable;

#define protocoll_size_ 9
typedef union {
    struct {
        Header header_;
        Variable variable_;
        Integer crc_;
    };
    unsigned char bytes[protocoll_size_];
} Protocoll;


#define kFixedPreamble 0x83

#define kSetProperty 0x00
#define kGetProperty 0x01
#define kAckProperty 0x02
#define kFaultProperty 0xFF
#define kUnknownProperty 0xFD

#define kAddressPresenceSensor 0x10
#define kAddressDispenserSensor 0x11
#define kAddressRGB 0x20
#define kAddressDoorLock 0x21
#define kAddressAlignActuators 0x22
#define kAddressSingleActuator 0x23
#define kAddressDoubleActuator 0x24
#define kAddressSendPresenceStatus 0x25
#define kAddressReset 0x26

#endif	// DEFINITIONS_H

