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
#include "..\foundation.h"
#include "..\global.h"
#include "openmac_test.h"
#include "..\hal\hal.h"

/* the testing of OpenMAC assumes you have 3 node and a host computer. they 
 * three communicate each other at the same. the host program will send packet
 * and try to receive it. the host program will compare the sending and received
 * together so that to judge whether there are data collision during the test.
 * 
 * Host <===> Node A <--------> Node B
 *             
 *            Node C 
 * 
 * Node C is a whitehole. it will send frames to Node B occasionally based on a 
 * timer driven. and Node A (the sink node) will communicate with Node B at the 
 * same time. Node A will accept frames from host and sends the frames received
 * to host to. while, Node B will simply forward the frame received back to its
 * sender, for example, the frame from C is return back C, the frame from A is 
 * return back to A.
 * 
 * Node C is the disturbence of A to B's communication.
 */ 

/* the value of NODETYPE can be 0,1,2
 * it is related to address
 * for node A, B, C, the address is 0x01, 0x02, 0x03 respectively.
 */
#define CONFIG_NODETYPE 0
#define CONFIG_ADDRESS CONFIG_NODETYPE

#define PANID         0x2420
#define SINK_ADDR     0x5678
#define NODE_1        0x2222
#define NODE_NOISE    0x3333

#define UART_BUF_SIZE 256

void openmac_run( void )
{
	char txbuf[OPENMAC_BUFFER_SIZE];
	char rxbuf[OPENMAC_BUFFER_SIZE];
	uint8 txlen, rxlen, count,n;
	uint16 nodefrom, nodeto;
	TOpenMAC m_mac;
	TOpenMAC * g_mac;
	char * buf;
	

	target_init();
	
	global_construct();
	spi_configure( g_spi );
	uart_configure( g_uart, 115200, 0, 0, 0, 0 );
	//timer_init( g_timer1, 1, 2 );
	
	g_mac = mac_construct( (char*)(&m_mac), sizeof(m_mac) );
	
	mac_init( g_mac, g_cc2420, g_timer1 );
	mac_configure( g_mac, MAC_BASIC_INIT, 0);
	
	
	memset( (char*)(&txbuf[0]), 0x00, OPENMAC_BUFFER_SIZE ); 
	memset( (char*)(&rxbuf[0]), 0x00, OPENMAC_BUFFER_SIZE );
	
	txlen = 0;
	rxlen = 0;
	
	//nodefrom = CONFIG_ADDRESS;
	//nodeto = (nodefrom+1) % 3;
	
	opf_setpanid( txbuf, PANID);
	opf_setpanid( rxbuf, PANID);
	mac_configure( g_mac, MAC_CONFIG_PANID, PANID);
	
	cc2420_receive_on(g_cc2420);  
        IRQEnable(); 
        
        led_twinkle(LED_YELLOW,6);
	
	// sink node
	#if CONFIG_NODETYPE == 0
	while (1)
	{  
                opf_setaddrfrom(txbuf,SINK_ADDR);
                opf_setaddrto(txbuf,NODE_1);
                mac_configure( g_mac, CC2420_CONFIG_LOCALADDRESS, SINK_ADDR);
		/*
		txlen = 11 + 10;
		
		for (n = 0; n < 10; n++) {
                  txbuf[n+9] = 0x22;
                }
		*/
		
		buf = rxbuf;
		if (rxlen == 0)
		{
			rxlen = mac_rawread( g_mac, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
		if (rxlen > 0)
		{
			//led_twinkle(LED_YELLOW,1);
			uart_write( g_uart, buf, rxlen, 0x00 );
			rxlen = 0;
		}
                /*
		buf = txbuf;
		if (txlen == 0)
		{
			txlen = uart_read( g_uart, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
		if (txlen > 0)
		{
			if (mac_rawwrite( g_mac, buf, txlen, 0x00 ) > 0)
				txlen = 0;
		}
		*/
		led_twinkle(LED_GREEN,1);
		mac_evolve( g_mac );
		led_twinkle(LED_RED,1);
	}
	#endif

	#if CONFIG_NODETYPE == 1
	txlen = 0;
	rxlen = 0;
	while (1)
	{
		opf_setaddrfrom(txbuf,NODE_1);
                opf_setaddrto(txbuf,SINK_ADDR);
                mac_configure( g_mac, CC2420_CONFIG_LOCALADDRESS, NODE_1);
                
                txlen = 11 + 10;
		
		for (n = 0; n < 10; n++) {
                  txbuf[n+9] = 0x33;
                }
		/*
		if (rxlen == 0)
		{
			rxlen = mac_rawread( g_mac, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
		
		if ((rxlen > 0) && (txlen == 0))
		{
			nodefrom = opf_nodefrom( rxbuf );
			nodeto = opf_nodeto( rxbuf );
			memmove( txbuf, rxbuf, OPENMAC_BUFFER_SIZE );
			opf_setnodefrom( txbuf, nodefrom );
			opf_setnodeto( txbuf, nodeto );
			txlen = rxlen;
		}
		*/
		if (txlen > 0)
		{
			if (mac_rawwrite( g_mac, buf, txlen, 0x00 ) > 0)
				txlen = 0;
		}
		led_twinkle(LED_GREEN,1);
		mac_evolve( g_mac );
		led_twinkle(LED_RED,1);
	}
	#endif
	
	
	#if CONFIG_NODETYPE == 2
	// you can change the node address here according to your own testing
	// environment.
	//
	buf = txbuf;
	opf_setaddrfrom( txbuf, 3 );
	opf_setaddrto( txbuf, 1 );
	
	while (1)
	{
		opf_setaddrfrom(txbuf,NODE_NOISE);
                opf_setaddrto(txbuf,SINK_ADDR);
                mac_configure( g_mac, CC2420_CONFIG_LOCALADDRESS, NODE_NOISE);
		
		timer_stop( g_timer1 );
		timer_setinterval( g_timer1, 500, 0 );
		timer_start( g_timer1 );
		
		while (!timer_expired(g_timer1))
		{
			mac_rawwrite( g_mac, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
		
		mac_evolve( g_mac );
		led_twinkle(LED_RED,1);
	}
	#endif
		
	mac_destroy( g_mac );
	global_destroy();	
	return;
}


