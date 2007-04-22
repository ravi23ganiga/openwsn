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
//#include "config.h"
//#include "start.h"

/*****************************************************************************
 * @author MaKun on 2007-04-18
 * test sending/TX function of cc2420 driver
 *
 * @modified by zhangwei on 20070418
 * just modify the format of the source file and including files
 *   
 ****************************************************************************/ 

#define PACKET
//#define CHAR_STREAM

static uint8 tx_frame[128];
static uint8 rx_frame[128];            //using CHAR_STREAM

static TCc2420Frame tx_test;
static TCc2420Frame rx_test;           //using PACKET

int cc2420tx_test (void)
{
    ///*     Fof RF test	
    uint8 n;
    int8 length;
    uint16 temp;
    uint8 ledPeriod;
    char * out_string = "the rssi value is : ";
    char * enter      = "\n";
  
    target_init();
    
    global_construct();
    spi_configure( g_spi );
    uart_configure( g_uart, 115200, 0, 0, 0, 0 );
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0);
    
    tx_test.panid = 0x2420;
    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, tx_test.panid, 0);
    
    tx_test.nodeto = 0x5678;
    tx_test.nodefrom = 0x1234;
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, tx_test.nodefrom, 0);
    
    for (n = 0; n < 10; n++) {
        tx_test.payload[n] = 2;
        tx_frame[10 + n] = 2;
    }
    tx_test.payload[2] = 1;  

    tx_frame[1] = tx_frame[2] = tx_frame[3] = 0;
    tx_frame[4] = 0x20; tx_frame[5] = 0x24;
    tx_frame[6] = 0x78; tx_frame[7] = 0x56;
    tx_frame[8] = 0x34; tx_frame[9] = 0x12; 
    
    
    cc2420_receive_on(g_cc2420);  
    IRQEnable(); 
    
	while (TRUE) 
	{    
		// transmit using TOpenFrame based interface
        #ifdef PACKET
        tx_test.payload[0]++;
        if (tx_test.payload[0] == 5)
        { 
        	tx_test.payload[0] = 1;
        } 
        
        led_twinkle(LED_GREEN,1);
        length = cc2420_write(g_cc2420,tx_test,10 + 11,0);
          
        /*
        if(length == -1) {led_twinkle(LED_RED,5);uart_putchar(g_uart,(char)0x00);}
        else {led_twinkle(LED_RED,1);uart_putchar(g_uart,(char)0x11);}
        */ 
		halWait(2000);
		#endif
	  
		// transmit using char buffer based interface
		// @modified by zhangwei on 20070418
		// huanghuan seems test the following section. i cannot guartantee whether 
		// the next char buffer based interface can work properly.
		#ifdef CHAR_STREAM         
        tx_frame[10]++;
        if (tx_frame[10] = 5) 
        {
        	tx_frame[10] = 1;
        }
        cc2420_rawwrite( g_cc2420, (char *)tx_frame, 10 + 11,0);
          
        led_twinkle(LED_GREEN,1);
       	//cc2420_sendframe(g_cc2420);
	  	halWait(3000);	  
	  	#endif

	}
	
	global_destroy();
	return 0;															
}
