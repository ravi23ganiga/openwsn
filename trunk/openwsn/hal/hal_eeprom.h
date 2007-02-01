#ifndef _EEPROM_H_5783_
#define _EEPROM_H_5783_

#include "hal_foundation.h"

void eeprom_rawread( uint32 flashaddr, uint32 memaddr, uint32 size );
void eeprom_rawwrite( uint32 flashaddr, uint32 memaddr, uint32 size );

#endif