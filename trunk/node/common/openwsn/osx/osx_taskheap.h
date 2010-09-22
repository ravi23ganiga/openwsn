#ifndef _OSX_TASKHEAP_H_4698_
#define _OSX_TASKHEAP_H_4698_

#include "osx_configall.h"
#include "osx_foundation.h"
#include "osx_taskpool.h"

/**
 * The future task list uses heap data structure to balance the performance of task 
 * searching, inserting and removing. 
 * 
 * TiOsxTaskHeap is also an independent component which can be used in applications. 
 * You can select it and enable the future-scheduling algorithm or unselect it to 
 * choose simple round robin scheduling.
 */

#define CONFIG_OSX_TASKHEAP_CAPACITY CONFIG_OSX_TASKPOOL_CAPACITY

#define TiOsxTaskHeapItem TiOsxTaskPoolItem

typedef struct{
    uint8 count;
    TiOsxTaskHeapItem * items[CONFIG_OSX_TASKHEAP_CAPACITY];
    TiOsxTaskPool * taskpool;
}TiOsxTaskHeap;


//TiOsxTaskHeap * osx_taskheap_construct( char * mem, uint16 memsize );
//void osx_taskheap_destroy( TiOsxTaskHeap * heap );
TiOsxTaskHeap * osx_taskheap_open( TiOsxTaskHeap * heap, TiOsxTaskPool * tpl );
void osx_taskheap_close( TiOsxTaskHeap * heap );


uint8 osx_taskheap_capacity( TiOsxTaskHeap * heap );
uint8 osx_taskheap_count( TiOsxTaskHeap * heap );
bool osx_taskheap_empty( TiOsxTaskHeap * heap );
bool osx_taskheap_full( TiOsxTaskHeap * heap );
void osx_taskheap_clear( TiOsxTaskHeap * heap );
TiOsxTaskHeapItem ** osx_taskheap_items( TiOsxTaskHeap * heap );

TiOsxTaskHeapItem * osx_taskheap_apply( TiOsxTaskHeap * heap );
void osx_taskheap_release( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item );

TiOsxTaskHeapItem * osx_taskheap_root( TiOsxTaskHeap * heap );
void osx_taskheap_deleteroot( TiOsxTaskHeap * heap );
int8 osx_taskheap_delete( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item );
bool osx_taskheap_deleteat( TiOsxTaskHeap * heap, uint8 idx );
int8 osx_taskheap_insert( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item );
int8 osx_taskheap_search( TiOsxTaskHeap * heap, TiOsxTaskHeapItem * item );
void osx_taskheap_sort( TiOsxTaskHeap * heap );
void create_task( void *funptr, TiOsxTaskHeap * heap, int16 timeline);

#endif /* _OSX_TASKHEAP_H_4698_ */
