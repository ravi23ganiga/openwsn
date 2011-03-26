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


#include "../hal_configall.h"
#include <string.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>

#include "../hal_foundation.h"
#include "../hal_targetboard.h"
#include "../hal_cpu.h"
#include "../hal_interrupt.h"
#include "../hal_assert.h"
#include "../hal_timer.h"
#include "../hal_led.h"
#include "../hal_debugio.h"


/*******************************************************************************
 * atmega 128 
 * Timer 0: is the most simple one
 * CSn2 | CSn1 | CSn0
 *	000  no clock(which will turn off the timer)
 *  001  no CLK prescalar
 *  010  CLK/8
 *  011  CLK/64
 *  100  CLK/258
 *  101  CLK/1024
 *  110  counting by the down edge of Tn pin
 *  111  counting by the up edge of Tn pin
 * 
 * @attention
 * @warning
 *  in atmega128, timer 0 and 2 are 8 bit timer. timer 1 and 3 are 16 bit timer.
 * 
 * reference
 * [] AVR GCC Interrupt in WinAVR (found in your winavr)
 *    file:///D:/portable/WinAVR-20080610/doc/avr-libc/avr-libc-user-manual/group__avr__interrupts.html
 *    Attention: Do not use SIGNAL() in new code. Use ISR() instead. 
 * [] Better GCC Interrupt Macro, 2006  (obsolete, but it's still meaningful to understand)
 *    http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=37830
 * [] AVR-GCC里定义的API, 2009, 
 *    http://hi.baidu.com/tao_%CC%CE/blog/item/7441e9eee32c0bf3b3fb9545.html
 *
 * @author zhangwei, tangwen on 2006-08-02
 * Timer hardware encapsulation
 * 
 * you can reference "test\timer.*" to see how to use the TiTimerAdapter object.
 * 
 * @history
 * @modified by zhangwei on 20060802
 * integrate Tangwen's new Timer code today. but not test.
 * 
 * @modified by zhangwei on 20061021
 * add channel support. 
 * testing needed.
 *
 * @modified by shanlei on 200610xx
 *	- corrected by shanlei and test passed on arm7
 * 
 * @modified by zhangwei on 200905xx
 *	- revision 
 * @modified by 谢静(TongJi University) on 20090530
 *	- new version implemention on atmegal 128 MCU (ICT GAINZ device)
 *  - in testing now.
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * interrupt service routine related functions
 * here implements service routines for each timer hardware. in principle, different 
 * hardware may share the same interrupt service routine.
 ******************************************************************************/

static inline void _timer_interrupt_handler( void * object, TiEvent * e );


/*******************************************************************************
 * timer interface implementations 
 ******************************************************************************/

/* construct a TiTimerAdapter object in the memory. no hardware operations in this 
 * step 
 */
TiTimerAdapter * timer_construct( char * buf, uint8 size )
{
	hal_assert( sizeof(TiTimerAdapter) <= size );
	memset( buf, 0x00, size );
	return (TiTimerAdapter *)buf;
}

void timer_destroy( TiTimerAdapter * timer )
{
	timer_disable( timer );
}

/* @param
 *	option		bit7...bit0
 *              bit0 == 0 interrupt driven bit (default), bit0 == 1, query driven
 */
TiTimerAdapter * timer_open( TiTimerAdapter * timer, uint8 id, TiFunEventHandler listener, void * listenowner, uint8 option )
{
    timer->id = id;
	timer->listener = listener;
	timer->listenowner = listenowner;
	timer->option = option;

	if ((option & 0x01) == 1)//当option为1时，为中断模式；否则为查询模式。——Yan
	{
	switch (timer->id)
	{
	case 0:
		hal_attachhandler( INTNUM_TIMER0_COMP, _timer_interrupt_handler, timer );
		break;
	case 1:
		hal_attachhandler( INTNUM_TIMER1_COMPA, _timer_interrupt_handler, timer );
		break;
	case 2:
		hal_attachhandler( INTNUM_TIMER2_COMP, _timer_interrupt_handler, timer );
		break;
	case 3:
		hal_attachhandler( INTNUM_TIMER3_COMPA, _timer_interrupt_handler, timer );
		break;
	}
	}
	return timer;
}

void timer_close( TiTimerAdapter * timer )
{
	timer_stop( timer );


	if ((timer->option & 0x01) == 1)
	{
	switch (timer->id)
	{
	case 0:
		hal_detachhandler( INTNUM_TIMER0_COMP );
		break;
	case 1:
		hal_detachhandler( INTNUM_TIMER1_COMPA );
		break;
	case 2:
		hal_detachhandler( INTNUM_TIMER2_COMP );
		break;
	case 3:
		hal_detachhandler( INTNUM_TIMER3_COMPA );
		break;
	}
	}
}

/* interval duration = system clock / prescale / count
 * for GAINZ hardware platform, system clock is 8 MHz
 *
 * prescale_factor | prescale 
 *        7            1024
 *        6             256
 *        5             128
 *        4              64
 *        3              32
 *        2              8
 *        1              1
 *        0        timer stop
 */

//此函数貌似无用，建议删除——Yan

void timer_setinterval_base( TiTimerAdapter * timer, uint8 prescale_factor, uint16 count, uint8 repeat )
{
	timer->prescale_factor = prescale_factor;
	timer->count = count;
	timer->repeat = repeat;
}

/* the value of "interval" is important! it should NOT be too small
 * to enable the application process the timer interrupt successfully. and it
 * should NOT be too large so that no calculation overflow occured! 
 *
 * param
 *	interval       determins when the timer expires from start() call. (milliseconds)
 *	repeat         decide whether the timer should work periodically
 *                 TRUE/1   period triggering
 *                 FALSE/0  trigger only once
 * 
 * result
 *	nothing happens if failed.
 *
 * @warning
 * @attention
 *	be careful with your input values! the automatic tuning may generate wrong 
 * results inside. 
 */

 //repeat参数在此timerAdapter尚未定义功能。——YAN
void timer_setinterval( TiTimerAdapter * timer, uint16 interval, uint8 repeat )
{
	// if ((timer->interval == interval) && (timer->repeat == repeat))
	// 	return;

	uint16 idx=0;
	uint16 pulscount = interval * (CONFIG_GAINZ_CLOCK_FREQUENCY / 1000);//interval的范围1~8
	bool found = false;

	/* searching for the largest bit of parameter "interval" */
	for (idx=0; idx<16; idx++)
	{
		if (pulscount & (1 << (15-idx)))
		{
			found = true;
			break;
		}
	}

	if (found)
		{idx = 15 - idx;
		}
	else
		return;


	timer->interval = interval;
	timer->repeat = repeat;

	switch (timer->id){
	case 0:
//just for timer0
	/* prescale_factor | prescale 
	 *        7            1024
	 *        6             256
	 *        5             128
	 *        4              64
	 *        3              32
	 *        2              8
	 *        1              1
	 *        0        timer stop
	 */
	if (idx > 11)
	{
		timer->prescale_factor = 7; // prescale is 1024
		timer->count = (pulscount & 0xFC00) >> 10;
	}
	else if (idx > 9)
	{
		timer->prescale_factor = 6;//prescale is 256
		timer->count = (pulscount & 0xFF00) >> 8;
	}
	else if (idx > 7)
	{
		timer->prescale_factor = 4;//prescale is 64
		timer->count = (pulscount & 0xFFC0) >> 6;
	}
	else if (idx > 4)
	{
		timer->prescale_factor = 2;//prescale is 8
		timer->count = (pulscount & 0xFFF8) >> 3;
	}
	else
	{
		timer->prescale_factor = 1;//prescale is 1
		timer->count = pulscount;
	}
	break;

	case 2:
	//default:
	/* prescale_factor | prescale 
	 *        7             T2引脚外部时钟源，上升沿有效
	 *        6             T2引脚外部时钟源，下降沿有效
	 *        5             1024
	 *        4              256
	 *        3              64
	 *        2              8
	 *        1              1
	 *        0        timer stop
	 */
	if (idx > 11)
	{
		timer->prescale_factor = 5; // prescale is 1024
		timer->count = (pulscount & 0xFC00) >> 10;
	}
	else if (idx > 9)
	{
		timer->prescale_factor = 4;//prescale is 256
		timer->count = (pulscount & 0xFF00) >> 8;
	}
	else if (idx > 7)
	{
		timer->prescale_factor = 3;//prescale is 64
		timer->count = (pulscount & 0xFFC0) >> 6;
	}
	else if (idx > 4)
	{
		timer->prescale_factor = 2;//prescale is 8
		timer->count = (pulscount & 0xFFF8) >> 3;
	}
	else
	{
		timer->prescale_factor = 1;//prescale is 1
		timer->count = pulscount;
	}
	break;
	}
	
	//timer->option = repeat;
}

void timer_start( TiTimerAdapter * timer )
{
	/* set prescalar according to timer->interval and timer->repeat */
	//todo 
	//dbo_putchar(timer->count);
	//dbo_putchar(timer->prescale_factor);

    // TCCR: timer control register

	switch (timer->id)
	{
	case 0://8位计时器

		// for overflow mode
		//TCNT0 = 0xFF - timer->count;
		//TCCR0 = timer->prescale_factor;
		//led_on(LED_RED);
		//loop_until_bit_is_set(TIFR,TOV0);中断标志位为TOV0.

		//上面为TCCR0的前几位设为0，将timer设为普通模式，但个人认为使用CTC模式更优。
		//因为普通模式写入TCNT0后，计数计到0xFF后中断，返回0，若要再次计数需要重写TCNT0；
		//而CTC模式为计数记到OCR0后，中断返回0；再计数到OCR0，再中断；
		//CTC模式为上层timer调用时，只需记录中断次数就可重复使用；而普通模式需要记录中断且重写TCNT0，当然如果使用最大计数值则无此顾虑
		//但是注意两种模式的中断标志位不同。


		//CTC mode
		OCR0 = timer->count;
		TCCR0 = (_BV(WGM01) & (~_BV(WGM00)));
		TCCR0 |= timer->prescale_factor;
		
		//loop_until_bit_is_set(TIFR,OCF0);中断标志为OCF0.

		break;

	case 1://16位计时器
		// for CTC mode
		
		OCR1A = 0x005f;//timer->count;
	    TCCR1A=0;       //WGM13:0 = 1 1 0 0  for CTC mode
		TCCR1B=(~_BV(WGM13) & (_BV(WGM12)));
		TCCR1B |= 0x05;  //timer->prescale_factor;
		//loop_until_bit_is_set(TIFR,TOV1);中断标志为TOV1.
		break;

	case 2:
		// for overflow mode
		//TCNT2 = 0xFF - timer->count;
    	//TCCR2 = timer->prescale_factor;
		//loop_until_bit_is_set(TIFR,TOV2);中断标志位为TOV2.

		//CTC mode
		   
		OCR2 = timer->count;
		TCCR2 = (_BV(WGM21) & (~_BV(WGM20)));
		TCCR2 |= timer->prescale_factor;

		//loop_until_bit_is_set(TIFR,OCF2);中断标志为OCF2.

		break;
	case 3:
		// for overflow mode
		//TCNT3 = 0xFFFF - timer->count;
	    //TCCR3A=0;
		//TCCR3B= timer->prescale_factor;
		//loop_until_bit_is_set(ETIFR,TOV3);中断标志为TOV3.
			//for CTC mode
		OCR3A= 0X005F;//timer->count;
		TCCR3A=0;
		TCCR3B=(~_BV(WGM33) & (_BV(WGM32)));
		TCCR3B |= 0x05;  //timer->prescale_factor;
	
		break;
	}

	if ((timer->option & 0x01) == 0x01)
		timer_enable(timer);
}

/* a value of zero of the prescalar value in the TCCRx will stop the cooresponding timer.
 * attention the prescalar is still running
 */
void timer_stop( TiTimerAdapter * timer )
{
	if ((timer->option & 0x01) == 0x01)
		timer_disable(timer);

	/* when the prescale setting {CS02, CS01, CS00} is 000, the clock will stoped. */
	uint8 mask = ~((1 << CS02) | (1 << CS01) | (1 << CS00));

	switch (timer->id)
	{
	case 0:
		TCCR0 &= mask;
		break;
	case 1:
		TCCR1B &= mask;
		break;
	case 2:
		TCCR2 &= mask;
		break;
	case 3:
		TCCR3B &= mask;
		break;
	}
}

void timer_restart( TiTimerAdapter * timer, uint16 interval, uint8 repeat )
{
	/* attention: 
	 * assume timer is already stoped here */
	timer_setinterval( timer, interval, repeat );
	timer_start( timer );
}


/* enable timer interrupt 
 * timer interrupt related registers: TIFR, ETIFR, TIMSK, ETIMSK
 * ref to atmega128 datasheet
 */
void timer_enable( TiTimerAdapter * timer )
{
	if ((timer->option & 0x01) == 0x01)
	{  
		switch (timer->id)
		{
		case 0:
			/* for overflow mode */
			// clear TOV0 / clear pending interrupt
			//TIFR = (1 << TOV0);				
			// enable timer0 overflow interrupt
			//TIMSK = (1 << TOIE0); 

			/* for output compare match mode */
			TIFR = (1 << OCF0);

			// enable timer0 compare match mode 
			TIMSK = (1 << OCIE0);
			
			break;
		case 1:
			/* for overflow interrupt */
			//TIFR = (1 << TOV1); 
			//TIMSK = _BV(TOIE1);

			/* for output compare match mode */
			
	        TIMSK = _BV(OCIE1A);
  			TIFR |= (1 << OCF1A); 
			
			// TIMSK |= _BV(TICIE1);
			
			break;
		case 2:
			/* for overflow mode */
			// enable timer2 overflow interrupt
			//TIFR = (1 << TOV2); 
			//TIMSK = (1 << TOIE2); 

			/* for output compare match mode */
		 	
			TIFR = (1 << OCF2);
			TIMSK =(1 << OCIE2);
			
			break;
		case 3:
			/* for overflow interrupt */
		//	ETIFR = (1 << TOV3); 
		//	ETIMSK = _BV(TOIE3);

			/* for output compare match mode */
			/* 
			ETIFR = ((1 << OCF3B) | (1 << OCF3A) | (1 << OCF3C));
			ETIMSK = (_BV(OCIE3A) | _BV(OCIE3A) | _BV(OCIE3C));
			//ETIMSK |= 0x10; // OCIE3A bit set
			// TIMSK |= _BV(TICIE1);
			*/
			ETIFR = (1 << OCF3A); 
			ETIMSK = _BV(OCIE3A);
			break;
		}
	}
}

/* disable timer interrupt */
void timer_disable( TiTimerAdapter * timer )
{
	if ((timer->option & 0x01) == 0x01)
	{
		switch (timer->id)
		{
		case 0:
			TIMSK &= ~_BV(OCIE0);
			break;
		case 1:
			TIMSK &= ~_BV(OCIE1A);
			break;
		case 2:
			TIMSK &= ~_BV(OCIE0);
			break;
		case 3:
			ETIMSK &= ~_BV(OCIE3A);
			break;
		}
	}
}

/* listener provides an mechnism that the TiTimerAdapter object can notify some 
 * object the current setting has been expired. 
 * 
 * attention: the listener function will only called when the interrupt is enabled.
 * the notifier function will be run in the interrupt mode. if the interrupt is
 * disabled, you should call expired() to check whether the timer expires manually.
 */
void timer_setlistener( TiTimerAdapter * timer, TiFunEventHandler listener, void * listenowner )
{
	timer->listener = listener;
	timer->listenowner = listenowner;
}

/* MCU的timer对象出于控制需要，经常包含多个channel，本函数用于选择channel 
 * 但是在该系统中，很少用到channel
 */
void timer_setchannel( TiTimerAdapter * timer, uint8 channel )
{
	timer->channel = channel;
	// todo
}

void timer_setprior( TiTimerAdapter * timer, uint8 prior )
{
	timer->priority = prior;
}

void timer_setvalue( TiTimerAdapter * timer, tm_value_t value )
{
}

tm_value_t timer_getvalue( TiTimerAdapter * timer )
{
	// read register and return the current timestamp
	return 0;
}

/* @return
 * @warning
 * 	return's the time elapsed between two this function calls.
 * 	the returned value maybe incorrect if the time duration is too long when it
 * is exceed the maximum limit of an tm_value_t integer.
 *  the developer should deal with such conditions. the timer object itself cannot
 * avoid such overflow problems.
 */
tm_value_t timer_elapsed( TiTimerAdapter * timer )
{
	tm_value_t curtime, ret;
	
	curtime = timer_getvalue( timer );
	if (curtime > timer->starttime)
		ret = curtime - timer->starttime;
	else
		ret = ((tm_value_t)(~0x00)) - timer->starttime + curtime;
	
	timer->starttime = curtime;
	return ret;
}

/* to judge whether the timer object is expired or not. this function will reset
 * the expired flag automatically. since the expired flag is often a bit in the 
 * timer's state register, it may also be cleared by the hardware when you read 
 * it out 
 */
bool timer_expired( TiTimerAdapter * timer )
{
	bool ret = false;
	// todo
	switch (timer->id)
	{
	case 0:
		// use (TIFR & _BV(TOV0)) for timer overflow flag
		// use (TIFR & _BV(OCF0)) for timer CTC flag 
 
		ret = (TIFR & _BV(OCF0));
		if (ret)
        {
			// clear the TOV0 bit by writing 1
			TIFR |= _BV(OCF0);
		}
		break;
	case 1:
			ret = (TIFR & _BV(OCF1A));
		if (ret)
        {
			// clear the TOV1 bit by writing 1
			TIFR |= _BV(OCF1A);
		}
		break;

	case 2:
			ret = (TIFR & _BV(OCF2));
		if (ret)
        {
			// clear the TOV2 bit by writing 1
			TIFR |= _BV(OCF2);
		}
		break;

	case 3:
			ret = (ETIFR & _BV(OCF3A));
		if (ret)
        {
			// clear the TOV3 bit by writing 1
			ETIFR |= _BV(OCF3A);
		}
		break;

		break;
	}
	return ret;
}


tm_value_t timer_clocksperms( TiTimerAdapter * timer )
{
	#ifdef CONFIG_MCU_ARM7
	return TIMER_CLOCKS_PER_MILLISECOND;
	#endif

	#ifdef CONFIG_MCU_ATMEGA
	// todo
	return 8;
	#endif

	return 1000;
}

/* old source code

uint8_t Timer3_setIntervalAndScale(uint16_t interval, uint8_t scale)
{

    if (scale > 7) 
    {
		return FAIL;
    }

    cpu_atomic_t atomic = _cpu_atomic_begin();
    {
      
	OCR3A = interval;//this is the scale
	OCR3B = 0x0000;
	OCR3C = 0x0000;
	TCNT3 = 0x0000; //this is the counter
	TCCR3A = 0x00;//Output: A: Disconnected, B: Disconnected, C: Disconnected,WGM[1..0]=00
	scale |= 0x08;//
	TCCR3B |= scale;//clk/(scale),WGM[3..2]=01
	TCCR3C = 0x00;//
	ETIMSK |= 0x10;//OCIE3 is enable 
	
    }
    _cpu_atomic_end(atomic); 
    return SUCCESS;
}

void  Timer3_sethalfsymbol(uint16_t symbols)
{  	
 
      uint16 halfsymbols;
      halfsymbols=symbols*2;
      Timer3_setIntervalAndScale(halfsymbols, 0x5);
     
}

uint16_t Timer3_setInterval(uint16_t interval)
{
      uint32 temp;
       temp=interval;
 	if (temp>=65535) temp=temp-65535;
	return (temp);
}
*/

/*******************************************************************************
 * interrupt service routine related functions
 * here implements service routines for each timer hardware. in principle, different 
 * hardware may share the same interrupt service routine.
 ******************************************************************************/

/* @attention
 *	- this function will be called by the hal_invokehandler() function. so you 
 * must call hal_attachhandler() and configure the interrupt number - handler map
 * before you can call it successfully inside hal_invokehandler().
 *	- this function is still inside the interrupt context. 
 *  - you can distinguish the which timer raised the interrupt by the parameter "object".
 */ 
void _timer_interrupt_handler( void * object, TiEvent * e )
{
	//led_twinkle( LED_RED | LED_GREEN, 1000 );

    TiTimerAdapter * timer = (TiTimerAdapter *)object;
	//timer = timer;
	//TiEvent ev;

	/* You can use timer->id to identify the current timer */

	/* cpu_atomic_t atomic = _cpu_atomic_begin();
	{
      	//if (Timer3_set_flag!=0) 
		{
			//OCR3A  = Timer3_setInterval(2000);
	    	//Timer3_set_flag--;
      	}
    }
    _cpu_atomic_end(atomic); 
	*/
	//memset( &ev, 0x00, sizeof(TiEvent) );
	//dbo_putchar(0x12);
	
	//ev.id = EVENT_TIMER_EXPIRED;
	if (timer->listener != NULL)
		timer->listener(timer->listenowner, NULL);//timer->listener( NULL, NULL );--------------Modified by Yan Shixing, 2009.11.5
}

#ifdef __cplusplus
}
#endif
