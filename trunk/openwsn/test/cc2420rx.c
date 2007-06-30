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
 *****************************************************************************/ 
#include "..\configall.h"
#include <stdlib.h>
#include "config.h"
#include "..\hal\hal.h"
#include "..\service\svc_debugio.h"
#include "..\global.h"

#define CONFIG_PACKET_API
//#define CONFIG_FRAME_API

static uint8 tx_frame[128];
//static uint8 rx_frame[128];            //using CONFIG_FRAME_API

static TCc2420Frame tx_test;
static TCc2420Frame rx_test;           //using CONFIG_PACKET_API

int cc2420rx_test (void)
{
    UINT8 n;
    int8 length;
    uint16 temp;
    uint8 ledPeriod;
    char * out_string = "   rssi = ";
    bool led_state = true;
  
    target_init();
    
    tx_test.panid = 0x2420;
    tx_test.nodeto = 0x1234;
    tx_test.nodefrom = 0x5678;

    global_construct();
    spi_configure( g_spi );
    uart_configure( g_uart, CONFIG_UART_BAUDRATE, 0, 0, 0, 0 );
   
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, tx_test.panid, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, tx_test.nodefrom, 0);
    
    for (n = 0; n < 10; n++) 
    {
        tx_test.payload[n] = 2;
        tx_frame[10 + n] = 2;
    }
    tx_test.payload[2] = 1;  

    tx_frame[1] = tx_frame[2] = tx_frame[3] = 0;
    tx_frame[4] = 0x20; tx_frame[5] = 0x24;
    tx_frame[6] = 0x78; tx_frame[7] = 0x56;
    tx_frame[8] = 0x34; tx_frame[9] = 0x12; 
    
    uart_write(g_uart, "cc2420rx_test started...\r\n", 24, 0 );
    cc2420_open( g_cc2420 );
    
    ledPeriod = 1;
    while (TRUE) 
	{ 
	    // test section one
	    // try receive frame using the frame-based API cc_read  
        #ifdef CONFIG_PACKET_API
        led_twinkle(LED_GREEN,ledPeriod);
	  
        //length = cc2420_read( g_cc2420,&rx_test,0,0);
        length = cc2420_read( g_cc2420,&rx_test,0);
	  
	    if(length > 0)
	    //if (length > 1) 
	    {
	    	led_state = !led_state;
	    	if (led_state)
	    		led_on( LED_RED );
	    	else
	    		led_off( LED_RED ); 
	    	
	        ledPeriod = rx_test.payload[0];
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
	  	// try receive using stream based API cc_rawread
		#ifdef CONFIG_FRAME_API
	  	led_twinkle(LED_GREEN,ledPeriod);
    	length = cc2420_rawread( g_cc2420,(char *)rx_frame, sizeof(rx_frame),0 );
	  	if (length > 11) 
	  	{
	  		ledPeriod = rx_frame[10];
	  		uart_write( g_uart, (char *)rx_frame, length ,0  );
	  	}
	 	#endif
	}
    global_destroy();

	return 0;															
}
