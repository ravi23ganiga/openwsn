/* RingQueue Data Structure
 * This ring queue has fixed number of items/elements. It will not increase memory
 * when it's full. It's mainly used in kernel developing or embedded developing
 * without dynamic memory allocator support
 *
 * @author zhangwei in 200503
 * @modified by zhangwei in 200803
 *	- eliminate the call of malloc() and free().
 */

#include "osx_configall.h"
#include "osx_foundation.h"
#include "../hal/hal_interrupt.h"
#include "osx_queue.h"

inline bool sysque_empty( TiLightQueue * que )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_empty(que);
	hal_atomic_end();
	return ret;
}

inline bool sysque_full( TiLightQueue * que )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_full(que);
	hal_atomic_end();
	return ret;
}

inline void * sysque_front( TiLightQueue * que )
{
	void * item;
	hal_atomic_begin();
	item = lwque_front( que );
	hal_atomic_end();
	return item;
}

inline void * sysque_rear( TiLightQueue * que )
{
	void * item;
	hal_atomic_begin();
	item = lwque_rear( que );
	hal_atomic_end();
	return item;
}

inline bool sysque_pushback( TiLightQueue * que, void * item )  
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_pushback( que, item );
	hal_atomic_end();
	return ret;
}

inline bool sysque_pushfront( TiLightQueue * que, void * item )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_pushfront( que, item );
	hal_atomic_end();
	return ret;
}

inline bool sysque_popfront( TiLightQueue * que )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_popfront( que );
	hal_atomic_end();
	return ret;
}

inline bool sysque_poprear( TiLightQueue * que )
{
	bool ret;
	hal_atomic_begin();
	ret = lwque_poprear( que );
	hal_atomic_end();
	return ret;
}

