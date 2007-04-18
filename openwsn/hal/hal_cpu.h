/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#ifndef _CPU_H_1675_ 
#define _CPU_H_1675_ 

#include "hal_foundation.h"

#ifdef HAL_USE_UCOSII
#include "..\arm\os_cpu.h"
#endif

/* @author zhangwei on 2006-07-20
 * you SHOULD always perfer to use enter_critical() instead of enable_interrupts()
 * and better leave_critical() than disable_interrupts(). the reason is that xxx_critical()
 * will leave the global interrupt flag unchanged, while the disable() enable() pair will
 * leave the flag enabled!!! this is sometimes unwanted!
 */
#ifdef HAL_USE_UCOSII
  #define hal_enable_interrupts() IRQEnable()
  #define hal_disable_interrupts() IRQDisable()
  #define hal_enter_critical() OS_ENTER_CRITICAL()
  #define hal_leave_critical() OS_EXIT_CRITICAL()
#else
  /* TODO hal_enter_critical() and hal_leave_critical() are essentially wrong!
   */
  #define hal_enter_critical() NO_OS_ENTER_CRITICAL()
  #define hal_leave_critical() NO_OS_EXIT_CRITICAL
  #define hal_enable_interrupts() IRQEnable()
  #define hal_disable_interrupts() IRQDisable()
#endif  



void NO_OS_ENTER_CRITICAL(void);
void NO_OS_EXIT_CRITICAL(void);


/* cpu sleep
 * the cpu will be wakeup by external hardware interrupts such as 
 * - UART interrupts
 * - cc2420 FIFOP interrupts
 */
#define hal_cpu_sleep() NULL
#define hal_cpu_wakeup() NULL
#define hal_sleep() hal_cpu_sleep()
#define hal_wakeup() hal_cpu_wakeup()

void hal_delay( uint16 delay );
void hal_reboot( void );

// @TODO
//void hal_attach_interrupt()
//void hal_detach_interrupt();


// obsolete functions
// should be replaced by hal_delay()
void halWait(uint32  dly);

#endif
