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

/******************************************************************************
 * floodsender.c
 * command sender node in the network
 *
 * Periodically sending LED on/off commands to control all the LEDs in the network.
 * 
 * @author zhangwei on 20061106
 *	- first created
 * @modified by zhangwei on 20090801
 *	- revision. compile passed.
 * @modified by Shi-Miaojing and Yan-Shixing (TongJi University) on 20090802
 *	- testing...
 * @modified by ShiMiaojing on 20091101 uncompiled need to modified svc_flood.c
 * test ok on November 17,2009  good performance.
 *****************************************************************************/
 
#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif 

#define CONFIG_NIOACCEPTOR_RXQUE_CAPACITY 2
#define CONFIG_NIOACCEPTOR_TXQUE_CAPACITY 2


#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/rtl/rtl_configall.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/svc/svc_nio_aloha.h"
#include "../../common/openwsn/svc/svc_nio_flood.h"

#define PANID				0x0001
#define BROADCAST_PAN       0xffff
#define LOCAL_ADDRESS		0x0009   
#define BROADCAST_ADDRESS	0xffff
#define DEFAULT_CHANNEL     11

#define MAX_IEEE802FRAME154_SIZE                128 

#define NAC_SIZE NIOACCEPTOR_HOPESIZE(CONFIG_NIOACCEPTOR_RXQUE_CAPACITY,CONFIG_NIOACCEPTOR_TXQUE_CAPACITY)


static TiCc2420Adapter		m_cc;
static TiFrameRxTxInterface m_rxtx;
static char                 m_nacmem[NAC_SIZE];
static TiAloha              m_aloha;
static TiFloodNetwork       m_net;
static TiTimerAdapter       m_timer;
static char                 m_txbuf[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

static void ledflood( uint16 ontime, uint16 offtime );

int main(void)
{
	// all the led will be turned on for 500 ms and off for 5000 ms 
	ledflood( 1000, 1000 );
}

void ledflood( uint16 ontime, uint16 offtime )
{
    TiCc2420Adapter * cc;
	TiFrameRxTxInterface * rxtx;
	TiNioAcceptor * nac;
    TiAloha * mac;
	TiFloodNetwork * net;
	TiFrame * txbuf;
    TiTimerAdapter * timer;
	char * msg = "welcome to floodsender";
	uint8 seqid=0;
	int k;

	char * pc;//todo

    /***************************************************************************
	 * Device Startup and Initialization 
     **************************************************************************/
	 
	target_init();
	//wdt_disable();
	
	led_on(LED_ALL);
	hal_delay( 1000 );
	led_off( LED_ALL );
	hal_delay(1000);

	rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_write( msg, strlen(msg) );
	
    /***************************************************************************
	 * Flood Startup
     **************************************************************************/

	cc = cc2420_construct( (char *)&m_cc, sizeof(TiCc2420Adapter) );
	nac = nac_construct( &m_nacmem[0], NAC_SIZE );
	mac = aloha_construct( (char *)&m_aloha, sizeof(TiAloha) );
	net = flood_construct( (void*)&m_net, sizeof(TiFloodNetwork) );
    timer= timer_construct( (char *)(&m_timer), sizeof(TiTimerAdapter));

	// open the transceiver driver. we use TiCc2420Adapter in this example.
	
	cc2420_open( cc, 0, NULL, NULL, 0x00 );
    // cc2420_settxpower( cc, CC2420_POWER_2 );
	rxtx = cc2420_interface( cc, &m_rxtx );
	hal_assert( rxtx != NULL );
	nac = nac_open( nac, rxtx, CONFIG_NIOACCEPTOR_RXQUE_CAPACITY, CONFIG_NIOACCEPTOR_TXQUE_CAPACITY);
	hal_assert( nac != NULL ); 
	
	// open the medium access control protocol for sending and receiving
	// should use a 16bit or 32bit timer for the MAC. 8 bit timer will lead to wrong
	// delay results.
	
	timer = timer_open( timer, 2, NULL, NULL, 0x00 ); 
	mac = aloha_open( mac, rxtx, nac, DEFAULT_CHANNEL, PANID,LOCAL_ADDRESS, timer, 
		NULL, NULL, 0x01);
	flood_open( net, mac, NULL, NULL, PANID, LOCAL_ADDRESS );
	hal_assert( (timer != NULL) && (mac != NULL) );

   /* //todo for testing
    while(1)
    {
		led_toggle(LED_RED);
		hal_delay(1000);
		dbc_putchar(0xff);
    }
	*/	
	// start periodical sending 
	// the application layer frame format is as the following 
	//   [SEQID 1B][COMMAND Id, 1B][DATA nB]

    //todo ���������������whileѭ�������޷�ʵ��pc[0]�ĸı䣬��Ϊÿ��ѭ������txbuf���г�ʼ�����൱��ÿ��pc[0]��ֵ����һ���ġ�
	txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	pc = frame_startptr( txbuf );
	
	hal_enable_interrupts();
	while(1) 
	{
		//txbuf = frame_open( (char*)(&m_txbuf), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
		//pc = frame_startptr( txbuf );
        pc[0] = !pc[0]; // command byte, controls the LED state

		
		pc[1] = seqid;
		for (k = 2; k<frame_capacity(txbuf); k++)
		{
			pc[k] = k;
		}
		
		frame_setlength( txbuf,frame_capacity( txbuf));//todo 

		while (1)
        {
            if (flood_broadcast(net, txbuf, 0x00) > 0)
            {   
				// output for testing
				dbc_putchar( pc[0] );
                led_toggle( LED_YELLOW );
                break;
            }
			
			// delay some time before retry in order to avoid occupy the wireless 
			// channel all the time.
            hal_delay(1000);                       
        }
		

		aloha_evolve( mac, NULL );
		flood_evolve( net, NULL );

		// delay the LED long enough so we can observe its flash
		if( pc[0])
			hal_delay( 500 );
		else
			hal_delay( 1000 );
	}	
}
