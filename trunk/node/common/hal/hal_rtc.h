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

#ifndef _HAL_RTC_H_3888_
#define _HAL_RTC_H_7325_

#define TiRealtimeClock TiRTC
#define TiCalendarTime TiCalTime
#define ctime_t TiCalTime

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
  uint16 year;
  uint8  month;
  uint8  day;
  uint8  hour;
  uint8  min;
  uint8  sec;
  uint16 msec;
}TiCalTime;

typedef struct{
  uint8 state;
  TiFunEventHandler listener;
  void * lisowner;
  uint8 prescale_selector;
  uint16 scale;
  uint16 interval;
  uint16 scale_counter;
  uint16 interval_counter;
  uint8 option;
  uint8 expired;
}TiRTC;

/* construct, destroy, open and close a TiRTC object */

TiRTC * rtc_construct( void * mem, uint16 size );
void rtc_destroy( TiRTC * rtc );
TiRTC * rtc_open( TiRTC * rtc, TiFunEventHandler listener, void * lisowner );
void rtc_close( TiRTC * rtc );

/* start or stop the running of a TiRTC object, and time management functions */

void rtc_start( TiRTC * rtc );
void rtc_stop( TiRTC * rtc );
bool rtc_settime( TiRTC * rtc, TiCalTime * caltime );
bool rtc_gettime( TiRTC * rtc, TiCalTime * caltime );
void rtc_forward( TiRTC * rtc, uint16 sec );
void rtc_backward( TiRTC * rtc, uint16 sec );

inline void rtc_adjust( TiRTC * rtc, int16 sec )
{ 
	if (sec > 0)
		rtc_forward( rtc, sec );
	else
		rtc_backward( rtc, -sec );
}

/* Configure the RTC fire time. There's two methods: 
 *	- Fire according to fixed time everyday;
 *	- Fire according to interval setting
 * When the RTC expired, rtc_expired() will return true for only 1 time. and the 
 * callback listener function will be called if it's not NULL.
 * 
 * option decided whether to fire periodically.
 *
 * rtc_setexpire()
 * set the condition when the RTC should expire and to call the listener 
 * 
 * rtc_set_scale_selector()
 * set the prescale selector of the RTC hardware. Refer to the datasheet or the application
 * note to get the prescale selector settings.
 *
 * rtc_setinterval()
 * 
 *
 * rtc_expired()
 */

void rtc_setexpire( TiRTC * rtc, TiCalTime * caltime, uint8 option );
void rtc_set_scale_selector( TiRTC * rtc, uint8 selector );
void rtc_setinterval( TiRTC * rtc, uint16 interval, uint8 option );
bool rtc_expired( TiRTC * rtc );

#ifdef __cplusplus
}
#endif

#endif