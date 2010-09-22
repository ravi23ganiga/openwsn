#ifndef _HAL_FOUNDATION_H_3721_
#define _HAL_FOUNDATION_H_3721_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

/******************************************************************************
 * @author zhangwei on 20060906
 * foundation.h
 * this is the base foundation file of all the files in the application.
 *
 * @modified by zhangwei on 20060906
 * add "signed" in the typedef of int8
 * this is because some compile will assume char as unsigned type, while here
 * we need a signed char.
 *****************************************************************************/

#define CONFIG_INT2HANDLER_ENABLE
#define CONFIG_INT2HANDLER_CAPACITY 32
#define CONFIG_NANOS_ENABLE

/* @attention
 * in WinAVR(avr-gcc), <stdint.h> contains the basic types used in WinAVR such as:
 * int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t,
 * intptr_t, uintptr_t, size_t, wchar_t 
 * 
 * so if you don't use WinAVR, you can comment the line about including stdint.h.
 */

#include "hal_configall.h"
#include <stdint.h>
#include "../rtl/rtl_foundation.h"

/* System wide event identifier */

#define EVENT_RESET                     1
#define EVENT_RESTART                   2
#define EVENT_WAKEUP                    3
#define EVENT_SLEEP                     4
#define EVENT_TIMER_EXPIRED             5
#define EVENT_UART_DATA_ARRIVAL         7
#define EVENT_DATA_ARRIVAL              7
#define EVENT_DATA_COMPLETE             8
#define EVENT_REQUEST                   9
#define EVENT_REPLY                     10
#define EVENT_ACTION_DONE               11

/* for memory maped I/O only */
/*
#define inp(port)         (*((volatile byte *) (port))) 
#define inpw(port)        (*((volatile word *) (port))) 
#define inpdw(port)       (*((volatile dword *)(port))) 
#define outp(port, val)   (*((volatile byte *) (port)) = ((byte) (val))) 
#define outpw(port, val)  (*((volatile word *) (port)) = ((word) (val))) 
#define outpdw(port, val) (*((volatile dword *) (port)) = ((dword) (val))) 
*/

#ifdef __cplusplus
extern "C" {
#endif

/* @modified by openwsn on 2010.08.04
 * - merge TiHalEvent and TiEvent together. They're actually already the same.
 * - merge TiHalEventHandler and TiEventHandler together. They're actually already the same.
 */

#define TiHalEvent TiEvent

/*
struct _TiHalEvent{
  uintx id;
  void (* handler)(void * object, struct _TiHalEvent * e);
  void * objectfrom;
  void * objectto;
};
typedef struct _TiHalEvent TiHalEvent;
*/

#define TiHalEventId uintx

#define TiHalEventHandler TiFunEventHandler
//typedef void (* TiHalEventHandler)(void * object, TiHalEvent * e);
typedef void (* TiFunInterruptHandler)(void);


/* the following type is used to implement atomic mechanism in hal_cpu module.
 * the current settings is adapt to atmega128 8 bit MCU only. you may need to change 
 * the definition when porting to a new architecture. */

#define hal_atomic_t cpu_atomic_t
typedef uint8 cpu_atomic_t;



/******************************************************************************
 * global variable
 *****************************************************************************/

/* g_atomic_level: to keep the atmic nested level. defined in this module. */

extern uint8 g_atomic_level;

/* interrupt number to object's handler mapping table.
 * this table is used to save the relationship between interrupt number and object's
 * event handler. when the hardware interrupt occurs, the kernel can use this
 * table to map the hardware interrupt call to the object's handler call.
 *
 * 	{interrupt number, objects handler, object}
 */

typedef struct{
  uint8 num;
  TiFunEventHandler handler;
  void * owner;
}_TiIntHandlerItem;

/* "iht_" denotes "interrupt number - handler table" */

#ifdef CONFIG_INT2HANDLER_ENABLE
extern _TiIntHandlerItem m_int2handler[CONFIG_INT2HANDLER_CAPACITY];
#endif


/******************************************************************************
 * software initialization of the hal layer
 *****************************************************************************/

void hal_init( TiFunEventHandler listener, void * object );

/******************************************************************************
 * interaction with the upper layer such as the osx kernel or other listeners
 *
 * osx is a ultralight OS kernel running on top of the hal layer. the following 
 * types and functions are used to help interfacing the hal layer objects to 
 * the osx kernel. 
 * 
 * though the following is used to help using osx kernel, this module doesn't 
 * require to include osx modules when compiling. 
 * 
 * @attention
 *	- you must call hal_swinit() some where before you can call the following 
 * functions successfully. 
 *****************************************************************************/


void hal_setlistener( TiFunEventHandler listener, void * listener_owner );
void hal_notifylistener( TiEvent * e );
void hal_notify_ex( TiEventId eid, void * objectfrom, void * objectto );


/*
defined in hal_timer
#ifdef CONFIG_TARGET_GAINZ
  #define tm_value_t uintx
#else
  #define tm_value_t uint32
#endif
*/

/**
 * TiTimerInterface
 */
//todo


#ifdef __cplusplus
}
#endif

#endif /*_HAL_FOUNDATION_H_*/
