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

/*****************************************************************************
 * @attention
 * the "cc2420" chip has address identification mechanism built. if you want to 
 * use it as a sniffer frontier, you must turn off it. 
 *
 * @author zhangwei on 20070423
 * 	- revision today.
 * @modified by yan-shixing 20090725
 *  - revision. tested ok
 ****************************************************************************/ 

#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_cc2420.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_debugio.h"

#define CONFIG_LISTENER    
#undef  CONFIG_LISTENER    

#define TEST_CHOICE 2
#define TEST_ACK
#undef  TEST_ACK


#define PANID				0x0001
#define LOCAL_ADDRESS		0x0002
#define REMOTE_ADDRESS		0x0001
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

TiCc2420Adapter             g_cc;
TiUartAdapter	            g_uart;

#if (TEST_CHOICE == 1)
static void recvnode1(void);
#endif

#if (TEST_CHOICE == 2)
static void recvnode2(void);
static void _cc2420_listener( void * ccptr, TiEvent * e );
#endif


int main(void)
{
    #if (TEST_CHOICE == 1)
	recvnode1();
    #endif

    #if (TEST_CHOICE == 2)
	recvnode2();
    #endif
}

#if (TEST_CHOICE == 1)
void recvnode1(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	char * msg = "welcome to recvnode...";
	char buf[BUF_SIZE];
	uint8 len;

	target_init();
	OS_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	cc2420_open( cc, 0, NULL, NULL, 0x00 );
	
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							//Enable RX
	cc2420_enable_addrdecode( cc );					//使能地址译码
	#ifdef TEST_ACK
	cc2420_enable_autoack(cc);
	#endif

	cc2420_setpanid( cc, PANID );					//网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	hal_enable_interrupts();	
	// when use this scan mode to receive data, interrupt should be disable; otherwise the data will be
	// read twice and in the second time there are no data actually which leads to a assert.
 	// Attention: in this scan mode, MCU always try to read and in my  test it is faster than the transmission of data. 
	// Thus, after 4 times, there no data at all, and the MCU still want to read, which lead to an assert. So we'd better
	// not use this scan mode.
	while(1) 
	{
		cc2420_evolve( cc );
		
		len = cc2420_read( cc, (char*)(&buf[0]), BUF_SIZE, 0x00 );
		if (len >= 5)
		{
			// output this frame to the computer through uart
			dbo_write( (char*)(&buf[0]), len );
		}
		
	}
}
#endif

#if (TEST_CHOICE == 2)
void recvnode2(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	char * msg = "welcome to recvnode...";

	target_init();
	OS_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	cc2420_open( cc, 0, _cc2420_listener, cc, 0x00 );
	
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							//Enable RX
	cc2420_enable_addrdecode( cc );					//使能地址译码
	cc2420_setpanid( cc, PANID );					//网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识
	#ifdef TEST_ACK
	cc2420_enable_autoack(cc);
	#endif
	hal_enable_interrupts();
 
	while(1) {};
}
#endif

#if (TEST_CHOICE == 2)
void _cc2420_listener( void * owner, TiEvent * e )
{
	TiCc2420Adapter * cc = &g_cc;
	char buf[BUF_SIZE];
    uint8 len=0;
	dbo_putchar( '>' );
	led_toggle( LED_RED );
	while (1)
	{
		len = cc2420_read(cc, &buf[0], BUF_SIZE, 0x00);
		dbo_putchar( '>' );
		if (len> 0)
		{
			dbo_n8toa( len );
			//for(int i=0;i<len;i++)
			//	dbo_putchar( buf[i] );
		}
		else 
			break;
	}
}
#endif


