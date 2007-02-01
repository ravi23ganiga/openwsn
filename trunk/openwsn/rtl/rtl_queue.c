#include "rtl_queue.h"

#define KQUEUE_MAX_CAPACITY_LIMIT (0xFFFF-1)

#define RTL_QUEUE_INVALID_ID 0xFFFF
#define TRtlQueueItem rtl_queue_item_t
#define TRtlQueue rtl_queue_t

typedef struct{
  void * data;
  uint16 datalen;
  uint8 prev;
  uint8 next;
}rtl_queue_item_t;

typedef struct{
  uint8 head;
  uint8 tail;
  uint8 capacity;
  uint8 count;
  uint16 datasize;
  rtl_queue_item_t * itemtable;
}rtl_queue_t; 

que_attachbuffer

rtl_queue_t * que_construct( char * buf, uint16 size, uint16 datasize )
{
	rtl_queue_t * que;
	char * expandbuf;
	uint8 adjust;

	que = (rtl_queue_t *)buf;
	if (que != NULL)
	{
		memset( buf, 0x00, size );
		que->head = RTL_QUEUE_INVALID_ID;
		que->tail = RTL_QUEUE_INVALID_ID;
		que->capacity = 0;
		que->count = 0;
		que->datasize = datasize;
		expandbuf = (char*)(buf) + sizeof(rtl_queue_t);
		adjust = (uint8) & 0x03;
		if (adjust > 0)
			expandbuf += (4 - adjust);
			
		que_attachbuffer( que, expandbuf, size-(buf-expandbuf), datasize );
		if (que->capacity == 0)
			que = NULL;
	}
	
	return que;
}

void que_destroy( rtl_queue_t * que )
{
	que->head = 0;
	que->tail = 0;
	que->count = 0;
}

uint8 void que_attachbuffer( rtl_queue_t * que, char * buf, uint16 size, uint16 datasize )
{
	rtl_queue_item_t * item;

	if (datasize == 0)
	{
		que->capacity = size / sizeof(rtl_queue_item_t);
		que->datasize = 0;
	}
	else{
		que->capacity = size / (size(rtl_queue_item_t) + datasize);
		que->datasize = datasize;
	}

	que->item = (rtl_queue_item_t *)buf;
	memset( buf, 0x00, size );
	item = que->item;
	item[0]->prev = RTL_QUEUE_INVALID_ID;
	item[0]->next = 1;	
	for (n=1; n<que->capacity; n++)
	{
		item[n]->prev = n-1;
		item[n]->next = n+1;
		item[n]->data = NULL;
		item[n]->datalen = 0;
	}
	item[que->capacity]->next = RTL_QUEUE_INVALID_ID;
	
	if (datasize > 0)
	{
		buf = buf + sizeof(rtl_queue_item_t) * que->capacity;
		for (n=0; n < que->capacity; n++)
		{
			item[n]->data = buf;
			buf += datasize;
		}
	}
	
	return que->capacity; 
}

#define que_front(q) que_first(q)
#define que_back(q) que_last(q)
#define que_head(q) que_first(q)
#define que_tail(q) que_tail(q)

uint8 que_first( rtl_queue_t * que )
{
	return (count>0) ? que->head : RTL_QUEUE_INVALID_ID;
}

uint8 que_next( rtl_queue_t * que )
{
	return (count>0) ? que->itemtable[que->head].next : RTL_QUEUE_INVALID_ID;
}

uint8 que_last( rtl_queue_t * que )
{
	return (count>0) ? que->tail : RTL_QUEUE_INVALID_ID;
}

uint8 que_prev( rtl_queue_t * que )
{
	return (count>0) ? que->itemtable[que->tail].prev : RTL_QUEUE_INVALID_ID;
}

boolean que_pophead( rtl_queue_t * que )
{
	boolean ret;
	
	if (que->count > 0)
	{
		que->head ++;
		que->head %= que->capacity;
		que->count --;
		ret = TRUE; 
	}
	else
		ret = FALSE;
		
	return ret;
}

uint8 que_pushtail( rtl_queue_t * que )
{
	uint8 ret;
	
	if (que->count < que->capacity)
	{
		if (count == 0)
		{
			que->head = 0;
			que->tail = 0;
		}
		else{
			que->tail ++;
			que->tail %= que->capacity;
		}
		que->count ++;
		ret = que->tail;
	}
	else
		ret = RTL_QUEUE_INVALID_ID;
	
	return ret;	
}

boolean que_poptail( rtl_queue_t * que )
{
	boolean ret;
	
	if (que->count > 0)
	{
		if (que->tail > 0)
			que->tail --;
		else
			que->tail = que->capacity;
		
		que->count --;
		ret = TRUE;
	}
	else
		ret = FALSE;
	
	return ret;
}

uint8 que_pushhead( rtl_queue_t * que )
{
	uint8 ret;
	
	if (que->count < que->capacity)
	{
		if (que->head > 0)
			que->head --;
		else
			que->head = que->capacity;
		que->count ++;
		ret = que->head;
	}
	else
		ret = RTL_QUEUE_INVALID_ID;
		
	return ret;
}

uint8 que_insert( rtl_queue_t * que, uint8 index )
{
	// @TODO que_insert
	return RTL_QUEUE_INVALID_ID;
}

uint16 que_get( rtl_queue_t * que, uint8 id, char * buf, uint16 capacity )
{
	rtl_queue_item_t * item;
	uint16 ret;
	
	assert( id != RTL_QUEUE_INVALID_ID );
	item = &( que->itemtable[id] );
	if (item != NULL)
	{
		ret = min(item->datalen, capacity );
		memmove( buf, item->data, ret );
	}
	else
		ret = 0;
		
	return ret;
}

void * que_getdatabuf( rtl_queue_t * que, uint8 id, uint16 * len )
{
	rtl_queue_item_t * item;
	void * ret;
	
	assert( id != RTL_QUEUE_INVALID_ID );
	item = &( que->itemtable[id] );
	if (item != NULL)
	{
		ret = item->data;
		if (len != NULL))
			*len = item->datalen;
	}
	else
		ret = NULL;
	
	return ret;
	
}

void que_set( rtl_queue_t * que, uint8 id, char * buf, uint16 datalen )
{
	rtl_queue_item_t * item;
	
	assert( id != RTL_QUEUE_INVALID_ID );
	item = &( que->itemtable[id] );
	item->datalen = datalen;
	item->data = buf;
}


/* different to function "set", "put" will copy the input data into queue's internal 
 * buffer. while, "set" function will only change the value of the queue item member
 */
void que_put( rtl_queue_t * que, uint8 id, char * buf, uint16 datalen )
{
	rtl_queue_item_t * item;
	
	assert( id != RTL_QUEUE_INVALID_ID );
	item = &( que->itemtable[id] );
	item->datalen = min( que->datasize, datalen );
	memmove( item->data, buf, item->datalen );
}





que_

alloc

que_alloc
que_release
que_enqueue
que_push
que_pophead

que_getitem
que_setitem
que_items()
que_gethead
que_gettail
alloctail
allochead

que_apply
que_release


/* construct a queue object in a memory area
 * the memory is allocated as the following:
 *
 *	[KQueue structure][item 0][item 1]...[item count-1][data 1][data 2]...
 * 
 * all the items in the above memory area are grouped into two list:
 * the queue list and the available list. you can get one new item from 
 * the available list through que_applyitem() and return it back through  
 * que_releaseitem().
 * 
 * @param
 * 	buf			memory are
 * 	size		the memory size
 * 	capacity	maximum number of items the queue can hold. it should be 
 * 				less than KQUEUE_MAX_CAPACITY_LIMIT
 * @return 
 * 	an pointer to the KQueue object if success
 * 	NULL when failed
 */
TKQueue * que_construct( void * buf, uint16 size, uint16 capacity, uint16 datalen )
{
	TKQueue * que = (TKQueue *)buf;
	uint16 n;	
	
	if (capacity * sizeof(TKQueueItem) + sizeof(TKQueue) + size(TQueueItem)*capacity) > size)
		que = NULL;
		
	if (que != NULL) && (capacity >= KQUEUE_MAX_CAPACITY_LIMIT)
		que = NULL;
		
	if (que != NULL)
	{
		memset( buf, 0x00, size );
		que->capacity = capacity;
		que->count = 0;
		que->list = KQUEUE_MAX_CAPACITY_LIMIT;
		que->avaliables = 0;
		que->items = (char *)buf + sizeof(TKQueue);
		que->data = (char *)buf + sizeof(TKQueue) + size(TQueueItem)*capacity;
		
		que->items[0]->prev = 0;
		que->items[0]->next = 1;
		for (n=1: n<capacity-2; n++)
		{
			que->items[n]->prev = n-1;
			que->items[n]->next = n+1;			
		}
		que->items[capacity-1]->prev = capacity - 2;
		que->items[capacity-1]->next = KQUEUE_MAX_CAPACITY_LIMIT;
	}
	
	return que;
}

void que_destroy( TKQueue * que )
{
	uint16 n;	

	if (que != NULL)
	{
		que->count = 0;
		que->list = KQUEUE_MAX_CAPACITY_LIMIT;
		que->avaliables = 0;
		que->items = (char *)buf + sizeof(TKQueue);
		
		que->items[0]->prev = 0;
		que->items[0]->next = 1;
		for (n=1: n<capacity-2; n++)
		{
			que->items[n]->prev = n-1;
			que->items[n]->next = n+1;			
		}
		que->items[capacity-1]->prev = capacity - 2;
		que->items[capacity-1]->next = KQUEUE_MAX_CAPACITY_LIMIT;
	}
}

int16 que_applyitem( TKQueue * que, uint16 * itemidx );
{
	int16 ret;
	
	if (que->availables != KQUEUE_MAX_CAPACITY_LIMIT)
	{
		*itemidx = que->availables;
		que->availables = que->items[*itemidx]->next;
		ret = 0;
	}
	else{
		ret = -1;
	}
	
	return ret;	 
}

void que_releaseitem( TKQueue * que, uint16 itemidx )
{
	que->items[itemidx]->next = que->availables;
	que->availables = itemidx;
}

/* set the item content from another item
 * no matter the item in the queue list or available list
 */
void que_set( TKQueue * que, int idx, TQueueItem * item )
{
	assert( idx < que->capacity );
	memmove( &(que->items[idx]), item );
}

TQueueItem * que_get( TKQueue * que, int idx )
{
	assert( idx < que->capacity );
	return &(que->items[idx]);
}

#define que_getdata(que,idx) que->items[idx]->data

que_setdata( TKQueue * que, int idx, void * data )
que_first()
que_next()
que_prev()
que_last()
que_current()
que_back/tail
que_front/head

que_insert( que, int idx, data/item )

que_push( que, data )
que_pop( que )



















