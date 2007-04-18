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
#include "..\hal\hal.h"
#include "..\global.h"
#include "uartecho.h"

/*****************************************************************************
 * @author zhangwei on 2006-07-25
 *
 * 本程序只测试ARM 7内部的两个UART，它会把每一个收到的字符原封不动的送回去。
 * 可通过超级终端、串口调试助手等测试本程序效果
 * 
 * @state
 * test passed
 * 
 ****************************************************************************/ 

#define UART_BUF_SIZE 256

void uartecho_run( void )
{

	char memory[UART_BUF_SIZE * 2];
	char * buf1 = (char *)( &memory[0] );
	char * buf2 = buf1 + UART_BUF_SIZE;
	uint8 rxlen1, rxlen2, count;

	target_init();
	global_construct();
    
	IRQEnable();
	
	uart_write( g_uart, "uartecho run...", 15, 0 );
	uart_configure( g_uart, 115200, 0, 0, 0, 0 );
	uart_write( g_uart, "after", 5, 0 );
	
	memset( buf1, 0x61, UART_BUF_SIZE ); 
	memset( buf2, 0x00, UART_BUF_SIZE ); 
	rxlen1 = 3;
	rxlen2 = 0;

	while (1)
	{
		count = uart_write( g_uart, buf1, rxlen1, 0 );
	    if (count > 0)
		{
			rxlen1 -= count;
			memmove( buf1, buf1 + count, rxlen1 );
			//led_twinkle(LED_GREEN,3);
			//rxlen1 -= count;
		}
			
		/*count = uart_write( g_debuguart, buf2, rxlen2, 0 );
		if (count > 0)
		{
			rxlen2 -= count;
			memmove( buf2, buf2 + count, rxlen2 );
		}
		*/
		
		count = uart_read( g_uart, (char*)buf1 + rxlen1, UART_BUF_SIZE - rxlen1, 0 );
	    rxlen1 += count;
	    /* 
	    if (count>0) 
	    {   
	    	uart_write( g_uart, "xxxxxx", 6, 0 );
	        uart_putchar( g_uart, count);
	    }
	    */
	        
	    IRQEnable();
	        
	    //led_twinkle(LED_GREEN,3);
	    //led_twinkle(LED_GREEN,3);
	    //led_twinkle(LED_GREEN,3);
		
		//count = uart_read( g_debuguart, (char*)buf2 + rxlen2, UART_BUF_SIZE - rxlen2, 0 );
		//rxlen2 += count;
	}
		
	global_destroy();	
	
	return;
} 

