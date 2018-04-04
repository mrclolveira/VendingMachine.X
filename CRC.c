/*
 * File:   CRC.c
 * Author: mrclo
 *
 * Created on 3 de Abril de 2018, 21:58
 */

#include "CRC.h"

unsigned int CRC_HW_calculate(const unsigned char *pData, unsigned long wLength) {
    char i;
    long wData;
    long wCrc = 0xffff;
    if (wLength == 0) {
        return wCrc;
    }

    do {
        for (i = 0, wData = ((unsigned int) 0xff & *pData++) << 8; i < 8; i++, wData <<= 1) {
            if ((wCrc & 0x8000) ^ (wData & 0x8000))
                wCrc = (wCrc << 1) ^ 0x1021;
            else wCrc <<= 1;
        }
    } while (--wLength);

    return wCrc & 0xffff;
}
