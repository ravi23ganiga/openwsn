/******************************************************************************
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
 *****************************************************************************/

#ifndef _HAL_FOUNDATION_H_5342_
#define _HAL_FOUNDATION_H_5342_

/*******************************************************************************
 * @author zhangwei on 2006-07-24
 * 	this is the foundation header of HAL. every file in "hal" directory should
 * include this one or "hal_configall.h" as their first header file in order
 * to make the definition and declarations in this file effective.
 *
 * @history
 * @modified by zhangwei on 2006-07-24
 * 		this file replace the old "include.h".
 * @modified by zhangwei on 2006-09-01
 * 		add type TInterruptHandler and TEventHandler. these two are function
 * 		pointer types. they are often used when setting callback functions.
 ******************************************************************************/

#include "hal_configall.h"
#include "./arch/arch_configall.h"
#include "./arch/arch_foundation.h"
#include <string.h>

#define min(x,y) ((x<y)?(x):(y))
#define max(x,y) ((x>y)?(x):(y))


// Useful stuff
#define BM(n) (1 << (n))
#define BF(x,b,s) (((x) & (b)) >> (s))
#define MINI(n,m) (((n) < (m)) ? (n) : (m))
#define MAX(n,m) (((n) < (m)) ? (m) : (n))
#define ABS(n) ((n < 0) ? -(n) : (n))

// Dynamic function pointer
typedef void (*VFPTR)(void);

/* TFunInterrupt
 * represent the hardware's interrupt service routine (ISR)
 *
 * TFunCallback
 * a standard callback function pointer type
 */
typedef void (* TInterruptHandler)( void );
typedef int8 (* TEventHandler)( void * data );

#include <string.h>

/******************************************************************************
 * Delay Functions:
 *
 * hal_pause()
 * not recommended to use directly.
 * make the program pause for a short time. we don't recomment you use this function
 * directly because the pause period is determined by CPU and hardware. it's different
 * on different hardware platform. for current implmentation on ARM7 processor,
 * it's about 2 cycle.
 *
 * hal_delay( delay )
 * hal_delay16( uint16 delay )
 * hal_delay32( uint32 delay )
 * recommend to use. they two function will cause the CPU to wait for a specified
 * period. the "delay" parameter is based on millseconds. for example, if you
 * want to delay 10 millisecond, you can simply call:
 *     hal_delay( 10 );
 *
 * for long delay and precise delay, we recommend you to use Timer rather than
 * hal_delay().
 *****************************************************************************/
#ifdef __GNUC__
/* usually the following instruction uses exactly 2 clock cycle @TODO sure??? */
#define hal_pause() NOP();

#elif defined __IAR_SYSTEMS_ICC__
#pragma optimize=none
#define hal_pause() NOP();

/* for un-specified compiles */
#else
#define hal_pause() _hal_pause()
#endif

#define hal_delay(dly) hal_delay16(dly)
void hal_delay16( uint16 dly );
void hal_delay32( uint32 dly );

/******************************************************************************
 * CPU Related Functions
 *****************************************************************************/

void hal_reboot( void );
void hal_sleep( void );
void hal_sleepwait( uint32 delay );
void hal_wakeup( void );
void hal_powerdown( void );

/******************************************************************************
 * Interrupt Related Functions
 *****************************************************************************/

#endif
