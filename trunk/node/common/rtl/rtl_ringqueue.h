#ifndef _RTL_RINGQUEUE_H_9032_
#define _RTL_RINGQUEUE_H_9032_

/* RingQueue Data Structure
 * This ring queue has fixed number of items/elements. It will not increase memory
 * when it's full. It's mainly used in kernel developing or embedded developing
 * without dynamic memory allocator support
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
 */

#include "rtl_configall.h"

#define RINGQUEUE_HOPESIZE(itemsize, capacity) (sizeof(TiRingQueue) + (itemsize)*(capacity))

typedef struct{
	uintx front;
	uintx rear;
	uintx capacity;
	uintx count;
	uintx itemsize;
}TiRingQueue;

TiRingQueue *   rque_construct( void * buf, uintx size, uintx itemsize );
void		    rque_destroy( TiRingQueue * rque );
uintx           rque_count( TiRingQueue * rque );
uintx           rque_capacity( TiRingQueue * rque );
bool            rque_empty( TiRingQueue * rque );
bool            rque_full( TiRingQueue * rque );
void *          rque_getbuf( TiRingQueue * rque, uintx idx );
void *          rque_front( TiRingQueue * rque );
void *          rque_rear( TiRingQueue * rque );
bool            rque_pushback( TiRingQueue * rque, void * item );
bool            rque_pushfront( TiRingQueue * rque, void * item );
bool            rque_popfront( TiRingQueue * rque );
bool            rque_poprear( TiRingQueue * rque );

#endif
