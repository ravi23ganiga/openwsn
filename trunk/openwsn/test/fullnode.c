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

#include "..\foundation.h"
#include "..\service\svc.h"
#include "..\global.h"

static TOpenFrame g_txframe;
static TOpenFrame g_rxframe;
static char * g_txframebuf;
static char * g_rxframebuf;
static char * g_txpktbuf;
static char * g_rxpktbuf;

// the first WORD is the PAN id, the second WORD is the node id
// they two forms a standard 802.15.4 short address.
// you can freely changing this setting according to your own requirements.
//
static uint16 _localaddr[2] = {0x01, 0x05};
static void gnode_execute( void );
static void sinknode_execute( void ); 
static int8 gnode_interpret( TOpenFrame * rxframe, TOpenFrame * txframe );


void fullnode_test( void )
{
	#ifdef CONFIG_GENERALNODE
	gnode_execute();
	#endif
	
	#ifdef CONFIG_SINKNODE
	sinknode_execute();
	#endif
	
	return;
}

// for generic node in sensor network, the main program is as the following.
// if it received a packet from other node, it will forward it to its next hop.
//
void gnode_execute( void )
{
	char *txdata = NULL, *rxdata = NULL, *buf = NULL;
	uint8 txlen = 0, rxlen = 0, len = 0, count = 0;
	
    target_init();
	global_construct();
	net_setlocaladdress( g_net, _localaddr[0], _localaddr[1] );
	
	opf_init( g_txframebuf, sizeof(g_txframe) );
	opf_init( g_rxframebuf, sizeof(g_rxframe) );

	g_txframebuf = (char *)(&g_txframe);
	g_rxframebuf = (char *)(&g_rxframe);
	g_txpktbuf = opf_packet( g_txframebuf );
	g_rxpktbuf = opf_packet( g_rxframebuf );
	
	while (1)
	{
		// if a packet can be read out through the NET layer interface of net_read()
		// and net_rawread(), then this is a packet to current node. the other
		// packets will be processed by NET layer itself, so they will not be 
		// read out by net_read() functions. these functions include those packets
		// to be forwarded.
		//
		if (rxlen == 0)
		{
			count = net_rawread( g_net, g_rxframebuf, sizeof(TOpenFrame), 0x00 );
			if (count > 0)
			{
				rxlen = count;
				
				if (txlen == 0)
				{
					gnode_interpret( (TOpenFrame *)g_rxframebuf, (TOpenFrame *)g_txframebuf );
				}
			}
		}
		
		if (txlen > 0)
		{
			count = net_rawwrite( g_net, g_txframebuf, sizeof(TOpenFrame), 0x00 );
			if (count > 0)
			{
				txlen = 0;
			}
		}

		debug_evolve( g_debugio );
		//lcs_evolve( g_lcs );
		net_evolve( g_net );
		mac_evolve( g_mac );
	}
		
	global_destroy();	
	
	return;
}

// for sink nodes in sensor network, the main program is as the following.
// different to generic node in the network, it will forward the packet received
// to host computer through the UART/SIO hardware. while, it will also send 
// the packet received from the host to others nodes in the network.
//
void sinknode_execute( void )
{
	char *txdata = NULL, *rxdata = NULL, *buf = NULL;
	uint8 txlen = 0, rxlen = 0, len = 0, count = 0;
	
    target_init();
	global_construct();
	
	net_init( g_net, g_mac, NULL );
	net_setlocaladdress( g_net, _localaddr[0], _localaddr[1] );
	
	opf_init( g_txframebuf, sizeof(g_txframe) );
	opf_init( g_rxframebuf, sizeof(g_rxframe) );

	g_txframebuf = (char *)(&g_txframe);
	g_rxframebuf = (char *)(&g_rxframe);
	g_txpktbuf = opf_packet( g_txframebuf );
	g_rxpktbuf = opf_packet( g_rxframebuf );
	
	//net_configure
	//net_setaddress
	
	while (1)
	{
		// try to read data from the UART/SIO connected to the host.
		// if data received, then try to send them through net_rawwrite()
		if (txlen == 0)
		{
			// @TODO: change to sio_read in the near future
			count = uart_read( g_sio->uart, g_txframebuf + txlen, sizeof(TOpenFrame) - txlen, 0x00 );
			txlen += count;
		}
		
		if ((txlen > 0) && (txlen >= opf_length(g_txframebuf)))
		{
			count = net_rawwrite( g_net, g_txframebuf, sizeof(TOpenFrame), 0x00 );
			if (count > 0)
			{
				txlen = 0;
			}
		}
		
		// try to read data from the network. if data received, then try to send 
		// them to the host through the UART/SIO.
		if (rxlen == 0)
		{
			count = net_rawread( g_net, g_rxframebuf, sizeof(TOpenFrame), 0x00 );
			if (count > 0)
			{
				rxlen = count;
			}
		}
		
		if ((rxlen > 0) && (rxlen <= opf_length(g_rxframebuf)))
		{ 
			// @TODO: change to sio_write in the near future
			count = opf_length(g_rxframebuf) - rxlen;
			count = uart_write( g_sio->uart, g_rxframebuf + count, rxlen, 0x00 );
			if (count > 0)
			{
				if (rxlen <= count)
					rxlen = 0;
				else 
					rxlen -= count;
			}
		}
		
		debug_evolve( g_debugio );
		//lcs_evolve( g_lcs );
		net_evolve( g_net );
		mac_evolve( g_mac );
	}
		
	global_destroy();	
	
	return;
}

// this function will interpret the frame/packet received in txframebuf, and 
// put the processing reply into rxframebuf
//
int8 gnode_interpret( TOpenFrame * rxframe, TOpenFrame * txframe )
{/*
	TOpenData * data = opt_data(opf_packet((char*)rxframe)); 
	int8 ret=0;
	
	switch (data->type)
	{
	case ODA_TYPE_TEMPSENSOR:
	case ODA_TYPE_VIBSENSOR:
	case ODA_TYPE_LIGHTSENSOR:
		// @TODO: 这里应该将packet字段尽可能填写完整
	 	//ret = sen_fillpacket( g_sensors, opf_packet((char*)txframe) );
	 	break;
	}
	*/
	return 0;
}

