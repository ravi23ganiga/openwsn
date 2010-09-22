#ifndef _OSX_TASKHEAP_H_4698_
#define _OSX_TASKHEAP_H_4698_

#include "osx_configall.h"
#include "osx_foundation.h"
#include "osx_taskpool.h"

#define CONFIG_OSX_FIFOSCHE_CAPACITY

#define TiOsxFifoSchedulerItem TiOsxTaskPoolItem

typedef struct{
    uint8 count;
    TiOsxFifoSchedulerItem * items[CONFIG_OSX_FIFOSCHE_CAPACITY];
    TiOsxTaskPool * taskpool;
}TiOsxFifoScheduler;

TiOsxFifoScheduler * osx_fifosche_construct( char * mem, uint16 memsize )
{

}

void osx_fifosche_destroy( TiOsxFifoScheduler * sche )
{

}

TiOsxFifoScheduler * osx_fifosche_open( TiOsxFifoScheduler * heap, TiOsxTaskPool * tpl );
{
}

void osx_fifosche_close( TiOsxFifoScheduler * heap );
{
}

int8 osx_fifosche_taskspawn( TiOsxFifoScheduler * sche, void * taskfunction, void * taskdata, int16 starttime, uint8 option )
{

}


/**
 * Check the task queue and execute them. After exection, the task will be removed
 * from the internal task queue and return to the osx task pool.
 */
void osx_fifosche_evolve( TiOsxFifoScheduler * sche, void * e )
{

}

void osx_fifosche_execute( TiOsxFifoScheduler * sche )
{

}

void osx_fifosche_kill( TiOsxFifoScheduler * sche, uint8 id )
{

}

void osx_fifosche_restart( TiOsxFifoScheduler * sche, uint8 id, int16 starttime )
{
}




uint8 osx_fifosche_capacity( TiOsxFifoScheduler * heap );
uint8 osx_fifosche_count( TiOsxFifoScheduler * heap );
bool osx_fifosche_empty( TiOsxFifoScheduler * heap );
bool osx_fifosche_full( TiOsxFifoScheduler * heap );
void osx_fifosche_clear( TiOsxFifoScheduler * heap );
TiOsxFifoSchedulerItem ** osx_fifosche_items( TiOsxFifoScheduler * heap );

