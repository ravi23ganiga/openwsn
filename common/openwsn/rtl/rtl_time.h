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
 * time library
 * This library is similar to the ANSI C time library. It supports two three time
 * types: 
 *   - TiTime, which is an unsigned long int
 *   - TiWallTime, which is an structure 
 *   - TiTimeBuffer/TiTimeStamp, which is an time buffer used for storage and transportation.
 * 
 * The difference between this module and ANSI C time library is: this one support
 * high percision storage and processing even to 1 milli-seconds, and it can also 
 * extend to us.
 */
typedef uint8  TiTime8;  TiSystemTime8  epoch tick  tm8  rtl_systime  rtl_caltime  rtl  sysclock
typedef uint16 TiTime16;  TiSystemTime32 
typedef uint32 TiTime32;
typedef uint64 TiTime64;

/** By default, the system time type is define as time64 */
#define TiSystemTime TiTime64

/*******************************************************************************
 * TiTime8 operations
 ******************************************************************************/

inline void tm8_reset( TiTime8 * var, uint8 value ) {*var=value;}
inline void tm8_clear( TiTime8 * var ) {*var=0;}

inline TiTime8 * tm8_plus( TiTime8 * var1, TiTime8 * var2 )
{
	(*var1) += (*var2);
	return var1;
}

inline TiTime8 * tm8_minus( TiTime8 * var1, TiTime8 * var2 ) 
{
	(*var1) -= (*var2);
	return var1;
}

inline bool tm8_forward( TiTime8 * var, uint8 interval )
{
	// return false to indicate overflow during forward
	if (*var + interval < *var)
		return false;
	else
		return true;
}

inline bool tm8_backward( TiTime8 * var, uint8 interval )
{
	// return false to indicate overflow during forward
	if (*var - interval > *var)
		return false;
	else
		return true;
}

/*******************************************************************************
 * TiTime64 operations
 ******************************************************************************/

inline void tm64_reset( TiTime64 * var, uint64 value ) {*var=value;}
inline void tm64_clear( TiTime64 * var ) {*var=0;}

inline TiTime64 * tm8_plus( TiTime64 * var1, TiTime64 * var2 )
{
	(*var1) += (*var2);
	return var1;
}

inline TiTime64 * tm64_minus( TiTime64 * var1, TiTime64 * var2 ) 
{
	(*var1) -= (*var2);
	return var1;
}

inline bool tm64_forward( TiTime64 * var, uint64 interval )
{
	// return false to indicate overflow during forward
	if (*var + interval < *var)
		return false;
	else
		return true;
}

inline bool tm64_backward( TiTime64 * var, uint64 interval )
{
	// return false to indicate overflow during forward
	if (*var - interval > *var)
		return false;
	else
		return true;
}

/**
 * TiCalendarTime / TiCalTime
 *
 * @reference
 *  SYSTEMTIME type in windows API
 */ 
typedef struct{
  uint16 year;
  uint8  month;
  int8 mday;
  int8 wday;
  int8 yday;
  int8  day;
  uint8  hour;
  uint8  min;
  uint8  sec;
  uint16 msec;
  uint16 usec;
}TiCalTime; 

caltime_clear( TiCalTime * caltime );
caltime_parse( TiCalTime * caltime, TiSystemTime * tm )
caltime_assemble( TiCalTime * caltime, TiSystemTime * tm )
caltime_plus( TiCalTime * t1, TiCalTime * t2 )
caltime_minus( TiCalTime * t1, TiCalTime * t2 )
caltime_forward_sec( TiCalTime * caltime, uint16 sec )
caltime_backward_sec( TiCalTime * caltime, uint16 sec )
caltime_forward_msec( TiCalTime * caltime, uint16 msec )
caltime_backward_msec( TiCalTime * caltime, uint16 msec )
caltime_compare( TiCalTime * t1, TiCalTime * t2 )
caltime_lessthan( TiCalTime * t1, TiCalTime * t2 )
caltime_greaterthan( TiCalTime * t1, TiCalTime * t2 )
caltime_lessthanequal( TiCalTime * t1, TiCalTime * t2 )
caltime_greaterthanequal( TiCalTime * t1, TiCalTime * t2 )

/* TiCalTime
 * 
 * 10 byte representation of accurate timestamp
 * [year 2B][month 1B][day 1B][hour 1B][min 1B][sec 1B][msec 2B][usec 2B]
 * 
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
caltime_pack( TiCalTime * t, char * buf, uint8 size=10 ); 
caltime_unpack( TiCalTime * t, char * buf, uint8 size=10 ); 



/*
void settimezone(int8 offset);
get_worldtime( TiWallTime * tm );
get_walltime( TiWallTime * tm );

walltime_current  wallclock
walltime_reset
walltime_forward
walltime_backward
walltime_timezone
walltime_set_timezone
walltime_differ

typedef struct{
  uint16 year;
  uint8  month;
  uint8  day;
  uint8  hour;
  uint8  min;
  uint8  sec;
      //int tm_mday;
      //int tm_wday;
      //int tm_yday;
  uint16 msec;
}TiCalTime; TiWallTime  TiClockTime


systime_current
systime_reset
systime_forward
systime_backward
systime_diff

walltime * gmtime( TiTime, walltime )

clocktime







#define TiTime TiTime64

typedef uint64 TiTime64

TiSystemTime

rtl_systime.h
rtl_time
rtl_rtctime

TiDateTime

syncwith( TiTime64 * tm, uint16 ratio, uint16 drift )

TiTime64









time.h��������,2009, http://blog.chinaunix.net/u3/94369/showart_1910373.html


hardware time   TiDeviceTime
hardware indepent time   TiSystemTime ����TiTme64���� ��device time��systime֮��Ҫ�����ܱ任
calendar time (hardware independent)  TiWallTime�� there're two variables, g_localtime, g_gmtime, 
	һ������Եģ�һ���Ǿ��Ե� used by rtc
timebuf for transmission TiTimeBuf (binary format, or ascii format, and can be extendable, from ns to even millions years)
����һ����osר�õ�ticker, ���ǵ�real time���ԣ�tickerӦ�ô�systime�в��������Ҹ�����ȷ��drift


Timer: hardware driven without interrupt because interrupt can be disabled
or software interrupt drive (time drift after a long time)


#ifdef CONFIG_PLATFORM16
#define TiTime TiTime16
#else
typedef TiTime TiTime32
#endif

#define TiTickCounter TiTickTime
#define TiSystemTime TiTickTimer

typedef struct{
    uint16 w3; 
    uint16 w2; 
    uint16 w1; 
    uint16 w0; 
}TiTickTime; TiDeviceTime  TiTime64

#define TiTimeBuffer TiTimeStamp
typedef unsigned char[10] TiTimeStamp;



ctime

TiWorldTime (calendar time, unique to greenwich time zone)
TiWallTime (wall clock time = calendar time)
TiSystemTime (64bit integer time)
TiCalTime
localtime

typedef struct _SYSTEMTIME { // st  
  WORD wYear;  
  WORD wMonth;  
  WORD wDayOfWeek;  
  WORD wDay;  
  WORD wHour;  
  WORD wMinute;  
  WORD wSecond;  
  WORD wMilliseconds;  
} SYSTEMTIME; 

time.h


@��������:     localtime
����ԭ��:     struct tm *localtime(const time_t *timer)
��������:     ����һ����tm�ṹ���Ļ���ʱ����Ϣ
��������:     ��tm�ṹ����ʱ�䣬�ṹtm��������:
struct  tm{
      int tm_sec;
      int tm_min;
      int tm_hour;
      int tm_mday;
      int tm_mon;
      int tm_year;
      int tm_wday;
      int tm_yday;
      int tm_isdst;
    };
����˵��:     timer-ʹ��time()������õĻ���ʱ��
�����ļ�:     <time.h>

#include <time.h>
#include <stdio.h>
#include <dos.h>
int main()
{
    time_t timer;
    struct tm *tblock;
    timer=time(NULL);
    tblock=localtime(&timer);
    printf("Local time is: %s",asctime(tblock));
    return 0;
}


@��������:     asctime
����ԭ��:     char* asctime(struct tm * ptr)
��������:     �õ�����ʱ��(����ʱ��ת��ΪASCII��)
��������:     ���ص�ʱ���ַ�����ʽΪ������,��,��,Сʱ���֣���,��
����˵��:     �ṹָ��ptrӦͨ������localtime()��gmtime()�õ�
�����ļ�:     <time.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
int main()
{
    struct tm t;
    char str[80];
    t.tm_sec=1;
    t.tm_min=3;
    t.tm_hour=7;
    t.tm_mday=22;
    t.tm_mon=11;
    t.tm_year=56;
    t.tm_wday=4;
    t.tm_yday=0;
    t.tm_isdst=0;
    strcpy(str,asctime(&t));
    printf("%s",str);
    return 0;
}


@��������:     ctime
����ԭ��:     char *ctime(long time)
��������:     �õ�����ʱ��
��������:     �����ַ�����ʽ������,��,��,Сʱ:��:��,��
����˵��:     time-�ò���Ӧ�ɺ���time���
�����ļ�:     <time.h>

#include <stdio.h>
#include <time.h>
int main()
{
    time_t t;
    time(&t);
    printf("Today's date and time: %s",ctime(&t));
    return 0;
}


@��������:     difftime
����ԭ��:     double difftime(time_t time2, time_t time1)
��������:     �õ����λ���ʱ����λΪ��
��������:     ʱ����λΪ��
����˵��:     time1-����ʱ��һ,time2-����ʱ���.�ò���Ӧʹ��time�������
�����ļ�:     <time.h>

#include <time.h>
#include <stdio.h>
#include <dos.h>
#include <conio.h>
int main()
{
    time_t first, second;
    clrscr();
    first=time(NULL);
    delay(2000);
    second=time(NULL);
    printf("The difference is: %f seconds",difftime(second,first));
    getch();
    return 0;
}


@��������:     gmtime
����ԭ��:     struct tm *gmtime(time_t  *time)
��������:     �õ��Խṹtm��ʾ��ʱ����Ϣ
��������:     �Խṹtm��ʾ��ʱ����Ϣָ��
����˵��:     time-�ú���time()�õ���ʱ����Ϣ
�����ļ�:     <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dos.h>
char *tzstr="TZ=PST8PDT";
int main()
{
    time_t t;
    struct tm *gmt, *area;
    putenv(tzstr);
    tzset();
    t=time(NULL);
    area=localtime(&t);
    printf("Local time is:%s", asctime(area));
    gmt=gmtime(&t);
    printf("GMT is:%s", asctime(gmt));
    return 0;
}


@��������:     time
����ԭ��:     time_t time(time_t *timer)
��������:     �õ�����������ʱ�������������ʱ��
��������:     ��������ʱ��
����˵��:     timer=NULLʱ�õ���������ʱ�䣬timer=ʱ����ֵʱ��������������ʱ�䣬time_t��һ��long����
�����ļ�:     <time.h>

#include <time.h>
#include <stdio.h>
#include <dos.h>
int main()
{
    time_t t;
    t=time();
    printf("The number of seconds since January 1,1970 is %ld",t);
    return 0;
}


@��������:     tzset
����ԭ��:     void tzset(void)
��������:     UNIX���ݺ��������ڵõ�ʱ������DOS����������;
��������:
����˵��:
�����ļ�:     <time.h>

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
int main()
{
    time_t td;
    putenv("TZ=PST8PDT");
    tzset();
    time(&td);
    printf("Current time=%s",asctime(localtime(&td)));
    return 0;
}

*/