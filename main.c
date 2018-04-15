/*
 * File:   main.c
 * Author: mrclolveira
 *
 * Created on 11 de Mar�o de 2018, 13:47
 */

#include <p24FJ256GB210.h>

// PIC24FJ256GB210 Configuration Bit Settings

// 'C' source line config statements

// CONFIG3
#pragma config WPFP = WPFP255
#pragma config SOSCSEL = LPSOSC
#pragma config WUTSEL = LEG
#pragma config ALTPMP = ALPMPDIS
#pragma config WPDIS = WPDIS
#pragma config WPCFG = WPCFGDIS
#pragma config WPEND = WPENDMEM

// CONFIG2
#pragma config POSCMOD = HS
#pragma config IOL1WAY = OFF
#pragma config OSCIOFNC = OFF
#pragma config FCKSM = CSECME
#pragma config FNOSC = PRIPLL
#pragma config PLL96MHZ = ON
#pragma config PLLDIV = DIV4
#pragma config IESO = ON

// CONFIG1
#pragma config WDTPS = PS32768
#pragma config FWPSA = PR128
#pragma config ALTVREF = ALTVREDIS
#pragma config WINDIS = OFF
#pragma config FWDTEN = OFF
#pragma config ICS = PGx1
#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF

#include "System.h"

#include <xc.h>
#include <stdbool.h>
#include <libpic30.h>


#include "Uart.h"
#include "ProtocollHandler.h"
#include "Sensors.h"
#include "definitions.h"
#include "Actuators.h"
#include "LED.h"
#include "RGB.h"
#include "Task.h"

uint8_t close_dispenser_ = false;
uint8_t send_presence_status_ = false;

void Setup() {
    ANSA = 0;
    ANSB = 0;
    ANSC = 0;
    ANSD = 0;
    ANSE = 0;
    ANSF = 0;
    ANSG = 0;

    RGBInit();
    SensorsInit();
    ActuatorsInit();
    LEDInit();
    UartInit();
    RCON = 0;

    ReturnElevatorToTop(true);
}

int main(void) {
    Setup();

    while(1) {
        if (close_dispenser_) {
            SendDispenserClosed();
            CloseDispenser();
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
    return 0;
}
