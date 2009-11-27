#ifndef _OSX_FOUNDATION_4328_
#define _OSX_FOUNDATION_4328_
/******************************************************************************
 * osx_foundation
 *
 * @author zhangwei on 200610
 * @author zhangwei on 200904
 *	- revision
 * @author zhangwei on 200906
 *	- add osx timer
 *****************************************************************************/

/* fundamental declarations for the kernel
 *
 * Q: How to port the kernel to a new platform
 * R:
 * step1: implement necessary hardware adapter objects
 * step2: update this module "os_foundation.h" to use new adapter objects.
 * then you should be able to compile the kernel successfully, because the kernel
 * is standard ANSI C.
 *
 * the kernel is based on hardware adapter objects and osx_foundation, so what
 * you need to do is porting this module only to the new platform.
 *
 *   osx_kernel, osx_simplesche, osx_rtsche, ...
 *        |-------------|-------------|
 *         osx_timer, osx_queue, osx_dba
 *                      |
 *               osx_foundation.h
 *                      |
 *               osx_configall.h
 */

#include "osx_configall.h"
#include "../rtl/rtl_foundation.h"
#include "../rtl/rtl_lightqueue.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_systimer.h"
#include "../hal/hal_interrupt.h"
#include "../hal/hal_cpu.h"
#include "osx_timer.h"
#include "osx_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * osx critical
 *****************************************************************************/

inline void osx_atomic_begin() 
{
	hal_atomic_begin();
}

inline void osx_atomic_end() 
{
	hal_atomic_end();
}

/******************************************************************************
 * Power Management
 *****************************************************************************/

inline void _osx_cpu_sleep() {cpu_sleep();}
inline void _osx_cpu_nop() {cpu_nop();}
//inline void _osx_cpu_wakeup() {cpu_nop();}
inline void _osx_cpu_wakeup() {;}

inline void _osx_target_on_sleep( void * svcptr, TiEvent * e )
{
	// todo
	// do something before sleep
	// such as saving data in the RAM
	cpu_sleep();
}

inline void _osx_target_on_wakeup( void * svcptr, TiEvent * e )
{
/*	cpu_nop();
	cpu_nop();
	cpu_nop();*/
	// todo
	// do something after wakeup, such as restore RAM content
}

inline void _osx_on_target_reboot( void * svcptr, TiEvent * e )
{
	// reboot the whole application
	// todo: save RAM content and necessary configurations
	//target_reboot();
}

inline void _osx_on_target_startup( void * svcptr, TiEvent * e )
{
	// todo do something for startup
}

inline void _osx_on_target_shutdown( void * svcptr, TiEvent * e )
{
	// todo do something before really shutdown
}

//inline void _osx_sleep()
// catnap, shortsleep, restsleep
// deepsleep
// softdown/shutdown/startup  shutdown is called by software,it's actually a soft shutdown, can be wakeup by RTC
// shutdown is totally shutdown. no software  method can start it up.
//_osx_wakeup
// which one should I call in RTC handler? osx_wakeup or _osx_base_startup?
/*
inline void _osx_sleep() {};  
inline void _osx_deepsleep() {};  
inline void _osx_set_energy_saving_mode() {};

_osx_cpu_cpu_sleep
generally, we should append NOP after sleep instruction to avoid work flow problems in ISR.

osx_target_sleep
osx_target_wakeup
osx_target_startup
osx_target_shutdown
*/

#ifdef __cplusplus
}
#endif

#endif
