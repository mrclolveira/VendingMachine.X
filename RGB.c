/*
 * File:   RGB.c
 * Author: mrclolveira
 *
 * Created on 12 de Março de 2018, 21:06
 */

#include "RGB.h"
#include "Uart.h"

void RGBInit(void) {
    __builtin_write_OSCCONL(OSCCON & 0xbf);
    RPOR10bits.RP21R = 18;
    RPOR13bits.RP26R = 19;
    RPOR9bits.RP19R = 20;
    __builtin_write_OSCCONL(OSCCON | 0x40);

    OC1R = 0x00;
    OC1RS = 0xFF;
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON2bits.OCTRIG = 0;
    OC1CON2bits.OCINV = 1;
    OC1CON1bits.OCTSEL = 0b111;
    OC1CON1bits.OCM = 0b110;

    OC2R = 0x00;
    OC2RS = 0xFF;
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2CON2bits.OCTRIG = 0;
    OC2CON2bits.OCINV = 1;
    OC2CON1bits.OCTSEL = 0b111;
    OC2CON1bits.OCM = 0b110;

    OC3R = 0x00;
    OC3RS = 0xFF;
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3CON2bits.OCTRIG = 0;
    OC3CON2bits.OCINV = 1;
    OC3CON1bits.OCTSEL = 0b111;
    OC3CON1bits.OCM = 0b110;
}

void SetRGB(const uint8_t red, const uint8_t green, const uint8_t blue) {
    const double kProportion = 16.0588235;
    uint16_t r = (double)red * kProportion;
    uint16_t g = (double)green * kProportion;
    uint16_t b = (double)blue * kProportion;

    OC1R = 0xFF - r;
    OC2R = 0xFF - g;
    OC3R = 0xFF - b;
}