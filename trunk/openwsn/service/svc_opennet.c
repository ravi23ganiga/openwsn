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
#include "svc_foundation.h"
#include "svc_opennet.h"

typedef struct{
  uint16 dest;
  uint16 next;
  uint8 metric;
  uint8 quality;
}TRouteTableItem;

TRouteTableItem g_routetable[ NET_ROUTE_TABLE_SIZE ];
TOpenNET g_opennet;

int8 net_construct( TOpenMAC * net, TActionScheduler * actsche )
{
	return 0;
}

void net_destroy()
{
	return;
}

void net_configure( uint8 ctrlcode, uint8 value )
{
	return;
}

int8 net_setaddress( char * addr, uint8 len )
{
	return 0;
}

int8 net_getaddress( char * addr, uint8 capacity )
{
	return 0;
}

int8 net_read( TOpenPacket * pkt, uint8 opt )
{
	return 0;
}

int8 net_rawread( char * frame, uint8 size, uint8 opt )
{
	return 0;
}

int8 net_write( TOpenPacket * pkt, uint8 opt )
{
	return 0;
}

int8 net_rawwrite( char * frame, uint8 size, uint8 opt )
{
	return 0;
}

int8 net_forward( TOpenPacket * pkt, uint8 opt )
{
	return 0;
}

int8 net_rawforward( char * frame, uint8 size, uint8 opt )
{
	return 0;
}

int8 net_evolve( TOpenNET * net )
{
	return 0;
}

int8 net_sleep( TOpenNET * net )
{
	return 0;
}

int8 net_wakeup( TOpenNET * net )
{
	return 0;
}

int8 net_installnotify( TOpenNET * net, TEventHandler * callback, void * data )
{
	return 0;
}

