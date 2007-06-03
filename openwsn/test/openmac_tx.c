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


#include "..\hal\hal_foundation.h"
#include "..\hal\hal_uart.h"
#include "..\hal\hal_timer.h"
#include "..\hal\hal_global.h"
#include "..\foundation.h"
#include "..\global.h"
#include "openmac_tx.h"


static TOpenMAC m_mac;
static TOpenMAC * _mac;

void openmac_tx( void )
{
	char txbuf[OPENMAC_BUFFER_SIZE];
	char rxbuf[OPENMAC_BUFFER_SIZE];
	char * buf;
	int8 txlen, rxlen;

	target_init();
	global_construct();
	_mac = mac_construct( (char*)(&m_mac), sizeof(m_mac) );
	//mac_init( _mac, g_cc2420, g_timer1 );
	timer_init( g_timer1, 1, 2 );

	// 在这里初始化一个OpenFrame
	
	memset( (char*)(&txbuf[0]), 0x00, OPENMAC_BUFFER_SIZE ); 
	memset( (char*)(&rxbuf[0]), 0x00, OPENMAC_BUFFER_SIZE ); 
	txlen = 0;
	rxlen = 0;
	
	// you can change the node address here according to your own testing
	// environment.
	//
	buf = (char*)(txbuf[0]);
	opf_setaddrfrom( txbuf, 3 );
	opf_setaddrto( txbuf, 1 );
	
	while (1)
	{
		timer_stop( g_timer1 );
		timer_setinterval( g_timer1, 500, 0 );
		timer_start( g_timer1 );
		
		while (!timer_expired(g_timer1))
		{
			mac_rawwrite( _mac, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
	}
		
	mac_destroy( _mac );
	global_destroy();	
	return;
}
