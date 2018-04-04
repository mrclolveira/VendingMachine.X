/*
 * File:   RGB.c
 * Author: mrclolveira
 *
 * Created on 12 de Março de 2018, 21:06
 */

#include "RGB.h"

void RGBInit(void) {
    TRISGbits.TRISG6 = 0;
    ANSGbits.ANSG6 = 0;
    TRISGbits.TRISG7 = 0;
    ANSGbits.ANSG7 = 0;
    TRISGbits.TRISG8 = 0;
    ANSGbits.ANSG8 = 0;

    OC1CON1 = 0x0000;
    OC1CON2 = 0x0000;
    OC1CON1bits.OCTSEL = 0b111;
    OC1CON1bits.TRIGMODE = 1;
    OC1CON1bits.OCM = 0b110;
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1R = 0x0FFF;
    OC1RS = 0x0;
    PR1 = 0x0FFF;

    OC2CON1 = 0x0000;
    OC2CON2 = 0x0000;
    OC2CON1bits.OCTSEL = 0b111;
    OC2CON1bits.TRIGMODE = 1;
    OC2CON1bits.OCM = 0b110;
    OC2CON2bits.SYNCSEL = 0x1F;
    OC2R = 0x0FFF;
    OC2RS = 0x0;
    PR2 = 0x0FFF;

    OC3CON1 = 0x0000;
    OC3CON2 = 0x0000;
    OC3CON1bits.OCTSEL = 0b111;
    OC3CON1bits.TRIGMODE = 1;
    OC3CON1bits.OCM = 0b110;
    OC3CON2bits.SYNCSEL = 0x1F;
    OC3R = 0x0FFF;
    OC3RS = 0x0;
    PR3 = 0x0FFF;

    __builtin_write_OSCCONL(OSCCON & 0xbf);
    RPOR10bits.RP21R = 1;
    RPOR13bits.RP26R = 2;
    RPOR9bits.RP19R = 36;
    __builtin_write_OSCCONL(OSCCON | 0x40);
}

void SetRGB(const uint8_t red, const uint8_t green, const uint8_t blue) {
    const double kProportion = 16.058823529411764705882352941176;
    uint16_t r = (double)red * kProportion;
    uint16_t g = (double)green * kProportion;
    uint16_t b = (double)blue * kProportion;

    OC1R = r;
    OC1RS = 0x0FFF - r;
    OC2R = g;
    OC2RS = 0x0FFF - g;
    OC3R = b;
    OC3RS = 0x0FFF - b;
}