#ifndef _HAL_TIMERADAPTER_H_6828_
#define _HAL_TIMERADAPTER_H_6828_
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
 * @author zhangwei on 2006-08-11
 * TiTimerAdapter 
 * This object is the software encapsulation of the MCU's hardware timer.
 * 
 * @modified by zhangwei 2006-09-05
 * @modified by tangwen on 2006-10-21
 * 	- revision
 *
 * @modified by Shanlei on 200611
 *  - tested on arm
 *  timer测试通过。本版本说明如下：
 *  timer_setvalue 和 timer_setcapture为空函数。
 *  timer_configure的priority参数表示了timer对象是否使用中断方式，priority
 * 为0-15表示使用中断方式，>15表示不使用中断。
 *  timer_VICinit只是根据timer对象的id 和 priority 设置了VIC地址，priority
 * 的值只能为0-15超出范围不赋予地址，此函数在timer_configure函数中调用，当
 * timer_configure的参数priority在0-15之间。原本两个timer的VIC地址为同一个
 *（timer_interrupt），经测试发现，当两个timer同时进入中断的时候因为使用同一
 * 地址，会产生冲突。现在为每一个timer分配一个VIC地址（Timer0_Int for timer0 
 * and Timer1_Int for timer1）解决了这种冲突。
 *  从timer_VICinit中分离出timer_VICenable和timer_VICdisable控制整个timer的
 * 使能或禁止，这两个函数在timer_start和timer_stop中根据timer->state被调用。
 *  timer_setinterval的计时最小间隔改为1ms，参数interval的值代表ms数。
 * repeat功能恢复，repeat为0，只匹配一次；repeat为1，连续匹配。
 *  timer_enable和timer_disable是对timer的每个比较通道中断的使能或禁止，在
 * 使用timer_expired的查询方式下，也需要使能比较通道的中断，以便产生中断标志，
 * 因此，这两个函数不能作为timer真正使用中断方式的表现，他们将不改变timer-
 * >state。
 *   timer_VICinit、timer_VICenable、timer_VICdisable、timer_enable、
 * timer_disable对上层都是不可见的。
 *
 * modified by zhangwei on 200612
 * timer的启动与停止建议使用start/stop，不要使用disable和enable
 *
 * @modified by XieJing in 2009.05
 *	- ported to atmega128 MCU and GAINZ platform.
 *
 *****************************************************************************/
 
#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_targetboard.h"

/* This macro defines how many clocks the Timer run in one milli-seconds.
 * since we often use milli-seconds when dealing with scheduling and communication
 * problems, this macro is quite good for such conversion.
 *
 * @attention
 *	though atmega128 is a 8 bit MCU, we still use uint16 as tm_value_t. 
 */
#ifdef CONFIG_TARGET_GAINZ
  #define tm_value_t uint16
  #define TIMER_CLOCKS_PER_MILLISECOND  (GAINZ_CLOCK_FREQUENCY / 1000)
  #define timer_msecof(clocks) (clocks/TIMER_CLOCKS_PER_MILLISECOND)
  #define timer_clocksof(ms) (ms*TIMER_CLOCKS_PER_MILLISECOND)
#else
  #define tm_value_t uint32
  #define TIMER_CLOCKS_PER_MILLISECOND  (Fpclk / 1000)
  #define timer_msecof(clocks) (clocks/TIMER_CLOCKS_PER_MILLISECOND)
  #define timer_clocksof(ms) (ms*TIMER_CLOCKS_PER_MILLISECOND)
#endif

/******************************************************************************
 * TiTimerAdapter Object
 *  id		    timer hardware id. usually it is 0, 1, 2. it depends on hardware
 *  channel	    each sub-timer in timer hardware. usually 0-3
 *	state	    bit 7 - bit 0
 *			    bit 0: enabled or not
 *			    bit 1: period trigger or not
 *			    bit 2: raise interrupt or not
 *  listener    this's a function pointer. it will be called when the timer expired
 *              and the interrupt enabled.
 *	object	    used as the parameter for the timer callback listener function
 *  priority	the interrupt priority level. 0 is the highest and 15 is the lowest.
 *****************************************************************************/
 
#define TIMER_STATE_ENABLED 	0x01
#define TIMER_STATE_PERIOD	 	0x02
#define TIMER_STATE_INTERRUPT 	0x04
#define TIMER_STATE_CAPTURE 	0x08


typedef struct{
  uint8 id;
  uint8 channel;
  uint8 state;
  uint16 interval;
  uint16 count;
  uint8 prescale_factor;
  uint8 repeat;
  TiFunEventHandler listener;
  void * listenowner;//object 
  uint8 priority;
  tm_value_t starttime;
  uint8 option;
}TiTimerAdapter;

#ifdef __cplusplus
extern "C" {   
#endif

TiTimerAdapter* timer_construct( char * buf, uint8 size );
void       timer_destroy( TiTimerAdapter * timer );
TiTimerAdapter* timer_open( TiTimerAdapter * timer, uint8 id, TiFunEventHandler listener, void * object, uint8 option );
void       timer_close( TiTimerAdapter * timer );
void       timer_setinterval( TiTimerAdapter * timer, uint16 interval, uint8 repeat );
void       timer_start( TiTimerAdapter * timer );
void       timer_restart( TiTimerAdapter * timer, uint16 interval, uint8 repeat );
void       timer_stop( TiTimerAdapter * timer );
void       timer_enable( TiTimerAdapter * timer );
void       timer_disable( TiTimerAdapter * timer );
void       timer_setlistener( TiTimerAdapter * timer, TiFunEventHandler listener, void * object );
void       timer_setchannel( TiTimerAdapter * timer, uint8 channel );
void       timer_setprior( TiTimerAdapter * timer, uint8 prior );
void       timer_setvalue( TiTimerAdapter * timer, tm_value_t value );
tm_value_t timer_getvalue( TiTimerAdapter * timer );
tm_value_t timer_elapsed( TiTimerAdapter * timer );
bool       timer_expired( TiTimerAdapter * timer );
tm_value_t timer_clocksperms( TiTimerAdapter * timer );


#ifdef __cplusplus
}
#endif

#endif /* _HAL_TIMERADAPTER_H_6828_ */

