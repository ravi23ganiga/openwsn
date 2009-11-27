
#include "rtl_iobuf.h"

TiIoBuffer * buf_construct( void * mem, uintx size )
{
	TiIoBuffer * buf = (TiIoBuffer *)mem;
	assert( size > sizeof(TiIoBuffer) );

	buf->size -= sizeof(TiIoBuffer);
	buf->skiplen = 0;
	buf->len = 0;
	buf->index = 0;
	return buf;
}

void buf_destroy( TiIoBuffer * buf )
{
	NULL;
}

uintx buf_read( TiIoBuffer * buf, char * mem, uintx size )
{
	uintx len;

	assert( buf->skiplen + buf->len >= buf->index );
	len = min( buf->skiplen + buf->len - buf->index, size );
	if (len > 0)
	{
		memmove( mem, (char*)buf_data(iobuf) + buf->index, len );
		buf->index += len;
	}
	return len;
}

uintx buf_write( TiIoBuffer * buf, char * mem, uintx len )
{
	len = min( buf->size - buf->index, len );
	if (len > 0)
	{
		memmove( (char*)buf_data(buf) + buf->index, mem, len );
		buf->index += len;
		if (buf->index > buf->skiplen + buf->len)
			buf->len = buf->index - buf->skiplen;
	}
	return len;
}

uintx buf_append( TiIoBuffer * buf, char * mem, uintx len )
{
	uintx avail;

	len = min( buf->size - (buf->skiplen - buf->len), len );
	if (len > 0)
	{
		memmove( buf_tail(iobuf), mem, len );
		buf->len += len;
	}
	return len;
}

void buf_replace( TiIoBuffer * buf, uintx start, char * newdata, uintx len )
{
	char * data = buf_header(buf);
	assert( start+len <= buf->size );
	memmove( data+start, newdata, len );
}

uintx buf_setskiplen( TiIoBuffer * buf, uintx value )
{
	intx delta = value - buf->skiplen;
	buf->skiplen = value;
	buf->len -= delta;
}

uintx buf_clear( TiIoBuffer * buf )
{
	buf->skiplen = 0;
	buf->len = 0;
	buf->index = 0;
}


