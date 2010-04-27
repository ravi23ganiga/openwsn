/* @modified by zhangwei on 2010.02.19
 * 	- bug fix: correct length setting error in function iobuf_pushback()
 * @modified by zhangwei on 2010.02.20
 * 	- bug fix: correct behavior of iobuf_append()
 *  - add: iobuf_adjustlength()
 *
 */
#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_assert.h"
#include "rtl_iobuf.h"

#ifdef CONFIG_DYNA_MEMORY
TiIoBuf * iobuf_create( uintx size )
{
	TiIoBuf * iobuf = (TiIoBuf *)malloc( IOBUF_HOPESIZE(size) );
	iobuf_construct( iobuf, size );
	return iobuf;
}
#endif

#ifdef CONFIG_DYNA_MEMORY
void iobuf_free( TiIoBuf * iobuf )
{
	iobuf_destroy( iobuf );
	free( iobuf );
}
#endif

TiIoBuf * iobuf_construct( void * mem, uintx memsize )
{
	TiIoBuf * iobuf = (TiIoBuf *)mem;
	iobuf->memsize = memsize;
	iobuf->size = memsize - sizeof(TiIoBuf);
	iobuf->length = 0;
	return iobuf;
}

void iobuf_destroy( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );
	iobuf->length = 0;
}

#ifdef CONFIG_DYNA_MEMORY
inline TiIoBuf * iobuf_duplicate( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );
	TiIoBuf * newbuf = iobuf_create(iobuf->size);
	iobuf_copyfrom( newbuf, iobuf );
    return newbuf;
}
#endif

inline char * iobuf_data( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );

	if (iobuf->length > 0)
		return (char*)iobuf + sizeof(TiIoBuf);
	else
		return NULL;
}

inline uintx iobuf_length( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );
	return iobuf->length;
}

inline uintx iobuf_size( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );
	return iobuf->size;
}

inline char * iobuf_ptr( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );
	return (char*)iobuf + sizeof(TiIoBuf);
}

inline char * iobuf_endptr( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );
	return iobuf_ptr(iobuf) + iobuf_length(iobuf);
}

inline void iobuf_clear( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );
	iobuf->length = 0;
}

inline bool iobuf_full( TiIoBuf * iobuf )
{
	rtl_assert( iobuf != NULL );
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
	memmove( iobuf_ptr(iobuf)+iobuf->length, data, count );
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

uintx iobuf_movefrom( TiIoBuf * iobuf1, TiIoBuf * iobuf2 )
{
	uintx count = min( iobuf_size(iobuf1), iobuf_length(iobuf2) );
	memmove( iobuf_ptr(iobuf1), iobuf_data(iobuf2), count );
	iobuf1->length = count;
	iobuf_popfront(iobuf2, count);
	return count;

}

uintx iobuf_moveto( TiIoBuf * iobuf1, TiIoBuf * iobuf2 )
{
	uintx count = min( iobuf_size(iobuf2), iobuf_length(iobuf1) );
	memmove( iobuf_ptr(iobuf2), iobuf_data(iobuf1), count );
	iobuf2->length = count;
	iobuf_popfront(iobuf1, count);
	return count;
}

// append iobuf2 to the end of iobuf1. it's quite similar to iobuf_pushback
uintx iobuf_append( TiIoBuf * iobuf1, TiIoBuf * iobuf2 )
{
	uintx avail, len, count=0;

	rtl_assert( (iobuf1 != NULL) && (iobuf2 != NULL) );
	avail = iobuf_available(iobuf1);
	len = iobuf_length(iobuf2);
	count = min(avail, len);
	if (count > 0)
	{
		memmove( iobuf_endptr(iobuf1), iobuf_ptr(iobuf2), count );
		iobuf_adjustlength( iobuf1, count );
	}
	return count;
}

inline void	iobuf_setlength( TiIoBuf * buf, uintx count )
{
	buf->length = count;
}

void iobuf_adjustlength( TiIoBuf * buf, int delta )
{
	buf->length += delta;
}

#ifdef CONFIG_DEBUG
void iobuf_dump( TiIoBuf * buf )
{
	int i;
	char * pc;
	printf("dump iobuf: memsize=%d, size=%d, length=%d\n", buf->memsize, buf->size, buf->length );
	if (buf->length > 0)
	{
		putchar('=');
		putchar('>');
		pc = iobuf_ptr(buf);
		for (i=0; i<buf->length; i++)
			putchar(pc[i]);
		putchar('\n');
	}
}
#endif
