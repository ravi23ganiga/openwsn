#include "osx_configall.h"
#include <string.h>
#include "osx_foundation.h"
#include "osx_taskpool.h"

static inline TiOsxTaskPoolItem * _osx_taskpool_items( TiOsxTaskPool * tpl);
/*
TiOsxTaskPool * osx_taskpool_create( int8 capacity )
{
    uint16 memsize = OSX_TASKPOOL_HOPESIZE(capacity);
    char * mem;

    mem = (char *)malloc( OSX_TASKPOOL_HOPESIZE() );
    if (mem != NULL)
        return osx_taskpool_construct( mem, memsize );
    else
        return NULL;    
}

void osx_taskpool_free( TiOsxTaskPool * tpl )
{
    if (tpl != NULL)
    {
        osx_taskpool_destroy( tpl );
        free( tpl );
    }
}
*/

TiOsxTaskPool * osx_taskpool_construct( char * mem, uint16 memsize )
{
    TiOsxTaskPool * tpl = (TiOsxTaskPool *)mem;
    TiOsxTaskPoolItem * item;
    int8 i;

    memset( mem, 0x00, memsize );
    //tpl->capacity = (memsize - sizeof(TiOsxPool)) / sizeof(TiOsxTaskPoolItem);
    tpl->count = 0;
    //tpl->allocated = -1;
    tpl->emptylist = 0;

    osx_assert( CONFIG_OSX_TASKPOOL_CAPACITY > 0 );

    item = _osx_taskpool_items( tpl );
    for (i=0; i<CONFIG_OSX_TASKPOOL_CAPACITY; i++)
    {
        item[i].state = 0;
        item[i].taskid = i;
        item[i].taskfunction = NULL;
        item[i].taskdata = NULL;
        item[i].timeline = 0;
        item[i].deadline = 0;
        item[i].heapindex = i;
        //item[i].itemprev = -1;
    }

    // build the empty list
    for (i=0; i<CONFIG_OSX_TASKPOOL_CAPACITY-1; i++)
    {
        item[i].itemnext = i+1;
    }
    item[CONFIG_OSX_TASKPOOL_CAPACITY - 1].itemnext = -1;

	return tpl;
}

void osx_taskpool_destroy( TiOsxTaskPool * tpl )
{
    tpl->count = 0;
    return;
}


/**
 * Apply an task descriptor item. This operation can be finished in O(1) time.
 */
int8 osx_taskpool_apply( TiOsxTaskPool * tpl )
{
    TiOsxTaskPoolItem * items;
    int8 taskid = -1;

    if (tpl->count < CONFIG_OSX_TASKPOOL_CAPACITY)
    {
        items = _osx_taskpool_items( tpl );
        taskid = tpl->emptylist;
        tpl->emptylist = items[taskid].itemnext;
        //items[taskid].next = tpl->allocated;
        //items[taskid].prev = -1;
        //tpl->allocated = taskid;
        items[taskid].state = 1;
        tpl->count ++;
    }

    return taskid;
}


/**
 * Release a task descriptor item and return it to the emptylist list.
 */
void osx_taskpool_release( TiOsxTaskPool * tpl, int8 id )
{
    TiOsxTaskPoolItem * items = _osx_taskpool_items(tpl);

    if (items[id].state != 0)
    {
        items[id].itemnext = tpl->emptylist;
        tpl->emptylist = id;
        items[id].state = 0;
        tpl->count --;
    }
}

uint8 osx_taskpool_capacity( TiOsxTaskPool * tpl )
{
    return CONFIG_OSX_TASKPOOL_CAPACITY;
}

uint8 osx_taskpool_count( TiOsxTaskPool * tpl )
{
    return tpl->count;
}

void osx_taskpool_clear( TiOsxTaskPool * tpl )
{
	return;
}

TiOsxTaskPoolItem * osx_taskpool_gettaskdesc( TiOsxTaskPool * tpl, int8 id )
{
    return _osx_taskpool_items(tpl) + id;
}

int8 osx_taskpool_gettaskid( TiOsxTaskPool * tpl, TiOsxTaskPoolItem * item )
{
    //return (taskdesc - _osx_taskpool_items(tpl)) / sizeof(TiOsxTaskPoolItem));
    return item->taskid;
}

inline TiOsxTaskPoolItem * _osx_taskpool_items( TiOsxTaskPool * tpl )
{
    //return (TiOsxTaskPoolItem *)((char*)tpl + sizeof(TiOsxTaskPool));
    return &(tpl->items[0]);
}

bool osx_taskpool_full( TiOsxTaskPool * tpl )
{
    return (tpl->count == CONFIG_OSX_TASKPOOL_CAPACITY);
}

bool osx_taskpool_empty( TiOsxTaskPool * tpl )
{
    return (tpl->count == 0);
}

