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
http://blog.chinaunix.net/u3/94369/showart_1910373.html

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
typedef uint8  TiTime8;
typedef uint16 TiTime16;
typedef uint32 TiTime32;
typedef uint64 TiTime64;


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
walltime_compare
walltime_lessthan
walltime_greaterthan
walltime_lessthanequal

systime_current
systime_reset
systime_forward
systime_backward
systime_diff

walltime * gmtime( TiTime, walltime )

clocktime




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


@函数名称:     localtime
函数原型:     struct tm *localtime(const time_t *timer)
函数功能:     返回一个以tm结构表达的机器时间信息
函数返回:     以tm结构表达的时间，结构tm定义如下:
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
参数说明:     timer-使用time()函数获得的机器时间
所属文件:     <time.h>

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


@函数名称:     asctime
函数原型:     char* asctime(struct tm * ptr)
函数功能:     得到机器时间(日期时间转换为ASCII码)
函数返回:     返回的时间字符串格式为：星期,月,日,小时：分：秒,年
参数说明:     结构指针ptr应通过函数localtime()和gmtime()得到
所属文件:     <time.h>

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


@函数名称:     ctime
函数原型:     char *ctime(long time)
函数功能:     得到日历时间
函数返回:     返回字符串格式：星期,月,日,小时:分:秒,年
参数说明:     time-该参数应由函数time获得
所属文件:     <time.h>

#include <stdio.h>
#include <time.h>
int main()
{
    time_t t;
    time(&t);
    printf("Today's date and time: %s",ctime(&t));
    return 0;
}


@函数名称:     difftime
函数原型:     double difftime(time_t time2, time_t time1)
函数功能:     得到两次机器时间差，单位为秒
函数返回:     时间差，单位为秒
参数说明:     time1-机器时间一,time2-机器时间二.该参数应使用time函数获得
所属文件:     <time.h>

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


@函数名称:     gmtime
函数原型:     struct tm *gmtime(time_t  *time)
函数功能:     得到以结构tm表示的时间信息
函数返回:     以结构tm表示的时间信息指针
参数说明:     time-用函数time()得到的时间信息
所属文件:     <time.h>

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


@函数名称:     time
函数原型:     time_t time(time_t *timer)
函数功能:     得到机器的日历时间或者设置日历时间
函数返回:     机器日历时间
参数说明:     timer=NULL时得到机器日历时间，timer=时间数值时，用于设置日历时间，time_t是一个long类型
所属文件:     <time.h>

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


@函数名称:     tzset
函数原型:     void tzset(void)
函数功能:     UNIX兼容函数，用于得到时区，在DOS环境下无用途
函数返回:
参数说明:
所属文件:     <time.h>

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