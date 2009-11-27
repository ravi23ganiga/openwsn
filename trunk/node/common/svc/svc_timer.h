#ifndef _SVC_TIMER_H_6789_
#define _SVC_TIMER_H_6789_
/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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

/* TiTimer
 * TiTimer enhances the hardware related TiTimerAdapter object by:
 *	- Multiplex one single hardware timer to multi software virtual timers. Thus
 * you can freely use these timers in the application. 
 *  - Support longer delay to maximum xxx seconds. While the maximum duration for
 * a hardware timer is restricted by the register width (8 bit or 16 bit).  
 *  - The time settings are all based on milli-second. The developer needn't care 
 * the transform between physical time and the calendar timer. 
 * 
 * Besides the above advantage, there's also some disadvantage of the TiTimer: 
 *	- Doesn't support Capture/PWM functions of the hardware timer. 
 *  - The foundation timer must be always on. 
 *
 * Attention: The minimal duration is decided by the base hardware timer's tick. 
 * There's no best tick settings. The smaller tick settings will improve the response
 * of the application, however, it will also lead to heavy loads. 
 */

#include "svc_configall.h"  
#include "../hal/hal_timer.h"
#include "svc_foundation.h"


#define CONFIG_VTIMER_COUNT             4
#define CONFIG_VTIMER_RESOLUTION        1
#define CONFIG_VTIMER_SCALE             1


typedef struct{
  uint8 state;
  TiFunEventHandler listener;
  void * lisowner;
  uint16 scale;
  uint16 interval;
  uint16 scale_counter;
  uint16 interval_counter;
  uint8 option;
  uint8 expired;
}TiTimer;

typedef struct{
  TiTimerAdapter * timeradapter; 
  uint16 resolution;
  TiTimer vtable[ CONFIG_VTIMER_COUNT ];
}TiTimerManager;

/*******************************************************************************
 * TiTimerManager
 ******************************************************************************/

TiTimerManager * vtm_construct( void * mem, uint16 size );
void vtm_destroy( TiTimerManager * vtm );
TiTimerManager * vtm_open( TiTimerManager * vtm, TiTimerAdapter * timeradapter, uint16 resolution );
void vtm_close( TiTimerManager * vtm );

void vtm_inputevent( void * vtmptr, TiEvent * e );
void vtm_evolve( void * vtmptr, TiEvent * e );
void vtm_forward( TiTimerManager * vtm, uint16 steps );
uint16 vtm_resolution( TiTimerManager * vtm );
TiTimer * vtm_apply( TiTimerManager * vtm) ; 
void vtm_release( TiTimerManager * vtm, TiTimer * vti );

/*******************************************************************************
 * TiTimer
 ******************************************************************************/

TiTimer * vti_open( TiTimer * vti, TiFunEventHandler listener, void * lisowner );
void vti_close( TiTimer * vti );
void vti_setscale( TiTimer * vti, uint16 scale );
void vti_setinterval( TiTimer * vti, uint16 interval, uint8 option );
void vti_start( TiTimer * vti );
void vti_stop( TiTimer * vti );
void vti_forward( TiTimer * vti, uint16 steps );
uint16 vti_backward( TiTimer * vti, uint16 steps );
bool vti_expired( TiTimer * vti );

#endif
