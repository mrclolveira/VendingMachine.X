/*
 * File:   RGB.c
 * Author: mrclolveira
 *
 * Created on 12 de Março de 2018, 21:06
 */

#include "RGB.h"
#include "Uart.h"

void RGBInit(void) {
    TRISGbits.TRISG6 = 0;
    ANSGbits.ANSG6 = 1;
    TRISGbits.TRISG7 = 0;
    ANSGbits.ANSG7 = 1;
    TRISGbits.TRISG8 = 0;
    ANSGbits.ANSG8 = 1;

    __builtin_write_OSCCONL(OSCCON & 0xbf);
    RPOR10bits.RP21R = 18;
    RPOR13bits.RP26R = 21;
    RPOR9bits.RP19R = 24;
    __builtin_write_OSCCONL(OSCCON | 0x40);

    OC1CON1 = 0x0000;
    OC1CON2 = 0x0000;
    OC1CON2bits.SYNCSEL = 0b00001;
    OC1CON2bits.OCTRIG = 0;
    OC1CON1bits.OCTSEL = 0b111;
    OC1R = 0x07FF;
    OC1RS = 0x0FFF;
    OC1CON1bits.OCM = 0b110;

    OC2CON1 = 0x0000;
    OC2CON2 = 0x0000;
    OC2CON2bits.SYNCSEL = 0b00100;
    OC2CON2bits.OCTRIG = 0;
    OC2CON1bits.OCTSEL = 0b111;
    OC2R = 0x07FF;
    OC2RS = 0x0FFF;
    OC2CON1bits.OCM = 0b110;

    OC3CON1 = 0x0000;
    OC3CON2 = 0x0000;
    OC3CON2bits.SYNCSEL = 0b00111;
    OC3CON2bits.OCTRIG = 0;
    OC3CON1bits.OCTSEL = 0b111;
    OC3R = 0x07FF;
    OC3RS = 0x0FFF;
    OC3CON1bits.OCM = 0b110;
}

void SetRGB(const uint8_t red, const uint8_t green, const uint8_t blue) {
    const double kProportion = 16.0588235;
    uint16_t r = (double)red * kProportion;
    uint16_t g = (double)green * kProportion;
    uint16_t b = (double)blue * kProportion;

    OC1R = 0x0FFF - r;

    OC2R = 0x0FFF - g;

    OC3R = 0x0FFF - b;
}