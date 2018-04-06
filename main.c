/*
 * File:   main.c
 * Author: mrclolveira
 *
 * Created on 11 de Março de 2018, 13:47
 */

#include <p24FJ256GB210.h>

// PIC24FJ256GB210 Configuration Bit Settings

// 'C' source line config statements


// CONFIG3
#pragma config WPFP = WPFP255           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config SOSCSEL = LPSOSC         // Secondary Oscillator Power Mode Select (Secondary oscillator is in Low-Power (low drive strength) Oscillator mode)
#pragma config WUTSEL = LEG             // Voltage Regulator Wake-up Time Select (Default regulator start-up time is used)
#pragma config ALTPMP = ALPMPDIS        // Alternate PMP Pin Mapping (EPMP pins are in default location mode)
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable (Segmented code protection is disabled)
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select (Last page (at the top of program memory) and Flash Configuration Words are not write-protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select (Protected code segment upper boundary is at the last page of program memory; the lower boundary is the code page specified by WPFP)

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS Oscillator mode is selected)
#pragma config IOL1WAY = ON             // IOLOCK One-Way Set Enable (The IOLOCK bit (OSCCON<6>) can be set once, provided the unlock sequence has been completed. Once set, the Peripheral Pin Select registers cannot be written to a second time.)
#pragma config OSCIOFNC = OFF           // OSCO Pin Configuration (OSCO/CLKO/RC15 functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Fail-Safe Clock Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = PRI              // Initial Oscillator Select (Primary Oscillator (XT, HS, EC))
#pragma config PLL96MHZ = OFF           // 96MHz PLL Startup Select (96 MHz PLL is software controlled (can be enabled by setting the PLLEN bit in CLKDIV<5>))
#pragma config PLLDIV = DIV12           // 96 MHz PLL Prescaler Select (Oscillator input is divided by 12 (48 MHz input))
#pragma config IESO = ON                // Internal External Switchover (IESO mode (Two-Speed Start-up) is enabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config ALTVREF = ALTVREDIS      // Alternate VREF location Enable (VREF is on a default pin (VREF+ on RA9 and VREF- on RA10))
#pragma config WINDIS = OFF             // Windowed WDT (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer (Watchdog Timer is disabled)
#pragma config ICS = PGx1               // Emulator Pin Placement Select bits (Emulator functions are shared with PGEC1/PGED1)
#pragma config GWRP = OFF               // General Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

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

bool close_dispenser_ = false;
bool send_presence_status_ = false;

void Setup() {
    // Wait for hardware get stable
    __delay_ms(2000);

    UartInit();
    UartWriteInt(RCON);
    RCON = 0;
    SensorsInit();
    ActuatorsInit();
    LEDInit();
    RGBInit();

    ReturnElevatorToTop();
}

int main(void) {
    Setup();

    while(true) {
        if (close_dispenser_) {
            CloseDispenser();
            SendDispenserClosed();
        }

        if (send_presence_status_) {
            SendPresenceStatus();
        }

        SetLedOn(!LATFbits.LATF5);
        __delay_ms(500);
    }
    return 0;
}
