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

/**
 * TiDate
 * Date�������2000��1��1�� 00:00:00Ϊ��׼����ʾ����֮���֮ǰ��������������1��ʼ��
 * TiDate uses 4B to store the date based on 2000.01.01 00:00:00:0000. It's the day 
 * count from that point. Attention we only need 14b(0-16384) to represent 0-9999, 
 * so there're actually 16b left for future use.
 * 
 * ע�⵽��ʾ0000-9999ֻ��Ҫ14b��0-16384��������4��B��ʵ�ϻ�ʣ��ܶ࣬��ߵ�32-14=18
 * ��λ�������ã��������λ�涨Ϊ����λ�������30b����ʾ�������������Ա�ʾ��2^30��
 * ���㹻���ˡ�
 */
typedef int32 TiDate;

/** 
 * 4B���洢��ҹ��00:00:00��֮��ĺ�����������1��������ÿ��ĺ�����86400 *1000���룬
 * ����ֻ��Ҫ27b��4���ֽڻ�ʣ��5��bitδ�ã����ǹ涨���λ��ʾ����λ��
 */
typedef uint32 TiTime;

/**
 * 2B���洢base date��2000��1��1�� 00:00:00��֮ǰ��֮���������ע�⵽��ʾ0000-9999
 * ֻ��Ҫ14b��0-16384��������4��B��ʵ�ϻ�ʣ��2b�����ǹ涨���λ��ʾ����λ��
 */
typedef uint16 TiShortDate;

/**
 * 2B���洢��ҹ��00:00:00��֮�������������һ��������ÿ��ֻ��24*60*60=86400�룬
 * ��Ҫռ��17λ��������2B��Χ�����Ժ����ᡣ��ô�죿����SQL Server��shorttimeֻ�洢
 * ��ҹ֮��ķ��������������3600���ӣ�ռ��12b����ʣ��4b�������á�
 */
typedef uint16 TiShortTime;

/** 8B����TiDate��TiTimeƴ�Ӷ��� */
typedef struct{
  TiDate date;
  TiTime time;
}TiDateTime;

/** 4B����TiShortDate��TiShortTime���϶��� */
typedef struct{
  TiShortDate date;
  TiShortTime time;
}TiShortDateTime;

TiDateTime * datetime_plus( TiDateTime * dt1, TiDateTime * dt2 );
TiDateTime * datetime_minus( TiDateTime * dt1, TiDateTime * dt2 );
TiDateTime * datetime_forward( TiDateTime * dt, uint32 interval );
TiDateTime * datetime_backward( TiDateTime * dt, uint32 interval );


/*
TiShortDateTime

TiTimeStamp = TiDateTime + ms/us/ps
��һ����ISO8601��timestamp������ǲ�һ�µģ�Ҳ��������Ҫ��һ�����õ�����

���ϳ�֮ΪStandard��ϵ�е�DateTime
char[4] TiDate
define TiDate char[4]

typedef struct{
    TiDate date;
    TiTime time;
    uint16 offset;
}TiDateTime;






Timebuf / datetime
uint8 datetime_size( TiDateTime * datetime );
void datetime_getdate( TiDateTime * datetime, TiDate * date )
TiDate * datetime_getdateptr( TiDateTime * datetime )
void datetime_gettime( TiDateTime * datetime, TiTime * time )
TiTime * datetime_gettimeptr( TiDateTime * datetime )
void datetime_getoffset( TiDateTime * datetime, char * offset, uint8 size );
char * datetime_getoffsetptr( TiDateTime * datetime )

void datetime_add( TiDateTime * datetime1, TiDateTime datetime2 )
void datetime_sub( TiDateTime * datetime1, TiDateTime datetime2 )

datetime_pack( TiDateTime * datetime, char * buf, uint8 size );
datetime_unpack( TiDateTime * datetime, char * buf, uint8 len );

uint32 datetime_day( TiDateTime * datetime );
uint32 datetime_seconds( TiDateTime * datetime );
uint32 datetime_milliseconds( TiDateTime * datetime );
uint16 datetime_microseconds( TiDateTime * datetime );
uint16 datetime_nanoseconds( TiDateTime * datetime );

uint32 datetime_to_caltime( TiDateTime * datetime, TiCalendarTime * caltime );
uint32 datetime_from_caltime( TiDateTime * datetime, TiCalendarTime * caltime );




�ڶ��ױ�ʾ����������Calendar�й�ϵ��������Ҫע��time zone��Ӱ�졣
TiCalendarTime ����������ʱ����ı�ʾ
TiCalendarTimeDuration ����ʱ�����֮��Ĳ�
TiCalendarTimeInterval������ʱ�������ɵ���ֹʱ���

UTC Time ����ʱ/�����׼ʱ��Coordinated Universal Time  
coordinated universal time: Greenwich Mean Time updated with leap seconds

ע�⣬����һ���ṹ�壬�ṹ��İڷž���ͳһ
һЩֵ����ͨ�����ʽṹ��ĳ�Ա��ã�һЩֵӦ��ͨ�������ã������ĳЩ����µ�Ч�ʣ����ǽṹ��Ķ���Ӧ����ͳһ�ģ�����ѭISO8601���ź����ǣ���Ϊ�ṹ��֮�󣬾����Ժܷ������չms/us/ns/ps��

�����״洢�����������ϴ��䣬�����С�����⣬���ǹ涨ÿ��������������ֽ��򣬲��ұ��������֮���޿հ��ֽڡ�
TiDateBuf 
TiTimeBuf
TiDateTimeBuf
TiShortDateTimeBuf

typedef char[4] TiDateBuf;
typedef char[4] TiTimeBuf;
typedef char[8] TiDateTimeBuf;
typedef char[4] TiShortDateTimeBuf;


������������չ��
����TiDateTimeBuf����
����ֽڵĴθ�λ��ʾ�Ƿ���չ�������չ��������ټ�2���ֽڱ�ʾus���Ժ����ν��С������жϴθ�λ���ж��Ƿ�����չ�ֽڡ�


��4�ױ�ʾר������Դ�߶����޵�Ƕ��ʽϵͳ������wireless sensor
��Ϊ�ڴ���ϵͳ�У����ǲ�����ȫ��Ҫȫ���ܵ�RTC�����ǲ���Ҫ������ʱ����Ļ��֡�ֻҪ��1��������ʾʱ�伴�ɣ����������̶����Ч�ʣ���Ϊ����û��ȫ���ܵ�RTC֧�֡����⣬����Ҫ֧��ms/ns��������ϵͳ�ľ���Ҫ��

�涨����һ��nB�ĳ�������ʾʱ�̣�ͨ���������������ʵ�ּӼ���ע�⵽�������������ͨ�������û��ָ���Ż�����˿�����ü�������ߵ�Ч�ʡ���Ȼ��Ҫ������һ��������ת��Ϊcalendar time����鷳�����Ǻ������ɸ���ǿ���gateway��backend serviceʵ�ֵģ�����һ��������Դ�ܽ��ŵ�MCU��ʵ�֡�

���ǿ���ֱ����TiDateTime������չ��ʽ����10���ֽ�
Ҳ������2000.01.01 00:00:00֮���ms������ʾ����������к���Ҳ����35b��5B�ڱ�ʾ��ϣ�����ͨ��timer�ж�û����ô�죬һ����ms���ж�һ�Σ����time counter�Ż����ӡ�����������û�п���us/ns/ps���⡣

��������Ƕ��ʽϵͳ��ʹ�ñ�׼��TiDateTime��ʽ�������Ҫ���������ƴ��2B us��չ�������Ҫ�������Լ���ƴns,ps,...�������ǵ�default�����У�ȱʡ�ͼ�����2B��us��չ���������������TiDateTime��ռ��10���ֽڡ�

����

����չ�ķ�ʽ���ڵ�ǰlevel���ж��Ƿ�����һ����չ
������ľ���4B date��ʽ
Ȼ�������ڶ���λ�ж��Ƿ���ms��ʾ��
msռ��4���ֽ�
���ms�ڶ���λΪ1�Ļ������ʾ��us��ʾ
usռ��2���ֽ�
Ȼ���������жϡ�
�������ǵ�Ƕ��ʽϵͳ���õ�ʱ���ʾ�ǣ�
[Date 4B]{[Time 4B]{ [ms 2B] {[us 2B] {[ns 2B] [ps 2B]}}}}
����β������Щ�����Ƿ���ڣ���Ҫ����չλ��ֵ��
�������ϴ����ʱ�򣬾��������Ӵ���ġ�
���� timebuf_size() / datetime_size() �����ֽ���

Timebuf / datetime
Uint8 datetime_size( datetime )
int32 datetime_getdate( datetime )
char * datetime_getdatebuf( datetime )
Int32 datetime_gettime( datetime )
Char * datetime_gettimebuf( datetime )
Datetime_getseconds
Get_milliseconds
Get_nanoseconds 
Datetime_pack / encode / write ���������
Datetime_unpack / decode / read

�����е�TiDateTime�ȶ�Ӧ����Ϊ���󿴴������п�����Ϊ��Ч�������õ��м������
���������ϴ���ʱ�õĶ���TiDateTimeComm�ṹ��������ʵ���Ǹ��ֽ����飬û��ʲô�ر�

ע�⣬����������У���������ν����Э��ʱ��ʱ���ĸ������ֻ���պͺ��������������һ���ͳһ��Ϊһ��8�ֽڵĺ����ʾ�������ǵ����滻�㣬��ʱ����date�����֣��Ͼ���date�����֣�����ns����Ҫ���ֵģ��ܹ���չ��������Ҫ�ġ�

�������ֱ����8�ֽڵ�ms��ʾ����������ʵ�ϣ�6���ֽ����ȫ��������ʾ���룬Ҳ���кܴ�ġ�������������2���ֽ��ٱ�ʾus��

���ָ���ɳ�֮Ϊ������ʱ SequenceTime / Local Time / Fast Time / 

Ticker ����/SliceCounter
�Ի�����Timer��װ�������ṩ��SystemTimer�ӿڣ�������ΪSystemTimerʹ��������osx�ںˡ���Ȼ��RTCҲ�ṩ��SystemTimer�ӿڣ�Ҳ������osx�ں����С�

�����һ��tick����time slice / time slot��Ϊʱ��ͬ����С��λ�����ǹ��ڴֲڣ���Ϊͨ����Ҫ��time slotҪ�����ܶ�׼��

ticker������Ӳ��Timer�����һ�������װ�������ṩ��ʱ����������е�ʵ��ʱ��
��������PC����ʱticker��ͬ���ǣ���Ҫ�ṩ��һ��tick/time slice��΢����������
һ��tick��ø�ms��һ����׼ȷ�Ļ����ϵ��������ʵ��������Ӳ�����ƾ�������������û�취��ֻ�ù�һ��ʱ������һ�¡�

TiTicker : public TiTimer
_correct_offset
.open( timeslice )
.forward( tick_count )  ����Ϊtick_count �����Ч
.backward( tick_count )
.adjust
.slicecount = value
.setvalue

.fineadjust / .finetune / .slicetune / slice

=> milliseconds
=> microseconds

TiTicker�е�������TiSvcTimer��Ϊʵ���˳�ʱ���ʱ������SvcTimer��;��λ��ͬ��

����osx��ticker������communication��timer tickerҪ���ǲ�ͬ�ģ����������һ��ʵ�֡�


*/
