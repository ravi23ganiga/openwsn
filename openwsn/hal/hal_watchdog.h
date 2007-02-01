
#ifndef _WATCHDOG_H_5628_
#define _WATCHDOG_H_5628_

/******************************************************************************
 * @author zhangwei on 2006-08-13
 * TWatchdog 
 * 
 * This module contains a TWatchdog object. This is an software abstraction of 
 * the hardware watchdog. 
 * 
 * @attention: There's usually only one watchdog hardware in the MCU. however, 
 * the software's construct procedure does not check this.
 *****************************************************************************/
 
#include "hal_foundation.h"

typedef struct{
  uint32 interval;
  uint8 state;
}TWatchdog;

/* construct a global TWatchdog object. You can reference this object by global
 * variable "g_watchdog". Note that the watchdog only start by calling enable()
 * function.
 *
 * This implementation only allows one watchdog object exists in the system.
 */
TWatchdog * watchdog_construct( char * buf, uint16 size, uint32 interval );
void watchdog_destroy( TWatchdog * wdg );
void watchdog_configure( TWatchdog * wdg, uint8 opt );
void watchdog_enable( TWatchdog * wdg );
void watchdog_disable( TWatchdog * wdg );
void watchdog_feed( TWatchdog * wdg );

#endif