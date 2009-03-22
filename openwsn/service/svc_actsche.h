
#ifndef _ACTSCHE_H_4278_
#define _ACTSCHE_H_4278_

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
 * 
 * ��Ӳ��timer�����ĵ�������������action����ѰӦ��ִ�е�action�����������֮��
 * 
 * ����Щ��ʱ�������ϸ�Ҫ��Ĵ��룬ActionScheduler�ṩ��һ���ǳ��õĿ���չ�ı�̿�ܣ����ر�
 * �ʺ�����ͨ��Э�����д����ӳٺ�Ҫ��ʱ�̶�׼�������������ô˵��״̬����������¼�����
 * event-driven��̣���ActionScheduler���������time-driven���ģʽ������ActionScheduler
 * �ǳ���Ҫ��
 * 
 * ActionScheduler�Ƕ�����RTOS�ģ�����RTOS������񣬶���Ӧ�������п��������ң���OS�ṩ��
 * ���Ȳ�ͬ��ActionScheduler�ṩ���ǽϸ߾��ȵĵ��ȣ����Դﵽms�������ڸ���ͨ��Э�鿪����
 * �Ƿǳ����õ�
 * 
 * OOP�ı��ģʽʹ��ϵͳ�п���֧�ֶ��ActionScheduler���󣮵������������£�Ϊ�˽�ʡӲ��
 * ��Դ������ֻ��ϵͳ��ʹ��һ��ActionScheduler��������е��෽���action�������Ρ�
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
 * R: ���ϵͳ�ܼ򵥣�ֻ��Ҫ�򵥵Ķ�ʱ�жϣ�ֱ��ʹ��timer����Ҳ�ǹ��õģ���ô����ֱ��ʹ��
 * timer�����Ǻܶ�����£���Ҫ�ӳٵĵط��ܶ࣬�����Ǵ�ʱ�����໥�ص����������벻ͬ��timer
 * ������������Ҫ������������£�ActionScheduler�ṩ��һ���㹻���������Ӳ����Դ�٣�����
 * ��Ҫһ��TimerӲ��)�ķ���������ʽ��ϵͳ�У���������ʹ��ActionScheduler�������Timer��
 * 
 * Q: how to use ActionScheduler?
 * R: there're two basic programming patterns to use ActionScheduler:
 * 
 * 1) hardware timer interrupt driven
 * Ӳ��timer��������timer interrupt service routine��ִ��ÿһ��Action��������ʱÿһ��
 * Action�������ж�̬�����У�����Ҫ�ر�ע���������룬���Ǻ��п��ܻ᲻ǡ���Ĺص��жϵ���Scheduler
 * �޷�ִ����ȥ���÷����Ƚϼ򵥣�ʱ�侫����ߣ��������׳������Ժ����ѣ���ĳЩ�жϻ��ƹ���
 * �򵥵�MCU���Կ�����ʵ��. Timerֻ�ڶ�ʱʱ�̵���ʱ��Ŵ����жϣ�������ַ�ʽ�ǳ��ʺ�sleep/wakeup
 * �͹��ı��ģʽ��
 * 
 * 2) hardware timer interrupt driven, but let the main scheduler execute them
 * ��1)��ͬ���ǣ�hardware timer interruptֻ������һ������ִ�б�־����������actionִ��
 * �������ϲ��Ӧ�ó�����scheduler���ã���scheduler������RTOS�е�һ����ѭ���̣߳�Ҳ������
 * ǰ��̨��ϵ�еĺ�̨��ѭ��������ʱ�����ڸ���action��ִ�д��ж�̬�õ����ж��⣬������1)��
 * �ĸ���ȱ�㣬������Ҫ����һ�������ӳ٣�Ӧ<1ms)
 * 
 * 3) hardware timer driven, but state query policy used
 * ��Ȼ��Ӳ��timer���������ǲ�������interrupt���ƣ�������������ȥ��ѯtimer��ǰʱ�䣬Ȼ��
 * ����evolve(...)��������action���ã�
 * 
 * 4) software timer driven
 * ����ͨ��һ��ģ���software����ִ�У�
 * ��ͨ������evolve(...)����ʵ�ֵģ�
 * 
 * Q: �������ÿ��action��ִ��ʱ�̣�
 * R: ������ʱ��������������һ���������ĳ���̶��Ŀ�ʼʱ�̣�һ��������ڵ�ǰʱ�̣�ȱʡ���
 * ������ڵ�ǰʱ�̣�Ҳ������begin_duration������ʼʱ�̣�������end_duration�����������á�
 * 
 * Q: ����ڹ涨��ʱ�������ڣ�ĳActionû��ִ����ϻ�������
 * R: ActionScheduler�ڲ���֤���κ�һ��ʱ�̣�ֻ����һ��action��ִ�У����������롣��������
 * �������ʱ��ActionScheduler�������ȴ���Actionִ����϶�������;���ϣ��⽫���º����Action
 * �ӳ�ִ�С�Action����������Ӧ��ʹ��Scheduler���㹻��ʱ��ִ�������ǣ���Action��������
 * ���ߣ�˵��ϵͳ����̫��Ӧ�跨������ܻ�Ľ���ƣ���ʱ�����׷�����Դ�ľ����߽��벻��Ԥ��
 * ״̬���������Ҫ��ʵ���б��⡣
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
