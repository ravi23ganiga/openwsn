#ifndef _OSX_FOUNDATION_4328_
#define _OSX_FOUNDATION_4328_
/******************************************************************************
 * osx_foundation
 *
 * @author zhangwei on 200610
 * @author zhangwei on 200904
 *	- revision
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
 *   osx_kernel, osx_simplesche, osx_rtsche
 *        |-------------|-------------|
 *                      |
 *               osx_foundation.h
 *                      |
 *               osx_configall.h
 */

#include "osx_configall.h"
#include "../rtl/rtl_foundation.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_timer.h"

//#define TiFunDispatchHandler TiFunEventHandler

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * osx critical
 *****************************************************************************/

#define osx_atomic_begin() hal_atomic_begin()
#define osx_atomic_end() hal_atomic_end()

/******************************************************************************
 * osx timer
 * the osx timer is used to drive the evolution of the whole osx kernel by periodically
 * call osx_hardevolve() function. it can also be used to drive some inaccurate
 * sampling tasks. if there's no osx timer, then the system can also drive the
 * osx kernel by infinite loop run. refer to the osx_execute() function. the infinite
 * loop run or osx_execute() can be placed into a separate thread of other RTOS.
 *
 * the MCU's hardware timers are usually very powerful. furthermore, they maybe
 * varies from chip suppliers. however, the OS core only needs a relatively simple
 * timer. it should be lightweight and efficient.
 *
 * the default "osx" system timer is the timer 0 in "hal" layer.
 *
 * for some MCU such as the ARM Cortex M3, the MCU core has already provide a
 * hardware SysTimer to help implement a OS core.
 *****************************************************************************/

/* if we use g_timer0 as our osx system timer */

/*
#define osx_systm_gettime()
#define osx_systm_settime()
#define osx_systm_elapsed()
#define osx_systm_restart()
#define osx_systm_configure()
#define osx_systm_expired()
#define osx_systm_handler()

#define osx_systimer_attach
#define osx_systimer_detach
#define osx_systimer_handler()
*/

#ifdef __cplusplus
}
#endif

#endif
