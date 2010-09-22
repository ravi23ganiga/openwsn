/******************************************************************************
 * nano-os (nos)
 * this module is a nano/tiny/light embedded operating system core. 
 * 
 * running on: 
 *	- Atmel Atmega MCU such as:
 *  - Atmega128 MCU
 *
 * compiled by 
 *	- WinAVR 2008 (Portable WinAVR)
 *  - AVR Studio 4.x (with WinAVR 2009)
 *****************************************************************************/


#include "../hal/hal_foundation.h"
#include "osx_nano.h"  

typedef struct _nos_sched_entry_t {
	void (*tp)(void);
}nos_sched_entry_t;

enum OS_CON{
	OS_MAX_TASKS = 8, 
	OS_TASK_BITMASK = OS_MAX_TASKS - 1
};

volatile nos_sched_entry_t g_taskqueue[OS_MAX_TASKS];
uint8_t g_nos_sched_full;
volatile uint8_t g_nos_sched_free;

static bool nos_run_next_task(void);
static void nos_wait(void);


inline void nos_atomic_end(nos_atomic_t state)
{
	* (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20) = state;
}

inline nos_atomic_t nos_atomic_start(void )
{
	nos_atomic_t result = * (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x3F + 0x20);
	__asm volatile ("cli");
	return result;
}

inline void nos_atomic_enable_interrupt(void)
{
	__asm volatile ("sei");
}

inline void nos_wait(void)
{
   __asm volatile ("nop");
   __asm volatile ("nop");}

 inline void nos_sleep(void)
{
	* (volatile unsigned char *)(unsigned int )& * (volatile unsigned char *)(0x35 + 0x20) |= 1 << 5;
	__asm volatile ("sleep");
}
   
inline void nos_init(void )
{
	g_nos_sched_free = 0;
	g_nos_sched_full = 0;
	//hal_set_nospost( os_post );
}

bool nos_post(void (*tp)(void))
{
	nos_atomic_t flag;
	uint8_t tmp;
	flag = nos_atomic_start();
	tmp = g_nos_sched_free;

    if (g_taskqueue[tmp].tp == (void *)NULL) 
	{
		g_nos_sched_free = (tmp + 1) & OS_TASK_BITMASK;
		g_taskqueue[tmp].tp = tp;
		nos_atomic_end(flag);
		return TRUE;
    }
	else {
		nos_atomic_end(flag);
		return FALSE;
    }
}

/* @attention
 * - the developer should provide the implementation of this function
 * developer should start the threads (by call os_post function)
 * according to the event information 
 * 
 * - this function should registered in hal layer by call hal_setlistener()
 * or else the hal layer doesn't know which where's the listener handler.
 * 
 * - the function name nos_listener() isn't mandatory. you can name it with any
 * other names unless TiFunEventHandler type.
 */
void nos_listener( void * object, TiEvent * e)
{
	// todo: 根据event中的内容post一个thread起来
	// 通常，该listener应有开发者创建，本身并非nano os组成部分
	// os_post( os_task_t * task );
}

inline bool nos_run_next_task(void)
{
	nos_atomic_t flag;
	uint8_t old_full;
	void (*func)(void );

	flag = nos_atomic_start();
	old_full = g_nos_sched_full;
	func = g_taskqueue[old_full].tp;
	if (func == (void *)0) 
    {
		nos_atomic_end(flag);
		return 0;
    }

	g_taskqueue[old_full].tp = (void *)0;
	g_nos_sched_full = (old_full + 1) & OS_TASK_BITMASK;
	nos_atomic_end(flag);
	func();
	return 1;
}

inline void nos_run_task(void)
{
	while (nos_run_next_task()) {};
	nos_sleep();
	nos_wait();
}

#ifdef OS_NANO_TEST
void nos_test()
{
	hal_init( nos_listener, NULL );
	os_run_task();
}
#endif
