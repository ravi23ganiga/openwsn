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

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_target.h"
#include "../../common/openwsn/rtl/rtl_openframe.h"

#define CONFIG_LISTENER    
//#undef  CONFIG_LISTENER    

#define PANID				0x0001
#define LOCAL_ADDRESS		0x01
#define REMOTE_ADDRESS		0x00
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11


TiCc2420Adapter             g_cc;
TiUartAdapter               g_uart;
//char                        g_rxbuf[BUF_SIZE];
char                        g_rxbufmem[ OPF_SUGGEST_SIZE ];


#ifdef CONFIG_LISTENER
static void _cc2420_listener( void * ccptr, TiEvent * e );
#endif

static void sniffer(void);


int main(void)
{
    sniffer();
}

void sniffer(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;
	char * msg = "welcome to sniffer...";
	#ifdef CONFIG_LISTENER
    uint8 len;
    #endif

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );

	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart = uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	#ifdef CONFIG_LISTENER
	cc = cc2420_open( cc, 0, _cc2420_listener, NULL, 0x00 );
	#else
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
	#endif

	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							//Enable RX
	cc2420_setpanid( cc, PANID );					//网络标识, seems no use in sniffer mode
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识, seems no use in sniffer mode
	cc2420_disable_addrdecode( cc );				//使能地址译码

    opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem), opf, 0x00 );

	hal_enable_interrupts();
 
	#ifdef CONFIG_LISTENER
	while(1) {}
	#endif

	#ifdef CONFIG_LISTENER
	while(1) 
	{
		len = cc2420_read( cc, opf->buf, opf->size, 0x00 );
		if (len > 0)
		{
			uart_putchar( uart, '>' );
			if (!opf_parse(opf, 0))
			{
				uart_putchar( uart, '?' );
			}
            uart_write( uart, (char*)&(opf->buf[0]), len, 0x01 );
			led_off(LED_YELLOW);
			hal_delay(500);
        }
		cc2420_evolve( cc );
	}
	#endif
}

#ifdef CONFIG_LISTENER
void _cc2420_listener( void * owner, TiEvent * e )
{
	TiCc2420Adapter * cc = &g_cc;
	char * msg = "_cc2420_listener: ";
	char buf[BUF_SIZE];
    uint8 len=0;

	uart_putchar( &g_uart, '*' );
	//uart_putchar( &g_uart, 0x77 );
	uart_write( &g_uart, msg, strlen(msg), 0x01 );	

	while (1)
	{
		len = cc2420_read(cc, &buf[0], BUF_SIZE, 0x00);
		if (len> 0)
		{
			uart_putchar( &g_uart, '>' );
			uart_putchar( &g_uart, len );

			for(int i=0;i<len;i++)
				uart_putchar(&g_uart, buf[i]);

			uart_putchar(&g_uart, 0x99);
		}
		else 
			break;
	}
}
#endif

/* old sniffer
void sniffer(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	char * msg = "welcome to sniffer...";

	uint8 buf[BUF_SIZE];
	uint8 len;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	// cc = cc2420_open( cc, 0, _cc2420_listener, NULL, 0x00 );
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );

	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart = uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							//Enable RX
	//cc2420_setpanid( cc, PANID );					//网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识
	cc2420_disable_addrdecode( cc );				//使能地址译码

	hal_enable_interrupts();
 
	while(1) 
	{
		len = cc2420_read( cc, (char*)(&buf[0]), BUF_SIZE, 0x00 );
		if (len > 0)
		{
			uart_putchar( uart, '>' );
            uart_write( uart, (char*)(&buf[0]), len, 0x01 );
			led_off(LED_YELLOW);
			hal_delay(500);
        }
		// cc2420_evolve( cc );
	}
}
*/


/*
void sniffer_run( void )
{
	char rxbuf[MAX_BUFFER_SIZE];
	char * buf;
	uint8 rxlen, count;
	TSioComm sio;
	char * msg = "sniffer started...\n";
	
    target_init();
    global_construct();
	led_init();
    spi_configure( g_spi );
    uart_configure( g_uart, CONFIG_UART_BAUDRATE, 0, 0, 0, 0 );
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0 );
    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, 0x2420, 0 );
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, 0x5678, 0);

    //uart_write( g_uart, "program run1..", 15, 0 );
    // this configure will disable the address recognition mechanism so that 
    // the transceiver "cc2420" can receive all the frames in transmission, even
    // the destination address isn't the current node. 
	// hardware address recognition can be enabled or disabled using 
	// MDMCTRL0.ADR_DECODE bit in register CC2420_MDMCTRL0 (p.39, cc2420 datasheet) 
	//FAST2420_SETREG(spi,CC2420_MDMCTRL0,0x02E2) ;

    cc2420_configure( g_cc2420, CC2420_CONFIG_SNIFFER_MODE, 0, 0);
	
	sio_construct( (char*)&sio, sizeof(TSioComm), g_uart, 0x00 );
	sio_configure( &sio, NULL, 0x00, 27 );
    sio_write( &sio, msg, strlen(msg)+1, 0x00 );

	//cc2420_relation( g_spi0 );
	memset( (char*)(&rxbuf[0]), 0x00, MAX_BUFFER_SIZE ); 
	rxlen = 0;   
    cc2420_receive_on(g_cc2420);  
    IRQEnable();
		
	// assume: you should have construct g_cc2420, g_sio, g_uart successfully now
	// each time the sniffer received a frame, the LED will toggle its state	
	sio_write( &sio, msg, strlen(msg), 0x00 );
	while (TRUE)
	{
		led_toggle( LED_RED );
		count = cc2420_rawread( g_cc2420, buf, rxlen, 0 );
		if (count > 0)
		{
			sio_write(&sio, buf, count, 0 );
		}
	}
		
	global_destroy();	
	return;
}
*/


/* 

void demo_handlepacket(void)
{
       uint8_t num,i,flag;
	flag=(cc2420_rxflag+cc2420_rxbufuse)&0x01;
	cc2420_rxbuf_now=cc2420_rxbuf[flag];
	//intf("\nreceive a packet in channel %04x!\n",cc2420_readreg(cc2420_FSCTRL));
	
	num=cc2420_rxbuf_now[0]+1;
	for (i=0;i<num;i++)
		printf("%02x",cc2420_rxbuf_now[i]);
	   
	usart_putbyte(0x7e);
	usart_putbyte(0x42);
	
	usart_putbyte(0x21);
	usart_putbyte(0x02);
	usart_putbyte(0x03);
	usart_putbyte(cc2420_rxbuf_now[10]);//source address
	usart_putbyte(0x01);
	usart_putbyte(cc2420_rxbuf_now[3]);//adc sequec
	
	usart_putbyte(cc2420_rxbuf_now[12]);//adc data0
	usart_putbyte(cc2420_rxbuf_now[13]);//adc data0
	
	usart_putbyte(cc2420_rxbuf_now[14]);//adc data1
	usart_putbyte(cc2420_rxbuf_now[15]);//adc data1
	
	usart_putbyte(cc2420_rxbuf_now[16]);//adc data2
	usart_putbyte(cc2420_rxbuf_now[17]);//adc data2
	
	usart_putbyte(cc2420_rxbuf_now[18]);//adc data3
	usart_putbyte(cc2420_rxbuf_now[19]);//adc data3
	
	usart_putbyte(cc2420_rxbuf_now[20]);//adc data4
	usart_putbyte(cc2420_rxbuf_now[21]);//adc data4
	
	usart_putbyte(cc2420_rxbuf_now[22]);//adc data5
	usart_putbyte(cc2420_rxbuf_now[23]);//adc data5
	
	usart_putbyte(cc2420_rxbuf_now[24]);//adc data6
	usart_putbyte(cc2420_rxbuf_now[25]);//adc data6
	
	usart_putbyte(cc2420_rxbuf_now[26]);//adc data7
	usart_putbyte(cc2420_rxbuf_now[27]);//adc data7
	
	usart_putbyte(cc2420_rxbuf_now[28]);//adc data8
	usart_putbyte(cc2420_rxbuf_now[29]);//adc data8
	
	usart_putbyte(cc2420_rxbuf_now[30]);//adc data9
	usart_putbyte(cc2420_rxbuf_now[31]);//adc data9
	
	usart_putbyte(0x01);//crc1
	usart_putbyte(0x01);//crc0
	
	usart_putbyte(0x7e);
	
	 
	free(cc2420_rxbuf[flag]);
	cc2420_rxbufuse--;
	if  ((cc2420_rxbufuse==0) && (rfpackernum!=0))
		{
		   cc2420_rxbufuse++;
		   HAL_post(&cc2420_readrxfifo);
		}
	cc2420_rxbuf[flag]=NULL;
	//printf("释放rxbuf,now point is %04x\n",cc2420_rxbuf[flag]);
}
*/



