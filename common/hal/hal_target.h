#ifndef _HAL_TARGET_H_7483_
#define _HAL_TARGET_H_7483_

/* Hardware Platform Layer
 * including CPU/MCU specific source codes
 * 
 * Reference 
 * Hardware Abstraction Architecture, http://www.tinyos.net/tinyos-2.x/doc/html/tep2.html
 */ 
 
//#include "hal_configall.h"
#include "hpl_atmega128.h" 
#include "hpl_gainz.h"

void target_init( void );
void target_reset( void );

#endif




