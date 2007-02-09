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

/*****************************************************************************
 * @author zhangwei on 20070115
 *  first created, determined the interface.
 ****************************************************************************/ 

/*****************************************************************************
 * The key idea of OpenNET
 * the running of OpenNET needs the support from libsink. libsink will periodically
 * send packets to maintain the network routing, thus the developing of OpenNET 
 * on sensor node can be more simple.
 * 
 * there're 3 type packets in OpenNET
 * ADV		broadcast to the whole network by libsink
 * 			from sink node to all the other nodes
 * 			usually the command packet.
 * 			different to QUERY type, ADV packets are transported by broadcasting.
 * QUERY 	similar to ADV type. but it has a PATH indicator in the packet.
 * 			the path indicator indicate the approximate query range of the 
 * 			QUERY. for example, 2 hops near node 3. the path indicator can also 
 * 			do help to its routing.
 * 			may contains other query criterions
 * DATA		data type packet. it has a PATH indicator in the packet.
 * 
 * for every node
 * 1. check the received packet
 * 2. if the packet is QUERY packet, the check whether itself is the destination.
 * 	 2.1 if the current node is the destination, then interpret the packet and 
 * 		sends the DATA packet back.
 *   2.2 if the current node is not the destination, then check local buffer. 
 * 			if the packet has been received in the past, then drop it
 * 			else forward the packet to neighbors
 * 3. if the packet is DATA packet, then check local buffer. if not exists, then
 * 	forward it to the next hop according to the path indicator in the packet.  
 * 4. if the packet is ADV packet, and the packet not in local buffer, 
 * 	then broadcast the packet to its neighbors.
 * 
 *****************************************************************************
 * NET layer packet format:
 * 
 * for DATA packet:
 * [1B packet control] [nB Path Indicator[1B Path Length][(n-1)B path] [mB data]
 * 		[mB data] usually is [1B sensor type][(m-1)B sensor data]
 * 
 * for QUERY packet:
 * [1B packet control] [nB Path Indicator[1B Path Length][(n-1)B path] 
 * 		[1B query command][mB query parameter]
 * 
 * for ADV packet:
 * [1B packet control] [nB Path Indicator[1B Path Length][(n-1)B path] 
 * 		[1B command][mB command parameter]
 * 
 * the packet control byte
 * b7 b6.... b0
 * 
 * b7 b6  	packet type
 * 			00 DATA		(from sensor to sink, not broadcast)
 * 			01 QUERY	(from sink to sensor, not broadcast)
 * 			02 ADV		(from sink to sensor, broadcast)
 * 			03 (reserved, maybe from sensor to sensor/sink and broadcast)
 * b5 = 0	(reserved)
 * b4 = 1   using path indicator
 * b3 b2 b1 b0 
 * 			TTL, maximum hop this packet can endure
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

TOpenNET * net_construct( char * buf, uint16 size )
{
	assert( sizeof(TOpenNET) <= size );
	memset( buf, 0x00, size );
	return (TOpenNET *)buf;
}

void net_destroy( TOpenNET * net )
{
	return;
}

void net_init( TOpenNET * net, TOpenMAC * mac, TActionScheduler * actsche )
{
	net->mac = mac;
}

void net_configure( TOpenNET * net, uint8 ctrlcode, uint8 value )
{
}

int8 net_setlocaladdress( TOpenNET * net, uint16 pan, uint16 nodeid )
{
	//mac_setlocaladdress( );
	return 0;
}

void net_getrmtaddress( TOpenNET * net, uint16 * pan, uint16 * nodeid )
{
	*pan = net->panid;
	*nodeid = net->nodeid;
}

int8 net_read( TOpenNET * net, TOpenFrame * frame, uint8 size, uint8 opt )
{
	return mac_read( net->mac, frame, size, opt );
}

int8 net_rawread( TOpenNET * net, char * framebuf, uint8 size, uint8 opt )
{
	return mac_rawread( net->mac, framebuf, size, opt );
}

int8 net_write( TOpenNET * net, TOpenFrame * frame, uint8 len, uint8 opt )
{
	return mac_write( net->mac, frame, len, opt );
}

int8 net_rawwrite( TOpenNET * net, char * framebuf, uint8 len, uint8 opt )
{
	return mac_rawread( net->mac, framebuf, len, opt );
}

int8 net_forward( TOpenNET * net, TOpenFrame * frame, uint8 len, uint8 opt )
{
	//return mac_forward( net->mac, frame, size, opt );
	return 0;
}

int8 net_rawforward( TOpenNET * net, char * framebuf, uint8 size, uint8 opt )
{
	return 0;
}

int8 net_evolve( TOpenNET * net )
{
	mac_evolve( net->mac );
	return 0;
}

int8 net_sleep( TOpenNET * net )
{
	mac_sleep( net->mac );
	return 0;
}

int8 net_wakeup( TOpenNET * net )
{
	mac_wakeup( net->mac );
	return 0;
}

int8 net_installnotify( TOpenNET * net, TEventHandler * callback, void * data )
{
	return 0;
}


