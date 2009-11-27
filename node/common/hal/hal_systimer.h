#ifndef _HAL_SYSTIMER_H_3214_
#define _HAL_SYSTIMER_H_3214_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is a free software; you can redistribute it and/or modify it under
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
 ******************************************************************************/ 

#include "hal_configall.h"
#include "hal_foundation.h"

/*******************************************************************************
 * hal_systimer
 * TiSysTimer is used by and should used by the osx kernel only. It drives the osx 
 * kernel to run. Besides the TiSysTimer, the osx kernel can also be drived by
 * a infinite loop.
 *
 * @author zhangwei on 200610
 * @modified by zhangwei on 20090520
 *	- revision
 ******************************************************************************/

/*
#ifdef CONFIG_TARGET_GAINZ
  #define tm_value_t uintx
#else
  #define tm_value_t uintx
#endif
*/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Q: what is TiSysTimer used for?
 * R: support Embedded OS or hardware drived schedulers
 * 
 * Q: why not using a TiTimerAdapter instead of a new TiSysTimer
 * R: the TiSysTimer is usually much simpler than TiTimeAdapter. it doesn't need
 * multi-channel and capture support. a simple TiSysTimer will make the upper layer
 * embedded os more easier to port to other platforms.
 *
 * Q: what's Tick?
 * R: Tick is an time unit. The system timer generates an expire event every tick
 *
 ******************************************************************************/

#define tm_value_t uint16  

typedef struct{
  uint8 state;
  //uint8 prescale_selector;
  //uint16 prescale;
  tm_value_t interval;
  uint8 reginterval;
  TiFunEventHandler listener;
  void * lisowner;
  uint16 TCCR;
  uint16 OCR3;
}TiSysTimer;

TiSysTimer * systm_construct( char * buf, uint8 size );
void systm_destroy( TiSysTimer * timer );
TiSysTimer * systm_open( TiSysTimer * timer, tm_value_t interval, TiFunEventHandler listener, void * lisowner );
void systm_close( TiSysTimer * timer );
void systm_start( TiSysTimer * timer );
void systm_stop( TiSysTimer * timer );
bool systm_expired( TiSysTimer * timer );

#ifdef __cplusplus
}
#endif

#endif /* _HAL_SYSTIMER_H_3214_ */

