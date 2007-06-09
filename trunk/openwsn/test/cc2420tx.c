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
 
#include "..\foundation.h"
#include <stdlib.h>
#include "..\hal\hal.h"
#include "..\global.h"

/*****************************************************************************
 * this test demostrate how to send a frame using TCc2420 object 
 *
 * @author MaKun on 2007-04-18
 * test sending/TX function of cc2420 driver
 *
 * @modified by zhangwei on 20070418
 * just modify the format of the source file and including files
 *   
 ****************************************************************************/ 

#define CONFIG_PACKET_API
#undef CONFIG_FRAME_API

#define PAN 0x2420
#define LOCAL_ADDRESS 0x1234
#define REMOTE_ADDRESS 0x5678

int cc2420tx_test(void)
{
    uint8 n;
    int8 length;
    //char * msg = "cc2420tx_test() running...\n";
	TOpenFrame txframe;			//used when CONFIG_PACKET_API
	uint8 txbuf[128];			//used when CONFIG_FRAME_API

    target_init();
    global_construct();
    spi_configure( g_spi );
    uart_configure( g_uart, 115200, 0, 0, 0, 0 );
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0);
    
    //uart_write( g_uart, msg, strlen(msg)+1, 0x00 ); 
    
    memset( &txframe, 0x00, sizeof(txframe) );
    txframe.length = 50; // between 1 and 0x127
    txframe.panid = PAN;
    txframe.nodeto = REMOTE_ADDRESS;
    txframe.nodefrom = LOCAL_ADDRESS;
    //txframe.nodeto = 0x3456;//for test the sniffer

    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, PAN, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, LOCAL_ADDRESS, 0);
    
    for (n = 0; n < 10; n++) 
    {
        txframe.payload[n] = 2;
        txbuf[10 + n] = 2;  //?
    }
    txframe.payload[2] = 1;  

    txbuf[1] = txbuf[2] = txbuf[3] = 0;
    txbuf[4] = 0x20; txbuf[5] = 0x24;
    txbuf[6] = 0x78; txbuf[7] = 0x56;
    txbuf[8] = 0x34; txbuf[9] = 0x12; 
    
    cc2420_receive_on(g_cc2420);  
    IRQEnable(); 
    
	while (TRUE) 
	{    
	    // test section one: 
		// transmit using TOpenFrame based interface: cc_write
		//
        #ifdef CONFIG_PACKET_API
        txframe.payload[0]++;
        if (txframe.payload[0] == 5)
        { 
        	txframe.payload[0] = 1;
        } 
        
        led_twinkle( LED_GREEN, 1 );
        //length = cc2420_write( g_cc2420, &(txframe), 10+11, 0 );
        txframe.length = 10 + 11;
        length = cc2420_write( g_cc2420, &(txframe), 0 );
          
        /*
        if (length == -1) 
        {
        	led_twinkle(LED_RED,5);
        	uart_putchar(g_uart,(char)0x00);
        }
        else{
        	led_twinkle(LED_RED,1);
        	uart_putchar(g_uart,(char)0x11);
        }
        */ 
		halWait(2000);
		#endif
	  
	    // test section two: 
		// transmit using char buffer based interface
		// @modified by zhangwei on 20070418
		// huanghuan seems test the following section. i cannot guartantee whether 
		// the next char buffer based interface can work properly.
		//
		#ifdef CONFIG_FRAME_API         
        txbuf[10]++;
        if (txbuf[10] = 5) 
        {
        	txbuf[10] = 1;
        }
        cc2420_rawwrite( g_cc2420, (char *)txbuf, 10 + 11,0);
          
        led_twinkle(LED_GREEN,1);
	  	halWait(3000);	  
	  	#endif
	}
	
	global_destroy();
	return 0;															
}
