#ifndef _HAL_RTC_H_3888_
#define _HAL_RTC_H_3888_
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
 * @author Yan Shixing on 2009-12-09
 * TiRtcAdapter 
 * This object is the software encapsulation of the MCU's Real-time clock.
 *
 *****************************************************************************/
 
#include "hal_configall.h"
#include "hal_foundation.h"

/* TiRtcAdapterTime
 * represent the int type used by the RTC hardware. it varies from 16bit to 32 bit 
 * on different hardware platform. 
 */
#ifdef CONFIG_TARGET_GAINZ 
  #define TiRtcAdapterTime uint16
#else
  #pragma error "you should choose correct TiRtcAdapterTime type on your hardware architecture!"
#endif

/* TiCalendarTime
 * this's a universal time type. an calendar time variable occupies 10 bytes in the 
 * memory. it can be transmitted over the network from one device to another device.
 */

#define TiRealtimeClock TiRtcAdapter  
#define TiRtc TiRtcAdapter
//#define TiCalendarTime TiCalTime
//#define ctime_t TiCalTime

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * TiRtcAdapter
 * 
 *****************************************************************************/

/* 10 byte representation of accurate timestamp
 * [year 2B][month 1B][day 1B][hour 1B][min 1B][sec 1B][msec 2B][usec 2B]
 * control: highest 2bit. always 00
 * year: 0-9999 14b 
 * reserved 4b
 * month: 1-12, 4b
 * day: 1-31: 5b
 * hour: 0-23: 5b
 * min: 6b
 * sec: 6b
 * msec: 0-999, 10b  (or using the highest 6b to represent usec. each unit represent 2^4=16us)
 * us: 0-999, 10b
 */

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
  TiCalTime * current_time;
  TiCalTime * timing_time;

  TiFunEventHandler listener;
  void * lisowner;

  uint8  repeat;  //似乎可改为option，然后通过option中的一个bit控制是否repeat更好，留出其他的bit备用
  uint16 scale;
  uint16 interval;
  uint16 scale_counter;
  uint16 interval_counter;
  uint8  expired;

  uint8 option;
}TiRtcAdapter;

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

TiRtcAdapter *	   rtc_construct( char * buf, uint8 size );
void       rtc_destroy( TiRtcAdapter * rtc );
TiRtcAdapter *	   rtc_open( TiRtcAdapter * rtc, TiFunEventHandler listener, void * object, uint8 option );
void       rtc_close( TiRtcAdapter * rtc );
void       rtc_start( TiRtcAdapter * rtc );
void       rtc_stop( TiRtcAdapter * rtc );
void 	   rtc_pause( TiRtcAdapter * rtc );//暂停计时
void 	   rtc_invertpause( TiRtcAdapter * rtc );//继续（恢复）计时
void       rtc_restart( TiRtcAdapter * rtc );//重新开始计时
void       rtc_setvalue( TiRtcAdapter * rtc, TiCalTime * caltime );//设置current_time
bool	   rtc_getvalue( TiRtcAdapter * rtc, TiRtcAdapter * to );//获得current_time

void 	   rtc_setexpire( TiRtcAdapter * rtc, TiCalTime * caltime, uint8 repeat);//该函数用于设置未来何时刻RTC定时到
void	   rtc_setinterval( TiRtcAdapter * rtc, uint16 interval, uint16 scale, uint8 repeat );
//该函数与rtc_setexpire类似，但是是指定定时时段有多长，他和rtc_setexpire在程序中只要用一个函数即可

void       rtc_active( TiRtcAdapter * rtc );//定时时刻到
void       rtc_expired( TiRtcAdapter * rtc );//定时间隔到

void	   rtc_forward( TiRtcAdapter * rtc, uint16 sec );
void 	   rtc_backward( TiRtcAdapter * rtc, uint16 sec );

void       rtc_setlistener( TiRtcAdapter * rtc, TiFunEventHandler listener, void * object );


#ifdef __cplusplus
}
#endif

#endif /* _HAL_RTC_H_3888_ */

