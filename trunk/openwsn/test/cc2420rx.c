/******************************************************************************
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
 ******************************************************************************/ 
#include "../configall.h"
#include <stdlib.h>
#include "../hal/hal.h"
#include "../service/svc_debugio.h"
#include "../global.h"

/******************************************************************************
 * @modified by zhangwei on 20070728
 * made revison today and eliminate some unncessary code in this test.
 * 
 ******************************************************************************/ 

#define CONFIG_GENERAL_RW
//#define CONFIG_RAWFRAME_RW

#define PANID 0x2420
#define REMOTE_NODE 0x1234
#define LOCAL_NODE 0x5678

static TCc2420Frame g_frame;
static uint8 g_buf[0xFF];

int cc2420rx_test (void)
{
    int8 length, n;
    uint16 temp;
    uint8 led_period;
    char * msg = "cc2420rx started...\r\n";
    char * out_string = "   rssi = ";
    bool led_state = true;
  
    target_init();
    led_twinkle( LED_RED, 1000 );
    
    global_construct();
    spi_configure( g_spi );
    uart_configure( g_uart, CONFIG_UART_BAUDRATE, 0, 0, 0, 0 );
   
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, PANID, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, LOCAL_NODE, 0);
   
    uart_write(g_uart, msg, strlen(msg), 0 );
    
    g_frame.panid = PANID;
    g_frame.nodeto = REMOTE_NODE;
    g_frame.nodefrom = LOCAL_NODE;

    for (n = 0; n < 10; n++) 
    {
        g_frame.payload[n] = 2;
        g_buf[10 + n] = 2;
    }
    g_frame.payload[2] = 1;  

    g_buf[1] = g_buf[2] = g_buf[3] = 0;
    g_buf[4] = 0x20; g_buf[5] = 0x24;
    g_buf[6] = 0x78; g_buf[7] = 0x56;
    g_buf[8] = 0x34; g_buf[9] = 0x12; 

    cc2420_open( g_cc2420 );
    uart_write( g_uart, "11\r\n", 4, 0x00 ); 
    
    led_period = 1;
    while (TRUE) 
    { 
        led_off( LED_ALL );
		
        // test section one
        // try receive frame using the frame-based API cc2420_read  
        #ifdef CONFIG_GENERAL_RW
        length = cc2420_read( g_cc2420, &g_frame, 0);
        if(length > 0)
         //if (length > 1) 
        {
            uart_write( g_uart, "12\r\n", 4, 0x00 ); 
            led_state = !led_state;
            if (led_state)
                led_on( LED_RED );
            else
                led_off( LED_RED ); 
            hal_delay( 500 );	
	    	
            led_period = g_frame.payload[0];
            temp = g_cc2420->rssi;
	  
            uart_write( g_uart, out_string, strlen(out_string), 0  );
            if ((temp / 100) > 0) 
                uart_putchar(g_uart,(char)(temp / 100 + 48));
	  		
            temp = temp % 100; 
            if ((temp / 10) > 0) 
                uart_putchar(g_uart,(char)(temp / 10 + 48));
            temp = temp % 10;
            uart_putchar(g_uart,(char)(temp + 48));
            uart_putchar(g_uart, '\n' );
        }
	  	#endif
	  
	 // test section two
        // try receive using stream based API cc2420_rawread
        #ifdef CONFIG_RAWFRAME_RW
        led_twinkle(LED_GREEN,led_period);
        length = cc2420_rawread( g_cc2420,(char *)g_buf, sizeof(g_buf),0 );
        if (length > 11) 
        {
            led_period = rx_frame[10];
            uart_write( g_uart, (char *)g_buf, length ,0  );
        }
        #endif
    }
    global_destroy();

    return 0;															
}

