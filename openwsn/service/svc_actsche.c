/*******************************************************************************
 * @author zhangwei on 2006-08-05
 * ActionScheduler(��Ϊ����)
 * This module is based on the "hal_timer" module, and provide a flexible scheduler
 * to the application. 
 * 
 * @note
 * the key idea of ActionScheduler is timer-driven action execution. 
 * ��ActionScheduler�У����ȶ�����һ����Action��ÿ��Action�����������Ҫ�أ�
 * 		{ ID��ִ��ʱ�̣�ִ�к��������� }
 ******************************************************************************/
 
#include "..\foundation.h"
#include "..\hal\hal_foundation.h"
#include "..\hal\hal_cpu.h"
#include "..\hal\hal_interrupt.h"
#include "..\hal\hal_assert.h"
#include "svc_actsche.h"


static uint8 _acts_allocateitem( TiActionScheduler * sche );
static void  _acts_disposeitem( TiActionScheduler * sche, uint8 id );
static uint8 _acts_insertaction( TiActionScheduler * sche, uint8 id, TiFunAction action, 
	void * param, uint32 delay );

//------------------------------------------------------------------------------
// construct an ActionScheduler object in the memory. This object should always 
// be created successfully except no enough memory. the scheduler is stoped by 
// default.
//------------------------------------------------------------------------------
TiActionScheduler * acts_construct( char * buf, uint16 size )
{
	TiActionScheduler * sche;
	uint8 n;
	
	if (sizeof(TiActionScheduler) > size)
		sche = NULL;
	else
		sche = (TiActionScheduler *)buf;
		
	if (sche != NULL)
	{
		memset( buf, 0x00, size );
		sche->clock = 0;
		sche->durationstart = 0;
		sche->state = 0x00;
		sche->activelist = ACTSCHE_LIST_NULL;
		sche->sleeplist = ACTSCHE_LIST_NULL;
		sche->emptylist = 0;
		for (n=0; n<ACTSCHE_MAX_ACTION_COUNT-1; n++)
		{
			sche->items[n].next = n+1; 
		}
		sche->items[ACTSCHE_MAX_ACTION_COUNT-1].next = ACTSCHE_LIST_NULL; 
		//sche->lastactive = ACTSCHE_INVALID_ACTION_ID;
		sche->timer = NULL;
		sche->callback = NULL;
	}
	
	return sche;
}

//------------------------------------------------------------------------------
// If an callback function has been assigned to the "timer" object, then detach
// it from the timer. 
//------------------------------------------------------------------------------
void acts_destroy( TiActionScheduler * sche )
{
	if ((sche->timer != NULL) && (sche->callback != NULL))
	{
		timer_disable( sche->timer );
		timer_configure( sche->timer,  NULL, NULL, 0 );
	}
	NULL;
}
 
// @attention
// @TODO
//	the prioroty in timer_configure(..) should be HIGH! you may need to change it
// according your hal_timer settings!
// 
// @param
//	timerhandler 	a callback function used by the TiTimerAdapter object
//					this param can be NULL, which will disable the Timer interrupt.
//	
void acts_configure( TiActionScheduler * sche, TiTimerAdapter * timer, TiEventHandler timerhandler )
{
	sche->timer = timer;
	sche->callback = timerhandler;
	
	if (timer != NULL)
	{
		timer_configure( timer, timerhandler, sche, 0 );
	}
}

void acts_begin_duration( TiActionScheduler * sche )
{	
	if (sche->timer != NULL)
	{
		sche->clock = timer_getvalue( sche->timer );
	}
	
	sche->durationstart = sche->clock;
	sche->state |= ACTSCHE_STATE_DURATION_FLAG;
}

void acts_end_duration( TiActionScheduler * sche )
{
	sche->state &= (~ACTSCHE_STATE_DURATION_FLAG);
}
 
//------------------------------------------------------------------------------
// This function simply put the action into the scheduler's internal list
// the scheduler will execute this action later. 
// this feature enables the safe calling of this function from interrupt 
// service routines.
//
// @return
//	>=0		success
//	-1		failed. however, this had better not happen. or else you may lost 
//			actions.
//
//------------------------------------------------------------------------------
int8 acts_inputaction( TiActionScheduler * sche, TiFunAction action, void * param, uint32 delay )
{
	uint8 id;
	int8 ret = 0;
	
	hal_enter_critical();
	id = _acts_allocateitem( sche );
	if (id != ACTSCHE_LIST_NULL)
		ret = (_acts_insertaction( sche, id, action, param, delay ) != ACTSCHE_LIST_NULL) ? 0 : -1;
	else
		ret = -1;
	hal_leave_critical();
	
	return ret;
}

//------------------------------------------------------------------------------
// execute all the actions in the active list. If someone action failed, then it
// return a negtive value. all the items after execution will be returned back 
// to the empty list. 
//
// @return 
//	0		all action success
//	negtive	some action failed. the value is the order index of the first failed
//			action in the list.
//
//------------------------------------------------------------------------------
int8 acts_actionprocess( TiActionScheduler * sche )
{
	TiActionItem * pitem;
	boolean failed=FALSE;
	uint8 id, bak;
	int8 ret=0;

	id = sche->activelist;
	while (id != ACTSCHE_LIST_NULL)
	{
		pitem = &( sche->items[id] );
		if (pitem->function != NULL)
			if (pitem->function( pitem->param ) < 0)
				failed = TRUE;
		if (!failed)
			ret ++;
		
		#ifndef ACTSCHE_HARDDRIVE_ENABLE
		hal_enter_critical();
		#endif
		
		bak = id;
		id = pitem->next;
		_acts_disposeitem( sche, bak );
		
		#ifndef ACTSCHE_HARDDRIVE_ENABLE
		hal_leave_critical();
		#endif
	}
	assert( sche->activelist == id );
	sche->activelist = ACTSCHE_LIST_NULL; 

	return failed ? ret : 0;
}

//------------------------------------------------------------------------------
// @return
//	the time interval between now and the next action to be processed.
// 	if there are no action actions, then return 0xFFFFFFFF (for 32bit systems)
//------------------------------------------------------------------------------
uint32 acts_getnextinterval( TiActionScheduler * sche )
{
	uint32 ret = ~0;
	
	if (sche->activelist != ACTSCHE_LIST_NULL)
		ret = sche->items[ sche->activelist ].delay;
	else
		ret = (~0);
		
	return ret;
}

//------------------------------------------------------------------------------
// judge whether a specific action is expired or not. an expired action should 
// be processed. this judgement is useful for those modules who only want an 
// expired signal.  If the action is expired, then it will be automatically 
// deleted from the ActionScheduler by this function.
//
// This function is quite similar to the Timer object's expired function. but 
// the timer object can only hold one action, but the ActionScheduler can hold 
// multiple actions at the same time.
//
// @modified by zhangwei on 20061022
// @attention
//	Important!
//	if the action "actid" doesn't found in ActionScheduler, then this function 
// will return TRUE too. the reason is as the following:
//	after the developer call inputaction(), you SHOULD NOT asked him to cancel 
// the action manually except he want to stop it before it expired. so this lead 
// to a great danger that the resource will not free if the developer forgot to 
// do it. 
//	how to deal with this phenomena? my solution is to cancel the action automatically
// when it is expired. this is done in evolve() or forward() function. so the 
// ActionScheduler can free the resource allocated to that action even when the 
// developer forget or failed to cancel it manually.
// 
//------------------------------------------------------------------------------
int8 acts_expired( TiActionScheduler * sche, uint8 actid )
{
	uint32 steps;
	uint8 prev, id;
	boolean found;
	
	steps = timer_elapsed( sche->timer ); // �������ϴε���elapse()������ʱ��
	acts_forward( sche, steps );
	
	id = sche->activelist;
	prev = ACTSCHE_LIST_NULL;
	found = FALSE;
	while (id != ACTSCHE_LIST_NULL)
	{
		if (id == actid)
		{
			found = TRUE;
			break;
		}
		
		prev = id;
		id = sche->items[id].next;
	}
	
	// release the item resources from activelist.
	if (found)
	{
		if (prev == ACTSCHE_LIST_NULL)
			sche->activelist = sche->items[id].next;
		else
			sche->items[prev].next = sche->items[id].next;
			
		_acts_disposeitem( sche, id );
		found = !found;
	}
	else{
		// query the action in sleep list
		id = sche->sleeplist;
		prev = ACTSCHE_LIST_NULL;
		found = FALSE;
		while (id != ACTSCHE_LIST_NULL)
		{
			if (id == actid)
			{
				found = TRUE;
				break;
			}
			
			prev = id;
			id = sche->items[id].next;
		}
	}
		
	return (!found);
}

//------------------------------------------------------------------------------
// cancel action. it will delete an action from the ActionScheduler, no matter
// this action is expired or not.
// this is the reverse of inputaction.
//------------------------------------------------------------------------------
void acts_cancelaction( TiActionScheduler * sche, uint8 actid )
{
	uint8 prev, id;
	boolean found;
	
	id = sche->activelist;
	prev = ACTSCHE_LIST_NULL;
	found = FALSE;
	hal_enter_critical();
	while (id != ACTSCHE_LIST_NULL)
	{
		if (id == actid)
		{
			found = TRUE;
			break;
		}
		
		prev = id;
		id = sche->items[id].next;
	}
	if (found)
	{
		if (prev == ACTSCHE_LIST_NULL)
			sche->activelist = sche->items[id].next;
		else
			sche->items[prev].next = sche->items[id].next;
			
		_acts_disposeitem( sche, id );
	}
	else{
		id = sche->sleeplist;
		prev = ACTSCHE_LIST_NULL;
		found = FALSE;
		while (id != ACTSCHE_LIST_NULL)
		{
			if (id == actid)
			{
				found = TRUE;
				break;
			}
			
			prev = id;
			id = sche->items[id].next;
		}
		if (found)
		{
			if (prev == ACTSCHE_LIST_NULL)
				sche->sleeplist = sche->items[id].next;
			else
				sche->items[prev].next = sche->items[id].next;
				
			_acts_disposeitem( sche, id );
		}
	}
	hal_leave_critical();
		
	return;
}

// let the clock forward "steps". 
// this function will place all the action into activelist and wait actionprocess()
// to process them.
//
void acts_forward( TiActionScheduler * sche, uint32 steps )
{
	uint8 id, tail;
	
	if (steps == 0)
		return;
		
	id = sche->sleeplist;
	
	// substract the delay time of every sleep action
	//
	while (id != ACTSCHE_LIST_NULL)
	{
		// @modified by zhangwei on 200606
		// @attention 
		// the following style is wrong, because it will cause overflow. 
		//  
		// sche->items[id].delay -= steps;
		// if (sche->items[id].delay <= 0)
		// {
		//  	do some thing here
		// }
		// 
		// the correct source code is as the following:

		if (sche->items[id].delay > steps)
			sche->items[id].delay -= steps;
		else
			sche->items[id].delay = 0;
		
		id = sche->items[id].next;
	}
	
	// find the tail item of the active list.
	// usually, the active list is NULL. but we'd better not assume it always true.
	//
	tail = ACTSCHE_LIST_NULL;
	id = sche->activelist;
	while (id != ACTSCHE_LIST_NULL)
	{
		tail = id;
		id = sche->items[id].next;
	}
	
	// move the actions that delay == 0 from the sleep list to active list
	//
	id = sche->sleeplist;
	if (sche->items[id].delay == 0)
	{
		#ifndef ACTSCHE_HARDDRIVE_ENABLE
		hal_enter_critical();
		#endif
		
		if (tail == ACTSCHE_LIST_NULL)
		{
			sche->activelist = id;
		}
		else{
			sche->items[tail].next = id;
		} 
		tail = id;
		
		id = sche->items[id].next;
		while (sche->items[id].delay == 0)
		{
			sche->items[tail].next = id;
			tail = id;
			id = sche->items[id].next;			
		}
		sche->items[tail].next = ACTSCHE_LIST_NULL;
		
		#ifndef ACTSCHE_HARDDRIVE_ENABLE
		hal_leave_critical();
		#endif
	}
	
	// @attention: the following code may overflow. but this has not serious result
	// the execution result will still be right.
	sche->clock += steps;
	
	return;
}

//------------------------------------------------------------------------------
// ��ʱ����ǰ�ƽ�stepsʱ�䲽��ÿһ������sleep״̬��action��delay��Ҫ��ȥsteps��
// �����action��delay <= 0����˵����actionӦ����ִ�С������actionlistִ��֮��
//
// ���������̵�ʱ�򣬾����ڵ���������������hal_sleep()ʹϵͳ����sleep״̬����
// ����һ��timer��ʱʱ�䵽�������´������ٴε���evolve()����ִ�С�
//
// while (TRUE)
// {
//   acts_evolve(...);
//   hal_sleep();
// }
//
// �ú����������Timer�ж����ʹ�ã�Ч�����
// timer_interupt()
// {
//   hal_cpu_wakeup();
//   steps = timer_elapsed(); // �������ϴε���elapse()������ʱ��
//   acts_evolve( g_actsche, steps );
// }
//
// @return 
//	the same as acts_actionprocess(...)
//------------------------------------------------------------------------------
int8 acts_evolve( TiActionScheduler * sche, uint32 steps )
{
	acts_forward( sche, steps );
	return acts_actionprocess( sche );
}

//------------------------------------------------------------------------------
// An soft clock simulation to drive the scheduler run. An infinite soft loop
// is used to simulate the hardware's clock to trigger the scheduler's evolve()
// function.
//------------------------------------------------------------------------------
int8 acts_softdrive_start( TiActionScheduler * sche, uint32 period )
{
	sche->state &= (~ACTSCHE_STATE_SOFTDRIVE_FLAG);
	while (TRUE)
	{
		if (sche->state & ACTSCHE_STATE_SOFTDRIVE_FLAG)
			break;
		
		// @attention: the following code will be overflow. but it seems has not
		// bad effects on our program, so let it go.
		//
		sche->clock += period;

		acts_evolve( sche, period );
	}
	sche->state &= (~ACTSCHE_STATE_SOFTDRIVE_FLAG);
	
	return 0;
}

#ifdef ACTSCHE_SOFTDRIVE_ENABLE
//------------------------------------------------------------------------------ 
// set the STOP flag bit of the scheduler's state to indicate the scheduler's
// loop stop.
//------------------------------------------------------------------------------ 
int8 acts_softdrive_stop( TiActionScheduler * sche )
{
	sche->state |= ACTSCHE_STATE_SOFTDRIVE_FLAG;
	return 0;
}
#endif

#ifdef ACTSCHE_SOFTDRIVE_ENABLE
int8 acts_querydrive_start( TiActionScheduler * sche, uint32 period )
{
	TiTimerAdapter * tm = sche->timer;
	uint32 interval;

	assert( tm != NULL );
	timer_stop( tm );
	timer_disable( tm );
	timer_restart( tm, period, TRUE );
		
	while (TRUE)
	{
		if (sche->state & ACTSCHE_STATE_SOFTDRIVE_FLAG)
			break;

		// @attention			
		// if RTOS introudced, then you can give up the CPU and let the CPU 
		// do other things in the following.
		// hal_wait() won't release the CPU for others.
		
		if (timer_expired( tm ))
		{
			interval = timer_elapsed( tm );
			acts_evolve( sche, interval );
			sche->clock += interval; // overflow; how to deal with it?
		}
		else{
			hal_delay( period );
		}
	}
	
	return 0;
}
#endif

#ifdef ACTSCHE_QUERYDRIVE_ENABLE
int8 acts_querydrive_stop( TiActionScheduler * sche )
{
	sche->state |= ACTSCHE_STATE_SOFTDRIVE_FLAG;
	return 0;
}
#endif

#ifdef ACTSCHE_QUERYDRIVE_ENABLE
// Hard Drive mode is essentially drived by timer interrupts. 
int8 acts_harddrive_start( TiActionScheduler * sche )
{
	TiTimerAdapter * tm = sche->timer;

	assert( tm != NULL );
	timer_stop( tm );
	timer_disable( tm );
	timer_configure( tm, acts_harddrive_default_timer_callback, (void *)sche, 0 );
	timer_restart( tm, acts_getnextinterval(sche), FALSE );
	return 0;
}
#endif

#ifdef ACTSCHE_QUERYDRIVE_ENABLE
int8 acts_harddrive_stop( TiActionScheduler * sche )
{
	timer_stop( sche->timer );
	timer_configure( sche->timer, NULL, NULL, 0 );
	sche->state |= ACTSCHE_STATE_SOFTDRIVE_FLAG;
	return 0;
}
#endif

#ifdef ACTSCHE_HARDDRIVE_ENABLE
//------------------------------------------------------------------------------ 
// if you DON'T want to introduce a infinite loop in your application and to 
// to call acts_evolve() frequently, you can let the "TiTimerAdapter" hardware object 
// do it for you. In this case you should configure the "ActionScheduler" object 
// with this timer's callback function. 
//------------------------------------------------------------------------------ 
int8 acts_harddrive_default_timer_callback( void * param )
{
	TiActionScheduler * sche;
	int8 ret;
	
	sche = (TiActionScheduler *)param;
	ret = acts_evolve( sche, timer_elapsed(sche->timer) );
	timer_restart( sche->timer,acts_getnextinterval(sche), 0 );
	return 0;
}
#endif

#ifdef ACTSCHE_HARDDRIVE_ENABLE
//------------------------------------------------------------------------------
// allocate an item from the available list
//
// @warning
//	This function should never return ACTSCHE_LIST_NULL when the application is 
// running! because this often means lost of actions for the scheduler!!!
//
// @return 
//	the id of item. which is essentially the item index in the item array.
//  return ACTSCHE_LIST_NULL when there's no free item to be allocated. 
//------------------------------------------------------------------------------
uint8 _acts_allocateitem( TiActionScheduler * sche )
{
	uint8 id;
	
	id = sche->emptylist;
	if (id != ACTSCHE_LIST_NULL)
	{
		sche->emptylist = sche->items[id].next;
	}

	return id; 
}
#endif

#ifdef ACTSCHE_HARDDRIVE_ENABLE
//------------------------------------------------------------------------------
// return the item into the empty list
// @return (null)
//------------------------------------------------------------------------------
void _acts_disposeitem( TiActionScheduler * sche, uint8 id )
{
	if (id != ACTSCHE_LIST_NULL)
	{
		sche->items[id].next = sche->emptylist;
		sche->emptylist = id;
	}
}
#endif

//------------------------------------------------------------------------------
// ������Ϊid��item���뵽activelist��sleeplist��ȥ
// �����ǰaction��delay == 0, Ҳ���ǲ���Ҫ�ӳپ�Ӧ����ִ�У�����뵽activelist����ĩβ��
// ������FIFO����ԭ��
// �����ǰaction��delay > 0, Ҳ����Ҫ�ӳ�һ��ʱ���Ż�ִ�У�����뵽sleeplist�����С�
// ע������ʱ��sleeplist�Ӷ�ͷ����β�ǰ���delay��С����˳�����еġ�
// 
// @param
//	id		the action item index. it must be a valid id, or else the system
//			will encounter memory access violation.
//
// @return 
//	0		success
//	ACTSCHE_LIST_NULL when failed
//------------------------------------------------------------------------------
uint8 _acts_insertaction( TiActionScheduler * sche, uint8 id, TiFunAction action, void * param, uint32 delay )
{
	uint8 prev, cur;
	uint32 curtime, elapsed_time;
	uint8 ret;

	if ((id == ACTSCHE_LIST_NULL) || (id >= ACTSCHE_LIST_NULL))
	{
		ret = ACTSCHE_LIST_NULL;
	}
	else{
		// @warning
		// 	attention you should have different calculating method to estimate 
		// the delay in action item.  This is different when dealing with 
		// durationi enable and duration disable cases. Take careful about 
		// the caculation overflow!
		//
		sche->items[id].function = action;
		sche->items[id].param = param;
		if (sche->state & ACTSCHE_STATE_DURATION_FLAG)
		{
			curtime = timer_getvalue( sche->timer );
			if (curtime >= sche->durationstart)
				elapsed_time = curtime - sche->durationstart;
			else
				elapsed_time = (~0) - sche->durationstart + curtime;
				
			assert( delay >= elapsed_time );
			delay = ((delay > elapsed_time) ? (delay - elapsed_time) : 0);
			sche->items[id].delay =  delay;
		}
		else{
			sche->items[id].delay = delay;
		}
		
		// search for the correct position to insert this item
		if (delay == 0)
		{
			prev = ACTSCHE_LIST_NULL;
			cur = sche->activelist;
			while (cur != ACTSCHE_LIST_NULL)
			{
				prev = cur;
				cur = sche->items[cur].next;
			}
		
			sche->items[id].next = ACTSCHE_LIST_NULL;
			if (prev == ACTSCHE_LIST_NULL)
				sche->activelist = id;
			else
				sche->items[prev].next = id;
		}
		else{
			prev = ACTSCHE_LIST_NULL;
			cur = sche->sleeplist;
			while (cur != ACTSCHE_LIST_NULL)
			{
				if (sche->items[cur].delay > delay)
					break;
				prev = cur;
				cur = sche->items[cur].next;
			}
		
			sche->items[id].next = cur;
			if (prev == ACTSCHE_LIST_NULL)
				sche->sleeplist = id;
			else
				sche->items[prev].next = id;
		}
		ret = 0;
	}
	
	return ret;
}
