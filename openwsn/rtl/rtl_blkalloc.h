#ifndef _TK_BLKALLOC_H_
#define _TK_BLKALLOC_H_

#define rtl_block_allocator_t TBlockAllocator 

/* this structure should be QWORD alignment to improve efficiency for 32 bit 
 * computers
 */
typedef struct{
  uint16 next;
  uint16 index;
}TBlockAllocatorItem;

/* this structure should be QWORD alignment to improve efficiency for 32 bit 
 * computers
 */
typedef struct{
  uint16 emptylist;
  uint16 activelist;
  TBlockAllocatorItem * blocktable;
  char * blockdata;
  uint16 chunksize
  uint16 blocksize;
  uint16 blockcapacity;
  uint16 blockcount;
}TBlockAllocator;  


#endif /*_TK_BLKALLOC_H_*/
