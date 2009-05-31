#ifndef _OSX_NANO_H_9062_
#define _OSX_NANO_H_9062_
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
 * 
 * attention
 *	this is super light os kernel which can run on small memory constraint MCU.
 *  it's not intended for complex task schedulings. and it may conflict with other
 *  embedded os kernels. generally, you should use only one os kernel in one 
 *  application.
 *
 * state
 *	compile passed. still needs testing.
 *
 * @author zhangwei on 20090523, Dept of Control, TongJi University, China
 *	- greate revison based on old source code
 *
 *****************************************************************************/

#include "../hal/hal_configall.h"
#include "../hal/hal_foundation.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Q: how to use nano-os (nos)
 * A: nano os is a none preemptve os kernel. it's based on hardware abstraction 
 * layer (hal). so: 
 *	- porting hal layer first before porting the nano os kernel. 
 *  - in your application, you should call hal_nosinit(os_post). 
 *  - call os_runtask() in an infinite while loop.
 *
 * example:
 *		int main(void)
 *		{
 *			target_init();
 *			hal_init();
 *			hal_nosinit(os_post)
 *			......
 *			while (1) {os_runtask();}
 *		}
 * 
 * Q: how to create an task in interrupt service routines?
 * A: interrupt service routine usually belongs to hal layer. so it should NOT
 *	start a task by call os_post() function directly because os kernel is running
 *  on top of the hal layer. 
 *    an ordinary application architecture is as the following:
 *
 *		[                  Application                   ]
 *      [ Network Communication and Other Local Services ]
 *		[             Nano OS Kernel (nos)               ] 
 *      [        Hardware Abstraction Layer (hal)        ]
 *  
 * running on: 
 *	- Atmel Atmega MCU such as:
 *  - Atmega128 MCU
 *
 * compiled by 
 *	- WinAVR 2008 (Portable WinAVR)
 *  - AVR Studio 4.x (with WinAVR 2009)
 * 
 * attention
 *	this is super light os kernel which can run on small memory constraint MCU.
 *  it's not intended for complex task schedulings. and it may conflict with other
 *  embedded os kernels. generally, you should use only one os kernel in one 
 *  application.
 *
 * state
 *	compile passed. still needs testing.
 *
 * @author zhangwei on 20090523
 *	- greate revison based on old source code
 *
 *****************************************************************************/
	
typedef uint8_t os_atomic_t;
typedef void (* os_task_t)(void);

/* the nano os is built based on the hardware abstraction layer. the "hal" layer
 * must provide the following function. the nano os will call this function in its
 * intialization process.
 *
 * this function is implemented inside the hal layer. usually in the module named
 * "hal_foundation". 
 */
//extern void hal_set_nospost(bool (* nospost)(os_task_t task));

/* os_atomic_start()
 * start an atomic processing
 * 
 * os_atomic_end()
 * end an atomic processing. this function must be used with os_atomic_start()
 * function and these two functions should be used inside one function only. 
 * there's no guarantee on the system behavior if the function pair appears in 
 * two functions. 
 */

os_atomic_t os_atomic_start(void);
void os_atomic_end(os_atomic_t state);
void os_atomic_enable_interrupt(void);

/* os_init
 * this function must be called before you call os_post() function. 
 *
 * os_sleep
 * make the system into sleep mode. this function is called when there's no tasks
 * running. 
 *
 * os_post
 * create a task 
 */
 
void os_init(void);
void os_sleep(void);
void os_run_task(void);
bool os_post(void (*tp)(void));

void nos_listener( void * object, TiEvent * e);

#ifdef OS_NANO_TEST
void os_test();
#endif

#ifdef __cplusplus
}
#endif

#endif

