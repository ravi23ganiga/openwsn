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
 *  timer����ͨ�������汾˵�����£�
 *  timer_setvalue �� timer_setcaptureΪ�պ�����
 *  timer_configure��priority������ʾ��timer�����Ƿ�ʹ���жϷ�ʽ��priority
 * Ϊ0-15��ʾʹ���жϷ�ʽ��>15��ʾ��ʹ���жϡ�
 *  timer_VICinitֻ�Ǹ���timer�����id �� priority ������VIC��ַ��priority
 * ��ֵֻ��Ϊ0-15������Χ�������ַ���˺�����timer_configure�����е��ã���
 * timer_configure�Ĳ���priority��0-15֮�䡣ԭ������timer��VIC��ַΪͬһ��
 *��timer_interrupt���������Է��֣�������timerͬʱ�����жϵ�ʱ����Ϊʹ��ͬһ
 * ��ַ���������ͻ������Ϊÿһ��timer����һ��VIC��ַ��Timer0_Int for timer0 
 * and Timer1_Int for timer1����������ֳ�ͻ��
 *  ��timer_VICinit�з����timer_VICenable��timer_VICdisable��������timer��
 * ʹ�ܻ��ֹ��������������timer_start��timer_stop�и���timer->state�����á�
 *  timer_setinterval�ļ�ʱ��С�����Ϊ1ms������interval��ֵ����ms����
 * repeat���ָܻ���repeatΪ0��ֻƥ��һ�Σ�repeatΪ1������ƥ�䡣
 *  timer_enable��timer_disable�Ƕ�timer��ÿ���Ƚ�ͨ���жϵ�ʹ�ܻ��ֹ����
 * ʹ��timer_expired�Ĳ�ѯ��ʽ�£�Ҳ��Ҫʹ�ܱȽ�ͨ�����жϣ��Ա�����жϱ�־��
 * ��ˣ�����������������Ϊtimer����ʹ���жϷ�ʽ�ı��֣����ǽ����ı�timer-
 * >state��
 *   timer_VICinit��timer_VICenable��timer_VICdisable��timer_enable��
 * timer_disable���ϲ㶼�ǲ��ɼ��ġ�
 *
 * modified by zhangwei on 200612
 * timer��������ֹͣ����ʹ��start/stop����Ҫʹ��disable��enable
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

