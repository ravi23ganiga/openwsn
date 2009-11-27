http://blog.chinaunix.net/u3/94369/showart_1910373.html


ctime

TiWorldTime
TiWallTime
TiSystemTime
localtime

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