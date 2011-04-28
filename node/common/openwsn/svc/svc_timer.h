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

#ifndef _SVC_TIMER_H_6789_
#define _SVC_TIMER_H_6789_

/*******************************************************************************
 * @author Zhang Wei(TongJi University) on 2007.04.23
 * 	- revision today.
 * @modified by YanShixing(TongJi University) on 2009.11.04
 *  - Revised. Tested ok.
 * @modified by Zhang Wei(TongJi University) in 2011.04
 *  - Correct bugs inside vtm_open(). In the past, this function always uses the 
 *    hardware timer 0.
 *  - Revised. 
 * @modified by Jiang Ridong(TongJi University) in 2011.04
 *  - Revised. Tested.
 ******************************************************************************/


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

/**
 * CONFIG_VTIMER_COUNT
 * Decides how many virtual timer this timer manager can support. The default value
 * is 4, but it can be set to any value larger than 0 if you want. 
 */
 
#ifndef CONFIG_VTIMER_COUNT
#define CONFIG_VTIMER_COUNT 4
#endif

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
  TiTimer vtable[CONFIG_VTIMER_COUNT];
}TiTimerManager;

/*******************************************************************************
 * TiTimerManager
 ******************************************************************************/

TiTimerManager * vtm_construct( void * mem, uint16 size );
void vtm_destroy( TiTimerManager * vtm );

/**
 * Initialize the timer manager object (TiTimerManager) for further operation. 
 * 
 * @param timeradapter This's an hardware timer abstraction object(TiTimerAdapter)
 *		who drives the timer manager to forward.
 * @param resolution Decides the smallest time slice supported by the timer manager.
 *      
 * @attention For TIMER 0 hardware of the GAINZ hardware, the valid range of the 
 *		resolution parameter is 1~8 because TIMER 0 of the atmega128 microcontroller 
 *      due to it's 8 bit limitation and internal frequency division designs.
 */
TiTimerManager * vtm_open( TiTimerManager * vtm, TiTimerAdapter * timeradapter, uint16 resolution );
void vtm_close( TiTimerManager * vtm );

/**
 * Input an event to the timer manager component. Everytime the timer manager receives
 * an event, it will forward the internal counter by step 1. Usually the event will
 * be generated by the timer hardware to push the timer manager to run.
 * 
 * @attention: This function accords with the standard TiFunEventHandler prototype:
 * 		typedef (void)(* TiFunEventHandler)( void * object, TiEvent * e )
 * So It can be used as the callback handler of TiTimerAdapter(hardware timer abstraction)
 * directly. 
 */
void vtm_inputevent( void * vtmptr, TiEvent * e );

/**
 * Evolves the timer manager component. Everytime this function is called, timer 
 * manager will forward the internal counter by step 1. Usually the event will
 * be generated by the timer hardware to push the timer manager to run.
 * 
 * Q: What's the difference between vtm_inputevent() and vtm_evolve()?
 * R: vtm_inputevent() can be called in interrupt service routines. While, it's 
 * unsafe to call vtm_evolve() in the interrupt service routines.
 * 
 * @attention: This function accords with the standard TiFunEventHandler prototype:
 * 		typedef (void)(* TiFunEventHandler)( void * object, TiEvent * e )
 * So It can be used as the callback handler of TiTimerAdapter(hardware timer abstraction)
 * directly. 
 */
void vtm_evolve( void * vtmptr, TiEvent * e );

/**
 * Forward all the virtual timers. If a virtual timer expires, then the related 
 * listener function will be called
 *
 * attention: For simplicity reasons, this service doesn't provide the vtm_backward() function.
 */
void vtm_forward( TiTimerManager * vtm, uint16 steps );

/**
 * Returns the resolution of the timer manager. It's the smallest time interval
 * that all child timers can provide.
 */
uint16 vtm_resolution( TiTimerManager * vtm );

/** 
 * Apply an TiTimer object from the timer manager.
 */
TiTimer * vtm_apply( TiTimerManager * vtm) ; 

/**
 * Release a TiTimer object and return it to the timer manager. 
 */
void vtm_release( TiTimerManager * vtm, TiTimer * vti );

/*******************************************************************************
 * TiTimer
 ******************************************************************************/

/**
 * Open an TiTimer object for use. Attention you should manually start the timer 
 * by call vti_start().
 * 
 * @attention: Before you call this function, you should apply a TiTimer object
 * from the TiTimerManager object by calling vtm_apply() first. Attention the 
 * the maximum TiTimer count is configured through the macro CONFIG_VTIMER_COUNT. 
 * vtm_apply() may return NULL if failed to allocate new timer. 
 */
TiTimer * vti_open( TiTimer * vti, TiFunEventHandler listener, void * lisowner );

void vti_close( TiTimer * vti );

/**
 * Set the scale parameter of the timer object. The scale parameter is used to extend
 * the timing period. Default value is 1. 
 * 
 * real timing interval = scale * interval. 
 * 
 * For example: If the scale is 100 when interval is 8, then the real timing interval 
 * is 800 which is 100 * 8.
 * 
 * So the maximum timing interval is: 2^16 * 2 ^16 ms =about 2^22 s =about 2^14 m 
 *		=about 2^6 hour = 2.6 days
 * It can be used for day-long timing applications.
 * 
 * @param scale Scale parameter to extend the timing interval. Range from 0x0000 to 0xFFFF.
 */
void vti_setscale( TiTimer * vti, uint16 scale );

/**
 * Set the interval that the timer to be expired from now on. The real waiting time
 * is actually (interval * scale). The default scale is 1. If you want to enlarge 
 * the interval longer than the low level hardware supported, you can set the scale.
 * 
 * @param interval The time interval before the timer expired. It's based on milli-seconds(ms).
 * 		The maximum value is 65536(0xFFFF) ms, which is about 65 seconds.
 * @param option Decide whether the timer itself restarted or not after last expiration.
 *		0x00 means trigger one timer only, and 0x01 means periodically triggered.
 */
void vti_setinterval( TiTimer * vti, uint16 interval, uint8 option );

/**
 * Start the timer object. Attention this function will automatically set the internal
 * expired flag to false. 
 *
 * @attention: If the timer is already started and the internal expired flag is 
 * still keeping true, the call to function will also set the expired flag to false.
 */
void vti_start( TiTimer * vti );

/**
 * Stop the timer. Nothing happens if the timer is already stopped.
 */
void vti_stop( TiTimer * vti );

//void vti_restart( TiTimer * vti, uint16 interval, uint8 option );
void vti_restart( TiTimer * vti, uint16 interval,uint16 scale, uint8 option );//todo modified by Jiang Ridong on 2011.04.18


void vti_forward( TiTimer * vti, uint16 steps );

uint16 vti_backward( TiTimer * vti, uint16 steps );

/**
 * Returns the internal expired flag. Attention this function will also clear the 
 * expired flag if it found the flag is true. So you have only one chance to get
 * the true value in a sequence of vti_expired() calls.
 */
bool vti_expired( TiTimer * vti );

#endif
