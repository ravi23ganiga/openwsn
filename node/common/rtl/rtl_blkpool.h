/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 
#ifndef _RTL_BLOCKPOOL_H_7681_
#define _RTL_BLOCKPOOL_H_7681_

/*****************************************************************************
 * Q: what's the difference between TBlockPool and TBlockAllocator? [20081224]
 * R: They two are essentially the same. However, TBlockPool returns the block
 *	with an id, and TBlockAllocator return the block with the memory pointer. 
 *  if you use TBlockPool, you need to get the memory pointer through 
 *  blkpool_address(id) function. 
 *	  With the block id, the object can move in the memory space. Furthermore, 
 *  "id" is usually smaller than pointer in 32 bit CPU, which can save memory 
 * space and still keep high efficiency. 
 *	  By using TBlockPool, we can greatly decrease the memory copy between different
 * layers[reference: test/dblrouter.c]. So the author suggest to use TBlockPool
 * when possible.   
 ****************************************************************************/ 


#define blockpool_t TBlockPool 

/* this structure should be QWORD alignment to improve efficiency for 32 bit 
 * computers
 */
typedef struct{
  uint16 next;
  uint16 index;
}TiBlockPoolItem;

/* this structure should be QWORD alignment to improve efficiency for 32 bit 
 * computers
 */
typedef struct{
  uint16 emptylist;
  uint16 activelist;
  TiBlockPoolItem * blocktable;
  char * blockdata;
  uint16 chunksize
  uint16 blocksize;
  uint16 blockcapacity;
  uint16 blockcount;
}TBlockPool;  


blkpool_create( blksize, blkcount )
blkpool_free
blkpool_construct
blkpool_destroy
blkpool_apply
blkpool_release
blkpool_count
blkpool_item[idx]
blkpool_address(id)

#endif /*_RTL_BLOCKPOOL_H_7681_*/
