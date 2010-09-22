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
#include "../hal_foundation.h"
#include "../hal_cpu.h"
#include "../hal_interrupt.h"
#include "..//hal_timer.h"
#include "../hal_debugio.h"
#include "../hal_uart.h"
#include "../hal_led.h"
#include "../hal_assert.h"
#include "../hal_targetboard.h"
#include "../hal_rtc.h"


/* attention:
 *  - atmega128L actually use TIMER0 hardware as RTC. so if you use RTC, then you 
 * cannot use TIMER0. the mainly difference between general TIMER0 and RTC are their 
 * clock source. the RTC uses the special RTC oscillator (usually 32768 KHz).
 */


static inline void _rtc_interrupt_handler( void * object, TiEvent * e );


TiRtcAdapter * rtc_construct( char * buf, uint8 size )
{
	hal_assert( sizeof(TiRtcAdapter) <= size );
	memset( buf, 0x00, size );
	return (TiRtcAdapter *)buf;
}

void rtc_destroy( TiRtcAdapter * rtc )
{
	
}

TiRtcAdapter * rtc_open( TiRtcAdapter * rtc, TiFunEventHandler listener, void * object, uint8 option )
{
	rtc->listener = listener;
  	rtc->lisowner = object;
  	rtc->option = option;
	
	hal_attachhandler( INTNUM_TIMER0_OVF, _rtc_interrupt_handler, rtc );

	return rtc;
}

void rtc_close( TiRtcAdapter * rtc )
{
	hal_detachhandler( INTNUM_TIMER0_OVF );
}

void rtc_start( TiRtcAdapter * rtc )
{
	TIFR = (1 << TOV0);
	TIMSK &= ~((1<<TOIE0)|(1<<OCIE0));//关闭定时器0中断以避免异常。

	rtc->current_time->sec = 0;
	rtc->current_time->min = 0;
	rtc->current_time->hour = 0;
	rtc->current_time->day = 1;
	rtc->current_time->month = 1;
	rtc->current_time->year = 2000;


	ASSR |= (1<<AS0);//将定时器0置为异步方式，使用独立晶振。
	TCNT0 = 0x00;
	TCCR0 = 0x05;//设置预分频器，当晶振为32.768K时，溢出周期为1s。
	while(ASSR & 0x07);//等待定时器0各寄存器更新结束，切换到异步状态。
	
	TIFR = (1 << TOV0);
	TIMSK |= (1<<TOIE0);//打开定时器0溢出中断，开始计时。
}

void rtc_pause( TiRtcAdapter * rtc )
{
	TIFR = (1 << TOV0);
	TIMSK &= ~(1<<TOIE0);//关闭定时器0溢出中断。
}
	
void rtc_invertpause( TiRtcAdapter * rtc )
{
	TIFR = (1 << TOV0);
	TIMSK |= (1<<TOIE0);//打开定时器0溢出中断。
}

void rtc_stop( TiRtcAdapter * rtc )
{
	TIFR = (1 << TOV0);
	TIMSK &= ~(1<<TOIE0);//关闭定时器0溢出中断。
	ASSR &= ~(1<<AS0);//将定时器0置为普通方式。
	
	TCNT0 = 0x00;
	TCCR0 = 0x00;
	while(ASSR & 0x07);//判断同步、异步切换结束，保证后续程序正常运行。

}

void rtc_restart( TiRtcAdapter * rtc )
{
	rtc_start( rtc );

}

void rtc_setvalue( TiRtcAdapter * rtc, TiCalTime * caltime )
{
	rtc_pause( rtc );

	rtc->current_time = caltime;

	rtc_invertpause( rtc );
}

bool rtc_getvalue( TiRtcAdapter * rtc, TiRtcAdapter * to )
{
	bool ret = false;
	to->current_time = rtc->current_time;
	if(to->current_time != NULL )
	{
		ret = true;
	}
	return ret;
}

void rtc_setexpire( TiRtcAdapter * rtc, TiCalTime *caltime, uint8 repeat )
{
	rtc->timing_time = caltime;
	rtc->repeat =  repeat;
	rtc->option = 0x01;
}

void rtc_setinterval( TiRtcAdapter * rtc, uint16 interval, uint16 scale, uint8 repeat )
{
	rtc->scale = scale;
	rtc->interval = interval;
	rtc->repeat = repeat;
	rtc->scale_counter = scale;
	rtc->interval_counter = interval;
}

void rtc_active( TiRtcAdapter * rtc )
{
	if(rtc->option == 0x01)
	{
		if ((rtc->current_time->year == rtc->timing_time->year) &&
            (rtc->current_time->month == rtc->timing_time->month) &&
            (rtc->current_time->day == rtc->timing_time->day) &&
            (rtc->current_time->hour == rtc->timing_time->hour) &&
            (rtc->current_time->min == rtc->timing_time->min) &&
            (rtc->current_time->sec == rtc->timing_time->sec))
		{
			rtc->expired = true;
			if (rtc->listener != NULL)
			{
				rtc->listener( rtc->lisowner, NULL );
				rtc->expired = false;
			}
			if (rtc->repeat == 0x00)
			{
				rtc->option = 0x00;
			}
		}
	}
}

void rtc_expired( TiRtcAdapter * rtc )
{
	if( rtc->interval_counter == 0 )
	{
		rtc->scale_counter --;
		if (rtc->scale_counter == 0)
		{
			rtc->expired = true;
			if (rtc->listener != NULL)
			{
				rtc->listener( rtc->lisowner, NULL );
				rtc->expired = false;
				if (rtc->repeat == 0x01)
				{
					rtc->scale_counter = rtc->scale;
					rtc->interval_counter = rtc->interval;
				}
			}
		}
		else rtc->interval_counter = rtc->interval;
	}
	else
	{
		rtc->interval_counter --;
	}
}


char not_leap( TiRtcAdapter * rtc ) //check for leap year
{
	if (!(rtc->current_time->year%100))
		return (char)(rtc->current_time->year%400);
	else
		return (char)(rtc->current_time->year%4);
}

void rtc_update( TiRtcAdapter * rtc)
{
	//refer to the application note of atmega128
	if (++rtc->current_time->sec==60)
	{
		rtc->current_time->sec=0;
		if (++rtc->current_time->min==60)
		{
			rtc->current_time->min=0;
			if (++rtc->current_time->hour==24)
			{
				rtc->current_time->hour=0;
				if (++rtc->current_time->day==32)
				{
					rtc->current_time->month++;
					rtc->current_time->day=1;
				}
				else if (rtc->current_time->day==31)
				{
					if ((rtc->current_time->month==4) || (rtc->current_time->month==6) || (rtc->current_time->month==9) ||(rtc->current_time->month==11))
					{
						rtc->current_time->month++;
						rtc->current_time->day=1;
					}
				}
				else if (rtc->current_time->day==30)
				{
					if(rtc->current_time->month==2)
					{
						rtc->current_time->month++;
						rtc->current_time->day=1;
					}
				}
				else if (rtc->current_time->day==29)
				{
					if((rtc->current_time->month==2) && (not_leap(rtc)))
					{
						rtc->current_time->month++;
						rtc->current_time->day=1;
					}
				}
				if (rtc->current_time->month==13)
				{
					rtc->current_time->month=1;
					rtc->current_time->year++;
				}
			}
		}
	}
}

void _rtc_interrupt_handler( void * object, TiEvent * e )
{
	TiRtcAdapter * rtc = (TiRtcAdapter *)object;
	
	rtc_update( rtc );//更新时间
	//判断是否定时时刻到。
	rtc_expired( rtc );//判断是否定时间隔到。
	

} 
