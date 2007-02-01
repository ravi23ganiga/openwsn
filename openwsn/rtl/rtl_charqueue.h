#ifndef _RTL_CHARQUEUE_H_1289_
#define _RTL_CHARQUEUE_H_1289_

#include "rtl_configall.h"

#define KCHARQUEUE_MAX_CAPACITY_LIMIT (0xFFFF-1)
#define kcharqueue_t TKCharQueue

typedef struct{
  uint16 count;
  uint16 capacity;
  uint16 head;
  uint16 tail;
  char * datatable;
}TKCharQueue; 


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
TKCharQueue * cque_construct( void * buf, uint16 size, uint16 capacity );
void cque_destroy( TKCharQueue * que );

#define cque_isempty(que) (que->count == 0)
#define cque_isfull(que) (que->count >= que->capacity)

boolean cque_push( TKCharQueue * que, char ch );
boolean cque_pop( TKCharQueue * que, char * ch );
boolean cque_feed( TKCharQueue * que, char ch );

#endif







