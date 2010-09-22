#ifndef _OSX_TLSCHE_H_4343_
#define _OSX_TLSCHE_H_4343_

#include "osx_configall.h"
#include "osx_foundation.h"
#include "../hal/hal_rtc.h"
#include "../hal/hal_timer.h"
#include "osx_taskpool.h"
#include "osx_taskheap.h"

#define CONFIG_OSX_TIMER_INTERVAL 500


typedef struct{
    TiOsxTaskHeap taskheap;
    TiOsxTaskPool taskpool;
    TiOsxTimer2 * timer;
}TiOsxTimeLineScheduler;

TiOsxTimeLineScheduler * osx_tlsche_open( TiOsxTimeLineScheduler * sche, TiOsxTimer2 * timer );
void osx_tlsche_close( TiOsxTimeLineScheduler * sche );
int8 osx_tlsche_taskspawn( TiOsxTimeLineScheduler * sche, TiOsxTask taskfunction, 
    void * taskdata, int16 starttime, uint8 priority, uint8 option );
void osx_tlsche_evolve( TiOsxTimeLineScheduler * sche, void * e );
void osx_tlsche_execute( TiOsxTimeLineScheduler * sche );
void osx_tlsche_kill( TiOsxTimeLineScheduler * sche, uint8 id );
void osx_tlsche_restart( TiOsxTimeLineScheduler * sche, uint8 id, int16 starttime );
#define osx_tlsche_stepbackward(sche,slicecount) osx_tlsche_stepforward((sche),(slicecount))
void osx_tlsche_stepforward( TiOsxTimeLineScheduler * sche, uint16 slicecount );

/*
Future Scheduler:

evt是对event而言，sche是对任务而言
osx_evtchecker + eventhandler
osx_evtdispatcher + default event handler

task list至少要有两种：
- 给event dispatcher用的：类似于周期性任务队列，除非用户显示的调用detach，否则一直执行，与service机制配套
- 给timeline sche用的，调度完一次，任务即结束，如果再次启动，必须再次tlsche_taskspawn/kill/postphone/restart/activate
*/

#endif /* _OSX_TLSCHE_H_4343_ */
