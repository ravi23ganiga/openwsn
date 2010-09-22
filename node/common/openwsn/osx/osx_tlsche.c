#include "osx_configall.h"
#include <string.h>
#include "osx_foundation.h"
#include "../hal/hal_led.h"
#include "osx_tlsche.h"
#include "osx_taskheap.h"
#include "osx_taskpool.h"

#include <avr/sleep.h>

void _osx_taskheap_item_dump( TiOsxTaskHeap * heap, int8 idx );//jiade
void _osx_taskheap_dump( TiOsxTaskHeap * heap );//jiade



TiOsxTimeLineScheduler * osx_tlsche_open( TiOsxTimeLineScheduler * sche, TiOsxTimer2 * timer )
{
    osx_taskpool_construct( (char *)(&sche->taskpool), sizeof(TiOsxTaskPool) );
    osx_taskheap_open( &(sche->taskheap), &(sche->taskpool) );
    sche->timer = timer; 
	return sche;
}

void osx_tlsche_close( TiOsxTimeLineScheduler * sche )
{
    // rtc_stop( sche->timer );
    osx_taskheap_close( &(sche->taskheap) );
    osx_taskpool_destroy( &(sche->taskpool) );
}

int8 osx_tlsche_taskspawn( TiOsxTimeLineScheduler * sche, TiOsxTask taskfunction, 
    void * taskdata, int16 starttime, uint8 priority, uint8 option )
{
    TiOsxTaskHeapItem item;
    memset( &item, 0x00, sizeof(TiOsxTaskHeapItem) );
    item.taskfunction = taskfunction;
    item.taskdata = taskdata;
    item.timeline = starttime;
    item.priority = 0;
    item.deadline = 0xFFFF;

    return osx_taskheap_insert( &(sche->taskheap), &item );
}

void osx_tlsche_evolve( TiOsxTimeLineScheduler * sche, void * e )
{    
    TiOsxTaskHeapItem * desc;
	

    do{
        desc = osx_taskheap_root( &(sche->taskheap) );
        if (desc == NULL)
            break;

        if (desc->timeline <= 0)
        {
            desc->taskfunction( desc->taskdata, NULL );
            osx_taskheap_deleteroot( &(sche->taskheap) );
        }
        else
            break;
    }while (true);

    osx_tlsche_stepforward( sche, CONFIG_OSX_TIMER_INTERVAL );
}

void osx_tlsche_execute( TiOsxTimeLineScheduler * sche )
{
	rtc_setinterval( sche->timer, 0, 2, 0x01); //定时周期为一秒 
	hal_enable_interrupts();
	rtc_start( sche->timer );

    while (1)
    {
        osx_tlsche_evolve( sche, NULL );
	
        set_sleep_mode(SLEEP_MODE_IDLE);
	    sleep_enable();
	    sleep_cpu();
	    sleep_disable();
    }

    // rtc_stop( sche->rtc );
}

void osx_tlsche_kill( TiOsxTimeLineScheduler * sche, uint8 id )
{
    osx_taskheap_delete( &(sche->taskheap), osx_taskpool_gettaskdesc(&(sche->taskpool), id) );
}

void osx_tlsche_restart( TiOsxTimeLineScheduler * sche, uint8 id, int16 starttime )
{
    //osx_taskheap_reinsert
}

void osx_tlsche_stepforward( TiOsxTimeLineScheduler * sche, uint16 slicecount )
{
    uint8 i;
    TiOsxTaskHeap * heap = &(sche->taskheap);

    for (i=0; i<osx_taskheap_count(heap); i++)
    {
        osx_assert( heap->items[i] != NULL );
        heap->items[i]->timeline -= slicecount;
    }
}


