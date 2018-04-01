/*
 * File:   LED.c
 * Author: mrclolveira
 *
 * Created on 11 de Março de 2018, 23:05
 */


#include "LED.h"

#define led(x) LATFbits.LATF5 = x

void LEDInit(void) {
    TRISFbits.TRISF5 = 0;
}

void SetLedOn(bool on) {
    led(on);
}