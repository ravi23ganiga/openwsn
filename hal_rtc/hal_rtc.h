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
#ifndef _REALTIMECLOCK_H_8318_
#define _REALTIMECLOCK_H_8318_

/******************************************************************************
 * @author Yan Shixing on 2009-12-09
 * TiRTC 
 * This object is the software encapsulation of the MCU's Real-time clock.
 *
 *****************************************************************************/
 
#include "../common/hal/hal_configall.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_target.h"

#define TiRealtimeClock TiRtc
#define TiCalendarTime TiCalTime
#define ctime_t TiCalTime

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * TiRTC Object
 * 
 *****************************************************************************/

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

  uint8  repeat;  //�ƺ��ɸ�Ϊoption��Ȼ��ͨ��option�е�һ��bit�����Ƿ�repeat���ã�����������bit����
  uint16 scale;
  uint16 interval;
  uint16 scale_counter;
  uint16 interval_counter;
  uint8  expired;

  uint8 option;
}TiRtc;

TiRtc *	   rtc_construct( char * buf, uint8 size );
void       rtc_destroy( TiRtc * rtc );
TiRtc *	   rtc_open( TiRtc * rtc, TiFunEventHandler listener, void * object, uint8 option );
void       rtc_close( TiRtc * rtc );
void       rtc_start( TiRtc * rtc );
void       rtc_stop( TiRtc * rtc );
void 	   rtc_pause( TiRtc * rtc );//��ͣ��ʱ
void 	   rtc_invertpause( TiRtc * rtc );//�������ָ�����ʱ
void       rtc_restart( TiRtc * rtc );//���¿�ʼ��ʱ
void       rtc_setvalue( TiRtc * rtc, TiCalTime * caltime );//����current_time
bool	   rtc_getvalue( TiRtc * rtc, TiCalTime * caltime );//���current_time

void 	   rtc_setexpire( TiRtc * rtc, TiCalTime * caltime, uint8 repeat);//�ú�����������δ����ʱ��RTC��ʱ��
void	   rtc_setinterval( TiRtc * rtc, uint16 interval, uint16 scale, uint8 repeat );
//�ú�����rtc_setexpire���ƣ�������ָ����ʱʱ���ж೤������rtc_setexpire�ڳ�����ֻҪ��һ����������

void       rtc_active( TiRtc * rtc );//��ʱʱ�̵�
void       rtc_expired( TiRtc * rtc );//��ʱ�����

void	   rtc_forward( TiRtc * rtc, uint16 sec );
void 	   rtc_backward( TiRtc * rtc, uint16 sec );

void       rtc_setlistener( TiRtc * rtc, TiFunEventHandler listener, void * object );


#ifdef __cplusplus
}
#endif

#endif /* _REALTIMECLOCK_H_8318_ */

