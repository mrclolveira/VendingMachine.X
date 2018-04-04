
#ifndef CRC_H
#define	CRC_H

#include <xc.h> // include processor files - each processor file is guarded.

unsigned int CRC_HW_calculate(const unsigned char *pData, unsigned long wLength);

#endif	// CRC_H

