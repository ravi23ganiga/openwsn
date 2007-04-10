/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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

#include "..\hal\hal_cc2420.h"
#include "..\foundation.h"
#include "..\global.h"
#include "..\service\svc_debugio.h"
#include "analyzer.h"

#define MAX_BUFFER_SIZE 0xFF

/* 请站在host角度区分TX还是RX
 */
void analyzer_run( void )
{
	char txbuf[MAX_BUFFER_SIZE];
	char rxbuf[MAX_BUFFER_SIZE];
	char * buf;
	uint8 txlen, rxlen, count;

	global_construct();
	//sio_configure()
	//sio_relation( g_uart0 );
	//cc2420_configure
	//cc2420_relation( g_spi0 );
	
	memset( (char*)(&txbuf[0]), 0x00, MAX_BUFFER_SIZE ); 
	memset( (char*)(&rxbuf[0]), 0x00, MAX_BUFFER_SIZE ); 
	txlen = 0;
	rxlen = 0;
	
	// assume you have construct g_cc2420, g_sio, g_uart successfully
	//
	while (TRUE)
	{
		/*
		buf = (char*)(txbuf[0]) + txlen;
		count = sio_read( g_sio, buf, MAX_BUFFER_SIZE - txlen, 0 );
		txlen += count;
		
		buf = (char*)(txbuf[0]);
		count = cc2420_write( g_cc2420, buf, txlen, 0 );
		if (count > 0)
		{
			//txlen -= count;
			//memmove( buf, buf + count, txlen );
			txlen = 0;
		}
		*/

		buf = (char*)(rxbuf[0]) + rxlen;
		count = cc2420_rawread( g_cc2420, buf, rxlen, 0 );
		rxlen += count;
		
		buf = (char*)(rxbuf[0]);
		count = sio_write( g_sio, buf, rxlen, 0 );
		if (count > 0)
		{
			rxlen -= count;
			memmove( buf, buf + count, rxlen );
		}
	}
		
	global_destroy();	
	
	return;
}


