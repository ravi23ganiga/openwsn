
#ifndef _ACTSCHE_H_4278_
#define _ACTSCHE_H_4278_

/*******************************************************************************
 * @author zhangwei on 2006-08-05
 * ActionScheduler(行为调度)
 * This module is based on the "hal_timer" module, and provide a flexible scheduler
 * to the application. 
 * 
 * @note
 * the key idea of ActionScheduler is timer-driven action execution. 
 * 在ActionScheduler中，调度对象是一个个Action，每个Action包括三大基本要素：
 * 		{ ID，执行时刻，执行函数，参数 }
 * 
 * 由硬件timer驱动的调度器会在所有action中搜寻应该执行的action，并负责调用之．
 * 
 * 对那些在时间上有严格要求的代码，ActionScheduler提供了一个非常好的可扩展的编程框架．它特别
 * 适合网络通信协议编程中处理延迟和要求时刻对准的情况．可以这么说，状态机处理的是事件驱动
 * event-driven编程，而ActionScheduler处理的则是time-driven编程模式．掌握ActionScheduler
 * 非常重要．
 * 
 * ActionScheduler是独立于RTOS的．不论RTOS存在与否，都可应用它进行开发．而且，与OS提供的
 * 调度不同，ActionScheduler提供的是较高精度的调度，可以达到ms级别，这在高速通信协议开发中
 * 是非常有用的
 * 
 * OOP的编程模式使得系统中可以支持多个ActionScheduler对象．但绝大多数情况下，为了节省硬件
 * 资源，我们只在系统中使用一个ActionScheduler，而让其承担多方面的action调度责任。
 * 
 * @history
 * @modified by zhangwei on 2006-08-30
 * 	revised
 * @modified by zhangwei on 2006-10-22
 * 	revised today.
 * 	act_expired() will return TRUE is the parameter "actid" doesnot found.
 * 	act_forward() will automatically cancel/delete the actid if it find the action
 * 		expired.
 * 	member variable "lastactive" remembers the last active id, thus improve the 
 * 		performance of expired().	
 * 
 ******************************************************************************/

#include "..\hal\hal_timer.h"
#include "svc_foundation.h"

#undef  ACTSCHE_SOFTDRIVE_ENABLE
#define ACTSCHE_SOFTDRIVE_ENABLE

#undef  ACTSCHE_QUERYDRIVE_ENABLE
#define ACTSCHE_QUERYDRIVE_ENABLE

#undef  ACTSCHE_HARDDRIVE_ENABLE
#define ACTSCHE_HARDDRIVE_ENABLE

#define ACTSCHE_MAX_ACTION_COUNT 	32
#define ACTSCHE_INVALID_ACTION_ID 	0xFF
#define ACTSCHE_LIST_NULL	 		ACTSCHE_INVALID_ACTION_ID


/*******************************************************************************
 * @note
 * 
 * Q: Difference between ActionScheduler and Timer object?
 * R: 如果系统很简单，只需要简单的定时判断，直接使用timer对象也是够用的，那么可以直接使用
 * timer。但是很多情况下，需要延迟的地方很多，且它们从时间上相互重叠，紧靠引入不同的timer
 * 都难以满足需要。在这种情况下，ActionScheduler提供了一种足够灵活且消耗硬件资源少（仅仅
 * 需要一个Timer硬件)的方案。在正式的系统中，建议优先使用ActionScheduler对象代替Timer。
 * 
 * Q: how to use ActionScheduler?
 * R: there're two basic programming patterns to use ActionScheduler:
 * 
 * 1) hardware timer interrupt driven
 * 硬件timer驱动，在timer interrupt service routine中执行每一个Action．由于这时每一个
 * Action都是在中断态下运行，所以要特别注意其它代码，它们很有可能会不恰当的关掉中断导致Scheduler
 * 无法执行下去．该方法比较简单，时间精度最高，但最容易出错，调试很困难．对某些中断机制过于
 * 简单的MCU而言恐难以实现. Timer只在定时时刻到的时候才触发中断，因此这种方式非常适合sleep/wakeup
 * 低功耗编程模式．
 * 
 * 2) hardware timer interrupt driven, but let the main scheduler execute them
 * 与1)不同的是，hardware timer interrupt只是设置一下请求执行标志，而真正的action执行
 * 交给更上层的应用程序主scheduler调用．主scheduler可以是RTOS中的一个死循环线程，也可以是
 * 前后台体系中的后台死循环．　这时，由于各个action的执行从中断态拿到了中断外，消除了1)中
 * 的各个缺点，但是需要增加一个调度延迟（应<1ms)
 * 
 * 3) hardware timer driven, but state query policy used
 * 仍然是硬件timer驱动，但是并不采用interrupt机制，而是让主程序去查询timer当前时间，然后
 * 调用evolve(...)函数触发action调用．
 * 
 * 4) software timer driven
 * 可以通过一个模拟的software驱动执行．
 * 是通过调用evolve(...)函数实现的．
 * 
 * Q: 如何设置每个action的执行时刻？
 * R: 表征该时刻有两种做法，一种是相对于某个固定的开始时刻，一种是相对于当前时刻．缺省情况
 * 是相对于当前时刻，也可以用begin_duration设置起始时刻，并且用end_duration结束这种设置。
 * 
 * Q: 如果在规定的时间区间内，某Action没有执行完毕会怎样？
 * R: ActionScheduler内部保证在任何一个时刻，只能有一个action在执行，不允许重入。所以上述
 * 情况发生时，ActionScheduler将继续等待该Action执行完毕而不会中途抢断，这将导致后面的Action
 * 延迟执行。Action产生的速率应能使得Scheduler有足够的时间执行完他们，若Action产生速率
 * 过高，说明系统负载太大，应设法提高性能或改进设计，此时很容易发生资源耗尽或者进入不可预料
 * 状态。这种情况要在实际中避免。
 ******************************************************************************/

struct _TiActionItem;
struct _TiActionScheduler;

typedef void (* TiEventHandler)(int e);
typedef TiEventHandler TiFunAction;

struct _TiActionItem{
  uint8 id;
  uint32 delay;
  TiFunAction function;
  void * param;
  uint8 next;
};

/* _TiActionScheduler
 *  state			each bit represent a state of the object.
 * 					bit 0: duration enable bit
 * 					bit 1: stop flag bit. this bit indicate the infinite loop 
 * 						to break by setting the flag bit.
 * 	clock			soft clock, it is also the timestamp mapping of the hardware timer.
 * 	                thus to support duration mechanism.
 * 	durationstart	the time that duration begins
 * 	callback		callback function used by the TiTimerAdapter object
 */
 
#define ACTSCHE_STATE_DURATION_FLAG 0x01
#define ACTSCHE_STATE_SOFTDRIVE_FLAG 0x02 
 
struct _TiActionScheduler{
  uint32 clock;
  uint32 durationstart;
  uint8 state;
  uint8 activelist;
  uint8 sleeplist;  
  uint8 emptylist;
  //uint8 lastactive;
  TiTimerAdapter * timer;
  TiEventHandler callback;
  struct _TiActionItem items[ACTSCHE_MAX_ACTION_COUNT];
};

typedef struct _TiActionItem TiActionItem;
typedef struct _TiActionScheduler TiActionScheduler;
  
  
TiActionScheduler * acts_construct( char * buf, uint16 size );
void acts_destroy( TiActionScheduler * sche );

void acts_configure( TiActionScheduler * sche, TiTimerAdapter * timer, TiEventHandler timerhandler );
void acts_begin_duration( TiActionScheduler * sche );
void acts_end_duration( TiActionScheduler * sche );
int8 acts_inputaction( TiActionScheduler * sche, TiFunAction action, void * param, uint32 delay );
int8 acts_actionprocess( TiActionScheduler * sche );
uint32 acts_getnextinterval( TiActionScheduler * sche );
int8 acts_expired( TiActionScheduler * sche, uint8 actid );
void acts_cancelaction( TiActionScheduler * sche, uint8 actid );
void acts_forward( TiActionScheduler * sche, uint32 steps );
int8 acts_evolve( TiActionScheduler * sche, uint32 steps );

#ifdef ACTSCHE_SOFTDRIVE_ENABLE
int8 acts_softdrive_start( TiActionScheduler * sche, uint32 period );
int8 acts_softdrive_stop( TiActionScheduler * sche );
#endif
#ifdef ACTSCHE_QUERYDRIVE_ENABLE
int8 acts_querydrive_start( TiActionScheduler * sche, uint32 period );
int8 acts_querydrive_stop( TiActionScheduler * sche );
#endif
#ifdef ACTSCHE_HARDDRIVE_ENABLE
int8 acts_harddrive_start( TiActionScheduler * sche );
int8 acts_harddrive_stop( TiActionScheduler * sche );
int8 acts_harddrive_default_timer_callback( void * param );
#endif

#endif /* _ACTSCHE_H_4278_ */
