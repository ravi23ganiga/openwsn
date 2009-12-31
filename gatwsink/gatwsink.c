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
/* gatwsink
 * This module implements an gateway sink node. This node bridges the wireless sensor
 * network and computer(through RS232). It will forward the packet received from other
 * nodes to the computer through Rs232 and broadcast the packet received from the 
 * computer. 
 *
 * @modified by zhangwei on 20091226
 */
#include "../common/hal/hal_configall.h"  
#include "../common/svc/svc_configall.h"  
#include "../common/rtl/rtl_foundation.h"
#include "../common/rtl/rtl_iobuf.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/rtl/rtl_textspliter.h"
#include "../common/rtl/rtl_lightqueue.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_adc.h"
#include "../common/hal/hal_luminance.h"
#include "../common/hal/hal_timer.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/svc/svc_foundation.h"
#include "../common/svc/svc_aloha.h"
#include "../common/svc/svc_timer.h"
#include "../common/svc/svc_ledtune.h"
#include "../common/svc/svc_one2many.h"
#include "../common/svc/svc_siocomm.h"

/* todo
 * yanshixing
 * please finish rtl_textspliter and svc_siocomm first 
 * however, before working on these two modules, please firstly try to finish the main program 
 * you can understand more about the interface of textspliter and siocomm
 */

#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_REQUEST_SIZE             7

#define CONFIGURE_COMMAND				0x77
#define RESTART_COMMAND					0x88
#define TEST_CMD						0x99

#define VTM_RESOLUTION 					7

/* attention
 * You should guarantee the following memory doesn't exceed the MCU's memory limit!
 * or else you may encounter unexpected errors!
 */
static TiTimerAdapter 		m_timeradapter;
static TiTimerManager 		m_vtm;
static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
static TiOne2Many 			m_o2m;
static TiUartAdapter		m_uart;
static TiSioComm        	m_sio;
static TiTextSpliter		m_spliter;

//static TiLedTune          m_ledtune;
static char                 m_txbuf[ IOBUF_HOPESIZE(0x7F) ];
static char                 m_rxbuf[ IOBUF_HOPESIZE(0x7F) ];
static char                 m_txque[ LIGHTQUEUE_HOPESIZE(sizeof(TiIoBuf), 3) ];
static char                 m_rxque[ LIGHTQUEUE_HOPESIZE(sizeof(TiIoBuf), 3) ];

/* output the TiIoBuf content throught UART by default */
int8 _interpret( char * cmd, uint8 len );
void _output_iobuf( TiIoBuf * buf, TiUartAdapter * uart );
void _gatwsink();

int main(void)
{
	_gatwsink();
}

void _gatwsink()
{
	uint8 len, count;	
	char * msg = "welcome to gateway sink node...";
	char * pkt;

	TiTimerAdapter * timeradapter;
	TiTimerManager * vtm;
	TiTimer * vti;

	TiCc2420Adapter * cc;
    TiAloha * mac;

	TiUartAdapter * uart;
	TiIoBuf * rxbuf;
	TiIoBuf * txbuf;
	TiOne2Many * o2m;
	TiSioComm * sio;
    //TiLedTune * ledtune;

	// Each item in the queue is an TiIoBuf object which contains an entire packet.
	TiLightQueue * rxque;
	TiLightQueue * txque;

	//·Ö¸îÓÃ
	TiTextSpliter * spliter;

	target_init();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	dbo_open(0, 38400);

	timeradapter = timer_construct( (void *)(&m_timeradapter), sizeof(m_timeradapter) );
	vtm = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
	txbuf = iobuf_construct( &m_txbuf[0], sizeof(m_txbuf) );
	rxbuf = iobuf_construct( &m_rxbuf[0], sizeof(m_rxbuf) );
	txque = lwque_construct( &m_txque[0], sizeof(m_txque), sizeof(m_txbuf) );
	rxque = lwque_construct( &m_rxque[0], sizeof(m_rxque), sizeof(m_rxbuf) );

	cc2420_open(cc, 0, NULL, NULL, 0x00 );
	aloha_open( mac, cc, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,timeradapter, NULL, NULL,0x01);
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	vtm = vtm_open( vtm, timeradapter, VTM_RESOLUTION );
	vti = vtm_apply( vtm );
	vti_open( vti, NULL, NULL );
    //ledtune = ledtune_construct( &m_ledtune[0], sizeof(m_ledtune), vti );
    //ledtune_open( ledtune );

	// assume: we use One2Many architecture to collect data from other nodes. 
	// todo: DataTree Protocol should be used to on the sink node in the future.

	o2m = one2many_construct( (void *)(&m_o2m), sizeof(m_o2m) );
	one2many_open( o2m, mac);

	spliter = tspl_construct((void *)(&m_spliter), sizeof(TiTextSpliter));
	sio = sio_construct( (char *)(&m_sio), sizeof(TiSioComm) );
	sio_open( sio, uart, spliter, 0x00 );

	uart_write( uart, msg, strlen(msg), 0x00 );

	hal_enable_interrupts();

	while(1)
	{
		// section 1: try to receive packet from the serial communication channel.
		// 
		// rxbuf is an TiIoBuf object to accept the packet from serial communication channel.
		// it's usually through the UART hardware 

		if (iobuf_empty(txbuf))
		{
			count = sio_read( sio, txbuf );
			if( count > 0 )
			{
				pkt = (char *)iobuf_ptr( txbuf );
				_output_iobuf( txbuf, uart );

				// packet format
				// Packet := [Length 2B] [Packet Control 2B] [Destination Node Identifier 8B]
				//           [Source Node Identifier 8B] [Packet Payload nB] {Checksum 2B}
				// [Packet Payload nB]  := [Function Id 1B][Function Parameters mB]
				// if Function Id = 0xFF, then the next byte after Function Id should also 
				// regarded as function id.

				// assume pkt[20] is the function start
				// the first 20 bytes are the packet header (NET layer)
				//
				// if the interpreter returns 0, then we should put this packet into the 
				// txque and wait for sent through the wireless interface
				//
				if (_interpret( &pkt[20], iobuf_length(txbuf)-20 ) == 0)
				{
					lwque_pushback( txque, txbuf );
					iobuf_clear( txbuf );
				}
				else
					iobuf_clear( txbuf );
			}
		}
		
		// section 2: try to send the packet through the wireless interface
		if (!lwque_empty(txque))
		{
			// get the first packet in the queue
			txbuf = lwque_front(txque);
			pkt = iobuf_ptr( txbuf );
			_output_iobuf( txbuf, uart );

			pkt[21] = 0x01;              // request type
			//request[0] = 0x01;
			//request[1] = 0xFF;              // set destination address
			//request[2] = 0xFF;              // 0xFFFF is the broadcast address
			//request[3] = (char)(CONFIG_NODE_ADDRESS >> 8);       
										// set source address, 
			//request[4] = (char)(CONFIG_NODE_ADDRESS & 0xFF);   
										// namely local address
			//iobuf_setlength( txbuf, 25 );

			len = one2many_broadcast( o2m, txbuf, 0x00 );
			iobuf_clear( txbuf );

			if (len <= 0)                   
			{
				// turn on the led for a little when broadcast failed
				led_on( LED_YELLOW );
				hal_delay( 300 );
				led_off( LED_YELLOW );

				// delay a little while before the next sending. don't be too aggresive 
				// to occupy the wireless channel all the time, though this should be 
				// done in the MAC layer.
				// hal_delay( 100 );
				// continue;
			}
			else
				lwque_popfront( txque );
		}

		// section 3: try to receive frames through the wireless interface

		if (iobuf_empty(rxbuf))
		{
			len = one2many_recv( o2m, rxbuf, 0x00 );
			if (len > 0)
			{	
				// the following lines can be uncommented for debugging only
				// pkt = iobuf_ptr( rxbuf );
				/*if (pkt[21] == 0x02)
				{
					_output_iobuf( rxbuf, uart );
                	ledtune_write( ledtune, MAKE_WORD(response[2], response[1]) );
				}*/

				// put the received packet into RX queue. the packets inside RX queue
				// will be continue forwarded through the sio channel
				lwque_pushback( rxque, rxbuf );
			}
		}

		// section 4: try to send the packet out through the serial communication interface

		if (!lwque_empty(rxque))
		{
			if (sio_write(sio, lwque_front(rxque)) > 0)
				lwque_popfront( rxque );
		}

		/* 
		// configure the time interval as 2 seconds. non-periodical
		// the "vti" timer will automatically stopped when it's expired.
		//
		//vti_setscale( vti, 1 );
		//vti_setinterval( vti, 3000, 0x00 );
		//vti_start( vti );

		while (!vti_expired(vti))
		{
			len = one2many_recv( o2m, rxbuf, 0x00 );
			if (len > 0)
			{	
				response = iobuf_ptr( rxbuf );
				if(response[0] == 0x02)
				{
					_output_iobuf( rxbuf, uart );
                    ledtune_write( ledtune, MAKE_WORD(response[2], response[1]) );
				}
			}
		}
		*/
		hal_delay(100);
		sio_evolve( sio, NULL );
		one2many_evolve( o2m, NULL);
		vtm_evolve( vtm, NULL );
	}
}

/* return
 *	1	command execute success
 *	0	this isn't an valid command
 *	-1	command execution failed
 */
int8 _interpret( char * cmd, uint8 len )
{
	int8 result=0;

	switch (cmd[0])
	{
	case CONFIGURE_COMMAND:
		// execute the command
		result = 1;
		break;

	case RESTART_COMMAND:
		// execute the command
		result = -1;
		break;

	case 0xFF:
		switch (cmd[1])
		{
		case TEST_CMD:
			result = 1;
			break;
		}
		break;

	default:
		result = 0;
	}

	return result;
}

void _output_iobuf( TiIoBuf * buf, TiUartAdapter * uart )
{
	char * response;
	uint8 len = 0, i=0;
	len = iobuf_length(buf);

	if ( len > 0)
	{
		response = iobuf_ptr(buf);
		
		for( i=0; i<len; i++ )
		{
			dbo_n8toa(response[i]);
			dbo_putchar(' ');
		}
		/*dbo_string( "response:\r\n" );

		dbo_string( "shortaddrto: " );
		dbo_n8toa( response[1] );
		dbo_n8toa( response[2] );

		dbo_string( "\r\nshortaddrfrom: " );
		dbo_n8toa( response[3] );
		dbo_n8toa( response[4] );

		dbo_string( "\r\nsensor value:" );
		dbo_n8toa( response[5] );
		dbo_n8toa( response[6] );
		dbo_string( "\r\n" );
		*/
		//dbo_putchar(response[5]);
		//dbo_putchar(response[6]);
	}
}
