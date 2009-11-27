/*
 * rtl_dispatcher.c
 * event dispatcher object. usually dispatch one event to one object. 
 *
 * @state
 *	tested
 *
 * @author zhangwei on 200903
 *	- first created. inspired by rtl_notifier. 
 */

#include "rtl_configall.h"
#include <stdlib.h>
#include <string.h>
#include "rtl_foundation.h"
#include "rtl_dispatcher.h"
#include "../hal/hal_debugio.h"


inline _TiDispatcherItem * _dispa_items( TiDispatcher * dispa );
inline bool _dispa_search( _TiDispatcherItem * items, uint8 capacity, uint8 eid, uint8 * pidx );
inline bool _dispa_apply( _TiDispatcherItem * items, uint8 capacity, uint8 * pidx );

TiDispatcher * dispa_create( uint8 capacity )
{
	uint16 size = DISPA_HOPESIZE(capacity);
	void * buf;
	
	buf = malloc( size  );
	if (buf != NULL)
	{
		memset( buf, 0x00, size );
		dispa_construct( buf, size, capacity );
	}

	return (TiDispatcher *)buf;
}

void dispa_free( TiDispatcher * dpa )
{
	dispa_destroy(dpa);
}

TiDispatcher * dispa_construct( char * buf, uint16 size, uint8 capacity )
{
	TiDispatcher * dpa = (TiDispatcher *)buf;
	rtl_assert( capacity > 0 );
	dpa->size = size;
	dpa->capacity = capacity;
	dpa->owner = NULL;
	return dpa;
}

void dispa_destroy( TiDispatcher * dpa )
{
	return;
}

/**
 * attention
 * The "handler" paramter should NOT be NULL, or an assert() in dispa_send() will failed.
 * However, the "object" parameter can be NULL.
 */
bool dispa_attach( TiDispatcher * dpa, uint8 id, TiFunEventHandler handler, void * object )
{
	_TiDispatcherItem * items = _dispa_items( dpa );
	uint8 idx=0;
	bool found = false;

	idx = id % dpa->capacity;
	if ((items[idx].id == id) || (items[idx].id == 0))
	{
		found = true;
	}
	else{
		found = _dispa_search(items, dpa->capacity, id, &idx);
		if (!found)
		{
			idx = id % (dpa->capacity) + 1;
			found = _dispa_apply(items, dpa->capacity, &idx);
		}
	}

	if (found)
	{
		items[idx].id = id;
		items[idx].handler = handler;
		items[idx].object = object;
	}

	return found;
}

void dispa_detach( TiDispatcher * dpa, uint8 id )
{
	_TiDispatcherItem * items = _dispa_items( dpa );
	uint8 idx=0;

	idx = id % (dpa->capacity);
	if (items[idx].id == id)
	{
		items[idx].id = 0;
		items[idx].handler = NULL;
		items[idx].object = NULL;
	}
	else{
		idx ++;
		if (_dispa_search(items, dpa->capacity, id, &idx))
		{
			items[idx].id = 0;
			items[idx].handler = NULL;
			items[idx].object = NULL;
		}
	}
}

/* attention
 *	- dispa_send() can be the listener of other components. because it's a standard 
 *    TiFunEventHandler type.
 */
void dispa_send( TiDispatcher * dpa, TiEvent * e )
{
	_TiDispatcherItem * items = _dispa_items( dpa );
	uint8 idx=0;
	bool found = false;

	rtl_assert((e != NULL) && (e->id != 0));
	if (e->id == 0)
		return;
	
	idx = e->id % (dpa->capacity);
	
	if (items[idx].id == e->id)
	{
		found = true;
	}
	else{
		idx ++;
		found = _dispa_search(items, dpa->capacity, e->id, &idx);
	}
	if (found)
	{	
		/* attention
		 * If this assert() failed, it often means the user gives wrong parameter 
		 * values of "handler" to the dispa_attach() function. The "handler" parameter
		 * shouldn't be NULL.
		 */ 
		rtl_assert( items[idx].handler != NULL );
		items[idx].handler( items[idx].object, e );
	}
}

inline _TiDispatcherItem * _dispa_items( TiDispatcher * dpa )
{
	return (_TiDispatcherItem *)( (char*)dpa + sizeof(TiDispatcher) );
}

/* search for specific items in the list
 * parameter
 *  pidx			     the final index after execution if found.
 */
inline bool _dispa_search( _TiDispatcherItem * items, uint8 capacity, uint8 id, uint8 * pidx )
{
	bool found = false;
	uint8 i=0, idx;

	idx = id % capacity;
	if (items[idx].id == id)
	{
		* pidx = idx;
		return true;
	}

	for (i=idx+1; i<capacity; i++)
	{
		if (items[i].id == id)
		{
			found = true;
			* pidx = i;
		}
	}
	if (!found)
	{
		for (i=0; i<idx-1; i++)
		{
			if (items[i].id == id)
			{
				found = true;
				* pidx = i;
			}
		}
	}

	return found;
}

/* allocate an empty item in the dispatcher item list
 * 
 * parameter
 *	pidx			(*pidx) is the initial start searching position
 *                  also the returned index if return true
 * 
 * return
 *  true when success, and false if failed. *pidx is the index if success.
 */
inline bool _dispa_apply( _TiDispatcherItem * items, uint8 capacity, uint8 * pidx )
{
	bool found = false;
	uint8 i=0, idx;

	idx = *pidx;
	for (i=*pidx; i<capacity; i++)
	{
		if ((items[i].id == 0) && (items[i].handler == NULL))
		{
			found = true;
			*pidx = i;
		}
	}
	if (!found)
	{
		for (i=0; i<(*pidx)-1; i++)
		{
			if ((items[i].id == 0) && (items[i].handler == NULL))
			{
				found = true;
				*pidx = i;
			}
		}
	}

	return found;
}

/* release the item with specific id
 *
 * parameter
 *	idx             initial start search position
 */
bool _dispa_release( _TiDispatcherItem * items, uint8 capacity, uint8 id, uint8 idx )
{
	bool found = false;
	//uintx i=0, idx;
/*
	// to do in the future
	rtl_assert( false );
*/
	return found;
}





