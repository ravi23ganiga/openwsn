/*******************************************************************************
 * @author zhangwei, tangwen on 2006-08-02
 * Timer hardware encapsulation
 * 
 * you can reference "test/timer.*" to see how to use the TTimer object.
 * 
 * @history
 * @modified by zhangwei on 20060802
 * integrate Tangwen's new Timer code today. but not test.
 * 
 * @modified by zhangwei on 20061021
 * add channel support. 
 * testing needed.
 * 
 ******************************************************************************/

#include "hal_foundation.h"
#include "hal_timer.h"
//#include <windows.h>
//#include <mmsystem.h>

TTimer * timer_construct( TTimer * timer, uint8 size )
{
	// @TODO
	return NULL;
}

void timer_destroy( TTimer * timer )
{
}


/*
　虽然Win95下可视化开发工具如VC、Delphi、C＋＋ Builder等都有专用的定时器控件Timer，
而且使用很方便，可以实现一定的定时功能，但最小计时精度仅为55ms，且定时器消息在
多任务操作系统中的优先级很低，不能得到及时响应，往往不能满足实时控制环境下的应用。
不过Microsoft公司在Win32 API函数库中已经为用户提供了一组用于高精度计时的底层函数，
如果用户使用得当，计时精度可到1ms。这个计时精度、对于一般的实时系统控制完全可以
满足要求。现将由C＋＋ Builder 4.0提供的重新封装后的一组与时间相关的主要接口函数
（函数名、参数、功能与Win32 API基本相同）说明如下：
　　DWORD timeGetTime(void)
　　返回从Windows启动开始经过的毫秒数。最大值为232，约49.71天。
*/

uint32 timer_get_high_resolution()
{
    //return timeGetTime();
	return 0;
}
