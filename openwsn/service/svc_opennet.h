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
#ifndef _OPENNET_H_4729_
#define _OPENNET_H_4729_

/******************************************************************************
 * @author zhangwei on 2006-12-20
 * @name OpenNET
 * 	this is the implementation of routing protocol for OpenM2M/OpenWSN system.
 * 
 * 	the windows/unix programmars are quite familar with the "socket" network 
 * interface. the "socket" is a stream-based network programming interface. 
 * it is more appropriate for fixed network than mobile/mesh networks. so in 
 * OpenNET design, we adopt "packet" interface rather than "stream-like" interface.
 * 
 * @attention:
 * 	OpenNET support multiple sink nodes and mobile sink nodes. it cannot running
 * indenpendently. It must be run with the "sinksvc" on the sink host. without 
 * the data collection sink, without routing.
 *  
 * @history
 * @modified by zhangwei on 2006-12-20
 * 	first created
 * 
 *****************************************************************************/
  
#include "svc_foundation.h"
#include ".\hal\hal_timer.h"
#include ".\hal\hal_uniqueid.h"
#include "svc_openmac.h"

#define NET_MAX_PAYLOAD_LENGTH 128
#define NET_ROUTE_TABLE_SIZE 16

typedef uint32 TOpenAddress;
  
/* TOpenPacket
 * it represents the packet transmitted on the network layer. 
 * it's structure is visible for upper application developers.
 */
typedef struct{
  uint16 control;
  uint8 seqid;
  uint16 netaddr;
  uint16 destaddr;
  uint16 srcaddr;
  uint8 datalen;
  char data[NET_MAX_PAYLOAD_LENGTH];
}TOpenPacket;

typedef struct{
  uint8 state;
  TOpenMAC * mac;
  uint8 seqid;
  char localaddr[8];
  uint8 addrlen;
  TEventHandler * callback;
}TOpenNET;

int8 net_construct( TOpenMAC * net, TActionScheduler * actsche );
void net_destroy();
void net_configure( uint8 ctrlcode, uint8 value );
int8 net_setaddress( char * addr, uint8 len );
int8 net_getaddress( char * addr, uint8 capacity );
int8 net_read( TOpenPacket * pkt, uint8 opt );
int8 net_rawread( char * frame, uint8 size, uint8 opt );
int8 net_write( TOpenPacket * pkt, uint8 opt );
int8 net_rawwrite( char * frame, uint8 size, uint8 opt );
int8 net_forward( TOpenPacket * pkt, uint8 opt );
int8 net_rawforward( char * frame, uint8 size, uint8 opt );
int8 net_evolve( TOpenNET * net );

int8 net_sleep( TOpenNET * net );
int8 net_wakeup( TOpenNET * net );
int8 net_installnotify( TOpenNET * net, TEventHandler * callback, void * data );

#endif


