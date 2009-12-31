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

/* TiSioComm service
 *
 * @created by zhangwei on 20091226
 * @modified by Yan Shixing on 20091228
 */

#include "../hal/hal_debugio.h"
#include "../rtl/rtl_iobuf.h"
#include "svc_siocomm.h"
#include "../rtl/rtl_textcodec.h"


/* Construct an TiSioComm service in the specified memory block */
TiSioComm * sio_construct( char * buf, uint16 size )
{
	hal_assert( sizeof(TiSioComm) <= size );
	memset( buf, 0x00, size );
	return (TiSioComm *)buf;
}

/* Destroy an TiSioComm service */
void sio_destroy( TiSioComm * sio )
{
	sio_close( sio );
}
/* Open an TiSioComm service */
TiSioComm * sio_open( TiSioComm * sio, TiUartAdapter * uart, TiTextSpliter * spliter, uint8 opt )
{
	sio->uart = uart;
	sio->option = opt;
	sio->spliter = spliter;
	sio->txbuf = iobuf_construct( &sio->txmem[0], sizeof(sio->txmem) );
	sio->rxbuf = iobuf_construct( &sio->rxmem[0], sizeof(sio->rxmem) );
	return sio;

}

TiSioComm * sio_close( TiSioComm * sio )
{
	return NULL;
}

/* Configure an TiSioComm service 
 * 
 * @todo
 * in the future, we'd better use TiSvcTimer instead of TiTimerAdapter now.
 */
 
void sio_configure( TiSioComm * sio, TiTimerAdapter * timer, uint8 opt )
{
	// sio->timer = timer;
	// sio->option = option;
}



/* Read a packet from TiSioComm service. */
uint8 sio_read( TiSioComm * sio, TiIoBuf * iobuf )
{
	

	uint8 count;
	char tempbuf[SIO_RXBUFFER_CAPACITY];
	// If the internal byte buffer still has some empty space, then call uart_read()
	// to read data and put them into the byte buffer. 
	//貌似没有必要，因为这个byte buffer在textspliter中已经做了。
	//available = iobuf_available(sio->rxbuf);
	count = uart_read( sio->uart, tempbuf, SIO_RXBUFFER_CAPACITY, 0x00 );

	// If uart_read() got some data and the sio->rxque is empty, then scan 
	// the frame flag bytes in the cache and perform frame spliting. If it founds an 
	// entire frame, then move this frame into sio->rxbuf
	//
	if ((count > 0) && (iobuf_empty(sio->rxbuf)) )
	{
		//scan packet start and stop characters;
		count = tspl_read( sio->spliter, tempbuf, count, 
			tempbuf, SIO_RXBUFFER_CAPACITY );
		if (count > 0)
		{
			count = text_decode( tempbuf, count, iobuf_ptr(sio->rxbuf), 
				iobuf_size(sio->rxbuf) );
			iobuf_setlength( sio->rxbuf, count );
		}
	}

	// if the sio->rxque has an entire frame
	//
	if (!iobuf_empty(sio->rxbuf))
	{
		iobuf_copyto( sio->rxbuf, iobuf );
		iobuf_clear( sio->rxbuf );
		iobuf_setlength( iobuf, count );
		return iobuf_length( iobuf );
	}
	else
		return 0;

}


/* Write a packet into TiSioComm. The packet will be sent out through UART interface */
uint8 sio_write( TiSioComm * sio, TiIoBuf * iobuf )
{
	/*uint16 count=0;

	if (iobuf_empty(sio->txbuf))
	{
		iobuf_pushbyte( sio->txbuf, SIO_START );
		count = text_encode( iobuf_ptr(iobuf), iobuf_length(iobuf), iobuf_endptr(sio->txbuf), 
			iobuf_available(sio->txbuf) );
		iobuf_setlength( sio->txbuf, ++count );
		iobuf_pushbyte( sio->txbuf, SIO_END );
		result = iobuf_length(iobuf);
	}

	if (!iobuf_empty(sio->txbuf)
	{
		count = uart_write( sio->uart, iobuf_ptr(sio->txbuf), iobuf_length(sio->txbuf) );
		if (count > 0)
		{
			iobuf_popfront(count);
		}
	}

	return result;
*/
	uint16 count = 0;
	uint16 result = 0;
	
	char tempbuf[SIO_TXBUFFER_CAPACITY];

	count = text_encode( iobuf_ptr(iobuf), iobuf_length(iobuf), 
		tempbuf, SIO_RXBUFFER_CAPACITY );
	
	iobuf_pushbyte( sio->txbuf, SIO_START );

	iobuf_pushback( sio->txbuf, tempbuf, count );

	iobuf_pushbyte( sio->txbuf, SIO_END );

	result = iobuf_length(iobuf);


	if (!iobuf_empty(sio->txbuf))
	{
		count = uart_write( sio->uart, iobuf_ptr(sio->txbuf), iobuf_length(sio->txbuf), 0x00 );
		if (count > 0)
		{
			iobuf_popfront(sio->txbuf, count);
		}
	}

	return result;

/*	if (sio->txbuf has enough free spaces)
	{
		put an SIO_START byte into sio->txbuf;
		put the data inside iobuf into sio->txbuf;
		put an SIO_STOP byte into sio->txbuf;
	}

	if (sio->txbuf has data)
	{
		try send the data inside txbuf;
		count = uart_write( sio->uart, sio->txbuf, sio->txlen );
		if (count > 0)
		{
			delete the first count bytes inside txbuf because they have been sent.
			and keep the left bytes still in the txbuf
		}
	}
	*/

	//return count;
	

}
/* Evolve the TiSioComm object */
void sio_evolve( TiSioComm * sio, TiEvent * e )
{
	/*
	uint8 count;

	if (!iobuf_empty(sio->txbuf)
	{
		count = uart_write( sio->uart, iobuf_ptr(sio->txbuf), iobuf_length(sio->txbuf) );
		if (count > 0)
		{
			iobuf_popfront(count);
		}
	}
	*/
}
