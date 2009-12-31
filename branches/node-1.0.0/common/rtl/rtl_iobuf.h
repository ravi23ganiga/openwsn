/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
#ifndef _RTL_IOBUF_H_4662_
#define _RTL_IOBUF_H_4662_
/*******************************************************************************
 * TiIoBuf
 * an array based high performance simple input/output queue.
 *
 * @state
 * 	finished. still needs testing
 *
 * @modified by zhangwei on 200905xx
 *	- created
 *
 * @modified by YanShixing on 20091226
 *	- modified iobuf_write(), iobuf_puchback(). add popbyte()
 * @modified by zhangwei on 20091228
 *	- modified iobuf_popbyte
 *  - attention iobuf_popbuf is more efficient than iobuf_popbyte.
 * @modified by zhangwei on 20091229
 	- modified 	iobuf_popbyte and comment the iobuf_popbuf
 ******************************************************************************/

#undef CONFIG_DYNA_MEMORY

#include "rtl_configall.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "rtl_foundation.h"

#define min(x,y) (((x)<(y))?(x):(y))

/*******************************************************************************
 * TiIoBuf
 * an array based high performance simple input/output queue.
 *
 * there're three kinds of buffers provided by the system:
 *	- TiIoBuf
 *	- TiQueueBuffer
 *  	- TiFrameBuffer
 *
 * The IoBuffer is the most simple and most widely used one among the three.
 * it's quite similar to an array with some utility function enhancement.
 ******************************************************************************/

#define IOBUF_HOPESIZE(size) (sizeof(TiIoBuf)+size)

/* @attention
 * @warning:
 * currently, the TiIoBuf is configured to use 8 bit integers. So you must be very
 * caution that not to exceed the maximum limit. 
 * suggested buffer size <= 0xFF - sizeof(TiIoBuf), which is about 240.
 */
#define uintx uint8

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uintx memsize;
	uintx size;
	uintx length;
}TiIoBuf;

#ifdef CONFIG_DYNA_MEMORY
inline TiIoBuf * 	    iobuf_create( uintx size );
inline void 			iobuf_free( TiIoBuf * iobuf );
#endif

inline TiIoBuf * 	    iobuf_construct( void * iobuf, uintx memsize );
inline void 			iobuf_destroy( TiIoBuf * iobuf );

#ifdef CONFIG_DYNA_MEMORY
inline TiIoBuf * 	    iobuf_duplicate( TiIoBuf * iobuf );
#endif
inline char * 			iobuf_data( TiIoBuf * iobuf );
inline char * 			iobuf_ptr( TiIoBuf * iobuf );
inline char * 			iobuf_endptr( TiIoBuf * iobuf );
inline uintx 			iobuf_size( TiIoBuf * iobuf );
inline uintx 			iobuf_length( TiIoBuf * iobuf );
inline void 			iobuf_clear( TiIoBuf * iobuf );
inline bool 			iobuf_full( TiIoBuf * iobuf );
inline bool 			iobuf_empty( TiIoBuf * iobuf );
inline uintx            iobuf_available( TiIoBuf * iobuf );

inline uintx 			iobuf_read( TiIoBuf * iobuf, char * otherbuf, uintx len );
inline uintx 			iobuf_write( TiIoBuf * iobuf, char * data, uintx len );
inline uintx 			iobuf_pushback( TiIoBuf * iobuf, char * data, uintx len );
inline uintx 			iobuf_pushbyte( TiIoBuf * iobuf, unsigned char value );
inline uintx            iobuf_front( TiIoBuf * iobuf, char * otherbuf, uintx len );
inline void             iobuf_popfront( TiIoBuf * iobuf, uintx count );
inline bool             iobuf_set( TiIoBuf * iobuf, uintx idx, char c );
inline bool             iobuf_get( TiIoBuf * iobuf, uintx idx, char * c );
inline uintx 			iobuf_copyfrom( TiIoBuf * iobuf1, TiIoBuf * iobuf2 );
inline uintx 			iobuf_copyto( TiIoBuf * iobuf1, TiIoBuf * iobuf2 );

inline void 			iobuf_setlength( TiIoBuf * buf, uint8 count );

#ifdef CONFIG_DYNA_MEMORY
inline TiIoBuf * iobuf_create( uintx size )
{
	TiIoBuf * iobuf = (TiIoBuf *)malloc( IOBUF_HOPESIZE(size) );
	iobuf_construct( iobuf, size );
	return iobuf;
}
#endif

#ifdef CONFIG_DYNA_MEMORY
inline void iobuf_free( TiIoBuf * iobuf )
{
	iobuf_destroy( iobuf );
	free( iobuf );
}
#endif

inline TiIoBuf * iobuf_construct( void * mem, uintx memsize )
{
	TiIoBuf * iobuf = (TiIoBuf *)mem;
	iobuf->memsize = memsize;
	iobuf->size = memsize - sizeof(TiIoBuf);
	iobuf->length = 0;
	return iobuf;
}

inline void iobuf_destroy( TiIoBuf * iobuf )
{
	iobuf->length = 0;
}

#ifdef CONFIG_DYNA_MEMORY
inline TiIoBuf * iobuf_duplicate( TiIoBuf * iobuf )
{
	TiIoBuf * newbuf = iobuf_create(iobuf->size);
	memmove( (char*)newbuf + sizeof(TiIoBuf), (char*)iobuf + sizeof(TiIoBuf), iobuf->length );
    return newbuf;
}
#endif

inline char * iobuf_data( TiIoBuf * iobuf )
{
	if (iobuf->length > 0)
		return (char*)iobuf + sizeof(TiIoBuf);
	else
		return NULL;
}

inline char * iobuf_ptr( TiIoBuf * iobuf )
{
	return (char*)iobuf + sizeof(TiIoBuf);
}

inline char * iobuf_endptr( TiIoBuf * iobuf )
{
	return (char*)iobuf + sizeof(TiIoBuf) + iobuf->length;
}

inline uintx iobuf_size( TiIoBuf * iobuf )
{
    return iobuf->size;
}

inline uintx iobuf_length( TiIoBuf * iobuf )
{
	return iobuf->length;
}

inline void iobuf_clear( TiIoBuf * iobuf )
{
	iobuf->length = 0;
}

inline bool iobuf_full( TiIoBuf * iobuf )
{
	return (iobuf->length == iobuf->size);
}

inline bool iobuf_empty( TiIoBuf * iobuf )
{
	return (iobuf->length == 0);
}

inline uintx iobuf_available( TiIoBuf * iobuf )
{
	return (iobuf->size - iobuf->length);
}

inline uintx iobuf_read( TiIoBuf * iobuf, char * databuf, uintx size )
{
	uintx count = min( size, iobuf->length );
	memmove( databuf, iobuf_ptr(iobuf), count );
	return count;
}

inline uintx iobuf_write( TiIoBuf * iobuf, char * data, uintx len )
{
	uintx count = min( iobuf->size, len );
	memmove( iobuf_ptr(iobuf), data, count );
	iobuf->length = count;
	return count;
}

/* iobuf_pushback
 * append the data at the end of the original data. If there's not enough space,
 * then only the (size-length) characters will be padded.
 */
inline uintx iobuf_pushback( TiIoBuf * iobuf, char * data, uintx len )
{
	uintx count = min(iobuf->size - iobuf->length, len);
	memmove( iobuf_ptr(iobuf)+iobuf->length, data, count );
	iobuf->length += count;
	return count;
}

/* iobuf_pushbyte
 * append a single byte at the end of the original data
 */
inline uintx iobuf_pushbyte( TiIoBuf * iobuf, unsigned char value )
{
	char * ptr = iobuf_ptr(iobuf) + iobuf->length;
	*ptr = value;
	iobuf->length ++;
	return 1;
}

inline uintx iobuf_front( TiIoBuf * iobuf, char * otherbuf, uintx len )
{
	uintx count = min( len, iobuf->length );
	memmove( otherbuf, iobuf_ptr(iobuf), count );
	return count;
}

inline void iobuf_popfront( TiIoBuf * iobuf, uintx count )
{
	if (count < iobuf->length)
	{
		memmove( iobuf_ptr(iobuf), iobuf_ptr(iobuf)+count, iobuf->length - count );
		iobuf->length -= count;
	}
	else
		iobuf->length = 0;
}

/*inline uint8 iobuf_popbuf( TiIoBuf * iobuf, char * otherbuf, uintx size )
{
	uintx count = min( len, iobuf->length );
	if (count > 0)
	{
		memmove( otherbuf, iobuf_ptr(iobuf), count );
		memmove( iobuf_ptr(iobuf), iobuf_ptr(iobuf)+count, iobuf->length - count );
		iobuf->length -= count;
		return count;
	}
	else
		return 0;
}
*/

/* @error
 * iobuf_popbyte
 * it should be 
 * inline bool iobuf_popbyte( TiIoBuf * iobuf, char * pc );
 * 考虑一下,现有的做法
 * 当iobuf中没有数据时会输出什么值?
 * 0x00,但是如果0x00也是正常的data那就完蛋了
 * 所以要改成 inline bool iobuf_popbyte( TiIoBuf * iobuf, char * pc );
 * 这样用返回值区分是否执行成功
 * Yan:我的想法是调用这个函数前一定要求有一句if(!(iobuf_empty(iobuf)))
 * 这样的话下述函数成立。
 *
 */
inline uint8 iobuf_popbyte( TiIoBuf * iobuf, char * pc )
{
		char * ptr = iobuf_ptr(iobuf);
		*pc = *ptr;
		iobuf->length --;
		memmove( iobuf_ptr(iobuf), iobuf_ptr(iobuf)+1, iobuf->length );
		return 1;
}

inline bool iobuf_set( TiIoBuf * iobuf, uintx idx, char c )
{
	assert( idx < iobuf_size(iobuf) );
	if (idx < iobuf_size(iobuf))
	{
		iobuf_ptr(iobuf)[idx] = c;
		return true;
	}
	else
		return false;
}

inline bool iobuf_get( TiIoBuf * iobuf, uintx idx, char * c )
{
	assert( idx < iobuf_length(iobuf) );
	if (idx < iobuf_length(iobuf))
	{
		*c = iobuf_ptr(iobuf)[idx];
		return true;
	}
	else
		return false;
}

inline uintx iobuf_copyfrom( TiIoBuf * iobuf1, TiIoBuf * iobuf2 )
{
	uintx count = min( iobuf_size(iobuf1), iobuf_length(iobuf2) );
	memmove( iobuf_ptr(iobuf1), iobuf_data(iobuf2), count );
	iobuf1->length = count;
	return count;
}

inline uintx iobuf_copyto( TiIoBuf * iobuf1, TiIoBuf * iobuf2 )
{
	uintx count = min( iobuf_size(iobuf2), iobuf_length(iobuf1) );
	memmove( iobuf_ptr(iobuf2), iobuf_data(iobuf1), count );
	iobuf2->length = count;
	return count;
}

inline void	iobuf_setlength( TiIoBuf * buf, uintx count )
{
	buf->length = count;
}

#ifdef __cplusplus
}
#endif

#undef uintx

#endif

