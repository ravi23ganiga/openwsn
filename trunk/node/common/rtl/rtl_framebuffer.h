//---------------------------------------------------------------------------
// rtl_framebuffer
//---------------------------------------------------------------------------

#ifndef rtl_framebufferH
#define rtl_framebufferH
//---------------------------------------------------------------------------

#include "rtl_configall.h"
#include "rtl_foundation.h"

#ifdef __cplusplus
extern "C" {
#endif



//---------------------------------------------------------------------------
// TxFrameBuffer
// to help manipluate frame generation and analysis. provide buffer management
// when add frame header and frame tail or vice versa.
//---------------------------------------------------------------------------
/*
class TxFrameBuffer{
private:
	char * m_buf;
	uint m_capacity;
	uint m_start;
	uint m_len;
public:
	TxFrameBuffer( uint capacity, uint skiplen=0 )
	{
		m_buf = new char[capacity];
		m_capacity = capacity;
		m_start = skiplen;
		m_len = 0;
	}

	~TxFrameBuffer()
	{
		if (m_buf)
		{
			delete m_buf;
			m_buf = NULL;
		}
	}

	void setskiplen( uint skiplen ) {m_start = skiplen;};

	// read the data out from the internal buffer. the results will be place into
	// the buffer indicated by pointer "buf"
	// @param
	//	buf:  	the memory block to receive the data
	//	capacity: the maximum bytes can be put into the buf
	// @return
	//  how many bytes put into the buf succesfully
	//
	uint read( char * buf, uint capacity )
	{
		uint count = min(capacity, m_len);
		if (count > 0)
		{
			memmove( buf, m_buf+m_start, count );
			m_len -= count;
		}
		return count;
	}

	// write the data into internal buffer
	// @param
	//	data:  	where the data is put
	//	len:	data length
	// @result
	//	how many bytes successfully write to the internal buffer
	//
	uint write( char * data, uint len )
	{
		uint count = min(m_capacity - m_start, len);
		if (count > 0)
		{
			memmove( m_buf+m_start, data, count );
			m_len += count;
		}
		return count;
	}

	// in the version, the space before the body part of the frame must be larger
	// than required
	//
	uint addheader( char * data, uint len )
	{
		assert( m_start >= len );
		m_start -= len;
		memmove( m_buf + m_start, data, len );
		return len;
	}

	uint appendtail( char * data, uint len )
	{
		uint count = min(m_capacity - m_start - m_len, len);
		if (count > 0)
		{
			memmove( data+m_start+m_len, data, count );
			m_len += count;
		}
		return count;
	}

	void removeheader( uint hdrlen )
	{
		assert( m_len >= hdrlen );
		m_start += hdrlen;
	}

	void removeheader( char * buf, unit hdrlen )
	{
		assert( m_len >= hdrlen );
		memmove( buf, m_buf + m_start, hdrlen );
    }

	void removetail( uint taillen )
	{
		assert( m_len >= taillen );
		m_len -= taillen;
	}

	void removetail( char * buf, uint taillen )
	{
		assert( m_len >= taillen );
		memmove(
	}

	char * buffer() {return m_buf;};
	uint capacity() {return m_capacity;}
	char * data() {return m_buf + m_start;}
	uint datalen() {return m_len;}

	uint moveto( TxFrameBuffer * fbuf, uint skiplen=0 )
	{
		fbuf->setskiplen( skiplen );
		fbuf->write( this->data(), this->datalen() );
	};
};
*/

#ifdef __cplusplus
}
#endif


#endif
