http://blog.chinaunix.net/u3/94369/showart_1910373.html


ctime

TiWorldTime
TiWallTime
TiSystemTime
localtime

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