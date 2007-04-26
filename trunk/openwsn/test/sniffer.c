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

#include "..\foundation.h"
#include "..\hal\hal_cc2420.h"
#include "..\service\svc_debugio.h"
#include "..\global.h"
#include "sniffer.h"

/*****************************************************************************
 * @author zhangwei on 20070423
 * revision today.
 * 
 * the "cc2420" chip has address identification mechanism built. if you want to 
 * use it as a sniffer frontier, you must turn off it. 
 * 
 ****************************************************************************/ 

#define MAX_BUFFER_SIZE 0xFF

void sniffer_run( void )
{
	char rxbuf[MAX_BUFFER_SIZE];
	char * buf;
	uint8 rxlen, count;

	global_construct();
	//sio_configure()
	//sio_relation( g_uart0 );
	//cc2420_configure
	// 设置2420可以接受任何的数据包
	// hardware address recognition can be enabled or disabled using 
	// MDMCTRL0.ADR_DECODE bit (p.39, cc2420 datasheet) 
	//
	//cc2420_relation( g_spi0 );
	//cc2420_init()
	
	memset( (char*)(&rxbuf[0]), 0x00, MAX_BUFFER_SIZE ); 
	rxlen = 0;
	
	// assume you have construct g_cc2420, g_sio, g_uart successfully now
	//
	while (TRUE)
	{
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
