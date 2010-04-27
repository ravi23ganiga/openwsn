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
	sio->rxque = iobuf_construct( &sio->quemem[0], sizeof(sio->quemem) );
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
	uintx available, count;
	char * pkt;
	uint8 i, len;
	TiIoBuf * tmp_iobuf;
	char * tmp_buf[ IOBUF_HOPESIZE(0x7F) ];
	tmp_iobuf = iobuf_construct( tmp_buf, sizeof(tmp_buf) );

	available = iobuf_available(sio->rxbuf);
	count = uart_read( sio->uart, iobuf_endptr(sio->rxbuf), available, 0x00 );

	if (count > 0)
	{
		iobuf_setlength( sio->rxbuf, iobuf_length(sio->rxbuf)+count );
	}
	/*dbo_putchar(0x11);
	dbo_putchar(count);
	uint8 len=iobuf_length(sio->rxbuf);
	if(len>0)
	{
		char * pkt=iobuf_data(sio->rxbuf);
		for(uint8 i=0;i<len;i++)
		{
			dbo_putchar(pkt[i]);
		}
	}*/

	// If uart_read() got some data and the sio->rxque is empty, then scan 
	// the frame flag bytes in the cache and perform frame spliting. If it founds an 
	// entire frame, then move this frame into sio->rxque
	//
	if (!iobuf_empty(sio->rxbuf) && (iobuf_empty(sio->rxque)))
	{
		// scan the data stream in "sio->rxbuf" for packet according to start and stop characters.
		// if found entire packet, then move it into "sio->rxque".
	
		count = tspl_rxhandle( sio->spliter, sio->rxbuf, tmp_iobuf );
		text_decode(tmp_iobuf, sio->rxque);

		if (count > 0)
		{
			iobuf_popfront( sio->rxbuf, count );
		}
	}

	// if the sio->rxque has an entire frame
	//
	if (!iobuf_empty(sio->rxque))
	{
		iobuf_copyto( sio->rxque, iobuf );
		iobuf_clear( sio->rxque );
		return iobuf_length( iobuf );
	}
	else
		return 0;
}


/* Write a packet into TiSioComm. The packet will be sent out through UART interface */
uint8 sio_write( TiSioComm * sio, TiIoBuf * iobuf )
{
	uint16 count=0, result=0;

	assert( !iobuf_empty(iobuf) );

	if (iobuf_empty(sio->txbuf))
	{
		result = text_encode( iobuf, sio->txbuf );
		result = tspl_txhandle( sio->spliter, sio->txbuf, sio->txbuf );
	}

	if (!iobuf_empty(sio->txbuf))
	{
		count = uart_write( sio->uart, iobuf_ptr(sio->txbuf), iobuf_length(sio->txbuf), 0x00 );
		if (count > 0)
		{
			iobuf_popfront(sio->txbuf, count);
		}
	}
	
	return result;
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
