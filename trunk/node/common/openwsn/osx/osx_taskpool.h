#ifndef _OSX_TASKPOOL_H_5678_
#define _OSX_TASKPOOL_H_5678_

#include "osx_configall.h"
#include "osx_foundation.h"

#define CONFIG_OSX_TASKPOOL_CAPACITY 8

// #define OSX_TASKPOOL_HOPESIZE(capacity) (sizeof(TiOsxTaskPool)+sizeof(_TiOsxTaskPoolItem)*(capacity))

/**
 * This structure is used in osx internal to record the task related information.
 * 
 * - taskid: task id. this id keep unchanged when a task descriptor is allocated. 
 * - taskfunction: function pointer
 * - taskptr: an pointer to the structure variable of the current task
 * - timeline: when the task should be started to run
 * - deadline: deadline for the task (reserved for future)
 * - listprev: (reserved for future) 
 * - listnext: (reserved for future) 
 * - itemprev: the index of the previous item in the allocated list or freed list
 * - itemnext: the index of the next item in the allocated list or freed list
 */
struct _TiOsxTaskPoolItem;
struct _TiOsxTaskPoolItem{
    uint8       state;
    int8        taskid;
    TiOsxTask   taskfunction;
    void *      taskdata;
    int16       timeline;
    int8        priority;
    int16       deadline;
    int8        heapindex;
    // int8     listprev;
    // int8     listnext;
    // int8     itemprev;
    int8        itemnext;
};
typedef struct _TiOsxTaskPoolItem  TiOsxTaskPoolItem;

/**
 * Implements a task descriptor pool. This pool can support allocating and free a 
 * task descriptor structure in O(1) time.
 * 
 * - count: how many items has been allocated
 * - allocated: this is the head item of the allocated item list
 * - freed: this is the head item of the freed item list.
 */
typedef struct{
    //int8 capacity;
    int8 count;
    //int8 allocated;
    int8 emptylist;
    TiOsxTaskPoolItem items[CONFIG_OSX_TASKPOOL_CAPACITY];
}TiOsxTaskPool;

TiOsxTaskPool * osx_taskpool_construct( char * mem, uint16 memsize );
void osx_taskpool_destroy( TiOsxTaskPool * tpl );

int8 osx_taskpool_apply( TiOsxTaskPool * tpl );
void osx_taskpool_release( TiOsxTaskPool * tpl, int8 id );

uint8 osx_taskpool_capacity( TiOsxTaskPool * tpl );
uint8 osx_taskpool_count( TiOsxTaskPool * tpl );
void osx_taskpool_clear( TiOsxTaskPool * tpl );

TiOsxTaskPoolItem * osx_taskpool_gettaskdesc( TiOsxTaskPool * tpl, int8 id );
int8 osx_taskpool_gettaskid( TiOsxTaskPool * tpl, TiOsxTaskPoolItem * item );

bool osx_taskpool_full( TiOsxTaskPool * tpl );
bool osx_taskpool_empty( TiOsxTaskPool * tpl );

#endif
