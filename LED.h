
#ifndef LED_H
#define	LED_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdbool.h>

void LEDInit(void);
void SetLedOn(uint8_t on);

#endif	// LED_H

