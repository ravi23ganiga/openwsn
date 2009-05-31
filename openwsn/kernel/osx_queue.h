#ifndef _RTL_SYSQUEUE_H_9032_
#define _RTL_SYSQUEUE_H_9032_

/* TiLightQueue Data Structure
 * this is a C language based queue data structure. it's actaully an ring queue.
 * different to TiDynaQueue, the TiLightQueue has fixed number of items/elements.
 * It will not increase memory when it's full. It's mainly used in kernel developing
 * or embedded developing without dynamic memory allocator support.
 *
 * another advantage of TiLightQueue is that it can be moved to another place
 * inside the address space and can still work.
 *
 * portability
 *	- ansi c language
 *  - linux, windows, and embedded platforms with c compiler only
 *
 * @state
 *	finished but not tested yet
 *
 * @author zhangwei in 200503
 * @modified by zhangwei in 200803
 *	- eliminate the call of malloc() and free().
 * @modified by zhangwei in 200905
 *	- change the name as TiLightQueue. the original name is TiRingQueue
 *    the new predix "lwque_" means "light weight queue"
 */

#include "osx_configall.h"
#include "osx_foundation.h"
#include "../hal/hal_interrupt.h"
#include "../hal/hal_cpu.h"
#include "../rtl/rtl_lightqueue.h"

/* TiSysQueue 基于 TiLightQueue实现，相比TiLightQueue，增加了互斥保护操作，
 * 使其可以用于multi-thread和interrupt service程序
 */

#define SYSQUEUE_HOPESIZE(itemsize,capacity) LIGHTQUEUE_HOPESIZE(itemsize,capacity)

#ifdef __cplusplus
extern "C" {
#endif

/* @attention
 * @warning
 * you must declare the TiSysQueue variables with prefix "volatile", or else you 
 * still encounter multi-thread problems in developing.
 */

#define TiSysQueue TiLightQueue

inline TiLightQueue * sysque_construct( void * buf, uintx size, uintx itemsize )
{
	return lwque_construct( buf, size, itemsize );
}

inline void sysque_destroy( TiLightQueue * que )
{
	return lwque_destroy( que );
}

inline uintx sysque_count( TiLightQueue * que )
{
	return lwque_count( que );
}

inline uintx sysque_capacity( TiLightQueue * que )
{
	return lwque_capacity( que );
}

inline bool sysque_empty( TiLightQueue * que );

inline bool sysque_full( TiLightQueue * que );

inline void * sysque_getbuf( TiLightQueue * que, uintx idx )
{
	return lwque_getbuf( que, idx );
}

inline void * sysque_front( TiLightQueue * que );
inline void * sysque_rear( TiLightQueue * que );
inline bool sysque_pushback( TiLightQueue * que, void * item );
inline bool sysque_pushfront( TiLightQueue * que, void * item );
inline bool sysque_popfront( TiLightQueue * que );
inline bool sysque_poprear( TiLightQueue * que );
	
#ifdef __cplusplus
}
#endif

#endif
