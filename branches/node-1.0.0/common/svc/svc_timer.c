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


/*******************************************************************************
 * @author zhangwei(TongJi University) on 20070423
 * 	- revision today.
 * @modified by yan-shixing(TongJi University) on 20091104
 *  - revision. test ok.
 ******************************************************************************/

#include "svc_configall.h" 
#include <string.h>
#include "../hal/hal_foundation.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_led.h"
#include "../hal/hal_assert.h"
#include "svc_foundation.h"
#include "svc_timer.h"

#define VTIMER_STATE_IDLE 0
#define VTIMER_STATE_ACTIVE 1

/*******************************************************************************
 * TiTimerManager
 ******************************************************************************/

TiTimerManager * vtm_construct( void * mem, uint16 size )
{
	memset( mem, 0x00, size );
	hal_assert( sizeof(TiTimerManager) <= size );
	return (TiTimerManager *)mem;
}

void vtm_destroy( TiTimerManager * vtm )
{
	vtm_close( vtm );
}

/* @modified by zhangwei on 20091126
 * zhangwei move the timer_open() out from vtm_open(). so that the developer can
 * freely choose which hardware timer adapter to use. 
 */
TiTimerManager * vtm_open( TiTimerManager * vtm, TiTimerAdapter * timeradapter, uint16 resolution )
{
	uintx i;

	vtm->timeradapter = timeradapter;
	vtm->resolution = resolution;
	for (i=0; i<CONFIG_VTIMER_COUNT; i++)
	{
		vtm->vtable[i].state = VTIMER_STATE_IDLE;
	}

	// attention
	// assert( timeradapter is initiazed successfully and prepared to run )
	// 我们需要周期性触发timer中断
	// timer_open( timeradapter, 0, vtm_inputevent, vtm, 0x01 ); //VTM_TIMERADAPTER_ID
	timer_setinterval( timeradapter, resolution, 8 );
	timer_start( timeradapter );

	return vtm;
}

void vtm_close( TiTimerManager * vtm )
{
	uintx i;
	for (i=0; i<CONFIG_VTIMER_COUNT; i++)
	{
		if (vtm->vtable[i].state == VTIMER_STATE_IDLE)
			vti_close( &vtm->vtable[i] );
	}
}

/* vtm_inputevent
 * This function differs from vtm_evolve. It can be called in interrupt service routines.
 * While, it's unsafe to call vtm_evolve() in the interrupt service routines.
 */
void vtm_inputevent( void * vtmptr, TiEvent * e )
{
	TiTimerManager * vtm = (TiTimerManager *)vtmptr;
	hal_atomic_begin();
	vtm_forward( vtm, vtm->resolution ); 
	hal_atomic_end();
}

void vtm_evolve( void * vtmptr, TiEvent * e )
{
	TiTimerManager * vtm = (TiTimerManager *)vtmptr;
	vtm_forward( vtm, vtm->resolution ); 
}

/* vtm_forward
 * Forward all the virtual timers. If a virtual timer expires, then the related 
 * listener function will be called
 *
 * attention: For simplicity reasons, this service doesn't provide the vtm_backward() function.
 */
void vtm_forward( TiTimerManager * vtm, uint16 steps )
{
	uintx i;
	for (i=0; i<CONFIG_VTIMER_COUNT; i++)
	{
		if (vtm->vtable[i].state)
		{			
			//led_toggle(LED_YELLOW);
			//hal_delay(100);
			vti_forward( &vtm->vtable[i], steps );
		}
	}
}

uint16 vtm_resolution( TiTimerManager * vtm )
{
	return vtm->resolution;
}

/* vtm_apply
 * This function will return an available virtual timer. It will return NULL if failes.
 * 
 * @obsolete
 * The old prototype is:
 *			TiTimer * vtm_apply( TiTimerManager * vtm, uint8 * pid )
 * (*pid) is the index of the current vtimer in the vtimer array. Since the (*pid) is
 * rarely used, it was removed from the parameter list.
 */
TiTimer * vtm_apply( TiTimerManager * vtm )
{
	bool found = false;
	uintx i=0;

	for (i=0; i<CONFIG_VTIMER_COUNT; i++)
	{
		if (vtm->vtable[i].state == VTIMER_STATE_IDLE)
		{
			found = true;
			break;
		}
	}

	if (found)
	{
		//*pid = i;
		return &(vtm->vtable[i]); 
	}
	else
		return NULL;
}

void vtm_release( TiTimerManager * vtm, TiTimer * vti )
{
	vti_close( vti );
}

/*******************************************************************************
 * TiTimer
 ******************************************************************************/

/* vti_open()
 * attention: You must manually start the timer by call vti_start().
 */
TiTimer * vti_open( TiTimer * vti, TiFunEventHandler listener, void * lisowner )
{
	vti->state = VTIMER_STATE_IDLE;
	vti->listener = listener;
	vti->lisowner = lisowner;
	vti->scale = 1;
	vti->interval = (uint16)(~0);
	vti->scale_counter = 1;
	vti->interval_counter = (uint16)(~0);
	return vti;
}

void vti_close( TiTimer * vti )
{
	vti->state = VTIMER_STATE_IDLE;
}

// 实际定时时间 = scale * interval，这样可以大大扩展定时时间
// 最大定时时间 = 2^16 * 2 ^16 ms =about 2^22 s =about 2^14 m =about 2^6 hour = 2.6 days
// 这样才能满足实际使用要求
//
void vti_setscale( TiTimer * vti, uint16 scale )
{
	vti->scale = scale;
	vti->scale_counter = scale;
}

// interval的单位：毫秒ms，最大65536 ms，约65s
// option: 决定是否周期性触发还是单次触发
void vti_setinterval( TiTimer * vti, uint16 interval, uint8 option )
{
	vti->interval = interval;
	vti->option = option;
	vti->interval_counter = interval;
}

void vti_start( TiTimer * vti )
{
	vti->state = VTIMER_STATE_ACTIVE;
}

void vti_stop( TiTimer * vti )
{
	vti->state = VTIMER_STATE_IDLE;
}

// todo 注意overflow，要考虑
// steps单位ms
void vti_forward( TiTimer * vti, uint16 steps )
{
	// decrease interval counter by steps. attention to the overflow case here.
	/*uint8 test[4];
	test[0]=vti->interval_counter;
	test[1]=(vti->interval_counter>>8);
	test[2]=steps;
	test[3]=(steps>>8);
	for(uint8 i=0; i<4; i++)
		dbo_putchar(test[i]);
	*/
	if (vti->interval_counter <= steps)
	{
		vti->scale_counter --;

		// if this timer expired, then call the listener if listener isn't NULL 
		// and updates the "expired" flag.
		if (vti->scale_counter == 0)
		{
			vti->expired = true;
			if (vti->listener != NULL)
			{
				vti->listener( vti->lisowner, NULL );
				vti->expired = false;
				if (vti->option & 0x01)
				{
					vti->scale_counter = vti->scale;
					vti->interval_counter = vti->interval;
				}
			}
		}
		else vti->interval_counter = vti->interval;
	}
	else{
		vti->interval_counter -= steps;
	}
}

uint16 vti_backward( TiTimer * vti, uint16 steps )
{
	if ((uint16)~0 - vti->interval_counter >= steps )
	{
		vti->interval_counter += steps;
	}
	else{
		vti->interval_counter += steps;
		vti->scale_counter ++;
	}

	return 0;
}

bool vti_expired( TiTimer * vti )
{
	bool ret = vti->expired;
	if (ret)
		vti->expired = false;
	return ret;
}
