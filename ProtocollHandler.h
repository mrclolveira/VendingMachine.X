
#ifndef PROTOCOLLHANDLER_H
#define	PROTOCOLLHANDLER_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdbool.h>
#include "definitions.h"

uint8_t Handle(Protocoll *cmd);
uint8_t HandleGetValues(Variable *var);
uint8_t HandleSetValues(const Variable *var);
void SendPresenceStatus();
void SendDispenserClosed();

extern uint8_t send_presence_status_;

#endif	// PROTOCOLLHANDLER_H