#ifndef _OSX_QUEUE_5678_
#define _OSX_QUEUE_5678_
/******************************************************************************
 * TiOsxQueue
 * osx queue is the event entrance of the osx kerenel. The hardware abstraction 
 * layer(hal) or other osx services can put events into the osx queue and wait for
 * further processing. 
 *****************************************************************************/

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
#include "../rtl/rtl_lightqueue.h"

/* RingQueue Data Structure
 * This ring queue has fixed number of items/elements. It will not increase memory
 * when it's full. It's mainly used in kernel developing or embedded developing
 * without dynamic memory allocator support
 *
 */


/* TiOsxQueue 基于 TiLightQueue实现，相比TiLightQueue，增加了互斥保护操作，
 * 使其可以用于multi-thread和interrupt service程序
 */

#define OSX_QUEUE_HOPESIZE(itemsize,capacity) LIGHTQUEUE_HOPESIZE(itemsize,capacity)
//#define LIGHTQUEUE_HOPESIZE(itemsize,capacity) (sizeof(TiLightQueue) + (itemsize)*(capacity))



/* @attention
 * @warning
 * you must declare the TiSysQueue variables with prefix "volatile", or else you 
 * still encounter multi-thread problems in developing.
 */

typedef TiLightQueue TiOsxQueue;
	
inline TiLightQueue * osx_queue_open( void * buf, uint16 size, uint16 itemsize )
{
	return lwque_construct( buf, size, itemsize );
}

inline void osx_queue_close( TiLightQueue * que )
{
	return lwque_destroy( que );
}

inline uint8 osx_queue_count( TiLightQueue * que )
{
	return lwque_count( que );
}

inline uint8 osx_queue_capacity( TiLightQueue * que )
{
	return lwque_capacity( que );
}

inline void * osx_queue_getbuf( TiLightQueue * que, uint8 idx )
{
	return lwque_getbuf( que, idx );
}

inline bool osx_queue_empty( TiLightQueue * que )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_empty(que);
	hal_atomic_end();
	return ret;
}

inline bool osx_queue_full( TiLightQueue * que )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_full(que);
	hal_atomic_end();
	return ret;
}

inline void * osx_queue_front( TiLightQueue * que )
{
	void * item;
	hal_atomic_begin();
	item = lwque_front( que );
	hal_atomic_end();
	return item;
}

inline void * osx_queue_rear( TiLightQueue * que )
{
	void * item;
	hal_atomic_begin();
	item = lwque_rear( que );
	hal_atomic_end();
	return item;
}

inline bool osx_queue_pushback( TiLightQueue * que, void * item )  
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_pushback( que, item );
	hal_atomic_end();
	return ret;
}

inline bool osx_queue_pushfront( TiLightQueue * que, void * item )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_pushfront( que, item );
	hal_atomic_end();
	return ret;
}

inline bool osx_queue_popfront( TiLightQueue * que )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_popfront( que );
	hal_atomic_end();
	return ret;
}

inline bool osx_queue_poprear( TiLightQueue * que )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_poprear( que );
	hal_atomic_end();
	return ret;
}

#endif
