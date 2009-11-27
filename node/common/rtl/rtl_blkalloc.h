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
#ifndef _RTL_BLKPOOL_8976_
#define _RTL_BLKPOOL_8976_

#define blockpool_t TBlockPool 

/* this structure should be QWORD alignment to improve efficiency for 32 bit 
 * computers, or DWORD alignment for 16 bit MCUs
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


#endif /*_RTL_BLKPOOL_8976_*/
