
#ifndef PROTOCOLLHANDLER_H
#define	PROTOCOLLHANDLER_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdbool.h>
#include "definitions.h"

bool Handle(Protocoll *cmd);
bool HandleGetValues(Variable *var);
bool HandleSetValues(const Variable *var);
void SendPresenceStatus();
void SendDispenserClosed();

extern bool send_presence_status_;

#endif	// PROTOCOLLHANDLER_H