#ifndef _TK_QUEUE_H_
#define _TK_QUEUE_H_


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

#endif /*_TK_QUEUE_H_*/
