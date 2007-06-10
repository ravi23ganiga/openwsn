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

#include "../configall.h"
#include "../hal/hal_cc2420.h"
#include "../service/svc_openmac.h"
#include "../foundation.h"
#include "../global.h"
#include "blink.h"

#define PANID 0x2420
#define WAITFOR_MASTER_DURATION 5000
#define MASTER_BROADCASR_INTERVAL 2000

/* the booting up process of a node contains at least the following three stages:
 * 
 * 1 power on and self test if possible
 * 2 repeat SEARCHING for master node for some time. if it can receive a message
 *   from its master, then it will flash the led according to the message received.
 * 3 if it cannot find a master node, then itself will become a master node by 
 * 	 broadcasting the MASTER message.  the master node should broadcast the 
 * 	 MASTER message with a random interval. so the final user can understand 
 *   the slave nodes are under full control of the master nodes.
 * 
 * there should only one master node among a group. so if the current node receive
 * a MASTER message, it should come into slave mode automatically.
 */

void blink_test()
{
	enum {MODE_MASTER=0, MODE_SLAVE};
	
	TOpenFrame txframe;
	TOpenFrame rxframe;
	uint8 state;
	uint8 n;
	
    target_init();
    global_construct();
    spi_configure( g_spi );
    memset( &rxframe, 0x00, sizeof(TOpenFrame) );
    memset( &txframe, 0x00, sizeof(TOpenFrame) );

    txframe.panid = PANID;
    txframe.nodeto = 0x1234;
    txframe.nodefrom=0x5678;
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, PANID, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, txframe.nodefrom, 0);
     for (n = 0; n < 10; n++) 
    {
        txframe.payload[n] = 2;
    }
    uart_configure( g_uart, 115200, 0, 0, 0, 0 );
    cc2420_open( g_cc2420 );
    timer_init(g_timer1,0,0);
    timer_configure( g_timer1,NULL, NULL, 0 ); 
    state = MODE_SLAVE;
    timer_setinterval( g_timer1,WAITFOR_MASTER_DURATION,1 );
    //timer_setinterval( g_timer1, WAITFOR_MASTER_DURATION,1 );
    timer_start( g_timer1 );
//	uart_write( g_uart, "uartecho run1..", 15, 0 );
    

	while (1)
	{
		
		switch (state)
		{
		// the GREEN LED off indicates the current node running in slave mode
		case MODE_SLAVE:
			led_on( LED_GREEN );
			hal_delay(1000);
		 cc2420_receive_on(g_cc2420);  
                  IRQEnable(); 
			//uart_write( g_uart, "run6..", 7, 0 );

			if (cc2420_read(g_cc2420, &rxframe, 0) > 0)
			{      
			    if (rxframe.payload[0] == 0)
					led_off( LED_RED );
				else
					led_on( LED_RED );
				hal_delay(1000);
				timer_restart( g_timer1, WAITFOR_MASTER_DURATION, 1);
			    uart_write( g_uart, "run2..", 7, 0 );

			}
			
			if (timer_expired(g_timer1))
			{
				timer_restart( g_timer1, MASTER_BROADCASR_INTERVAL,1 );
				state = MODE_MASTER;
				//led_toggle( LED_YELLOW );
				uart_write( g_uart, "run3..", 7, 0 );
	
			}
			break;
			
		// the green LED on indicates the current node running in master mode
		case MODE_MASTER:
			led_off( LED_GREEN );
			hal_delay(1000);
			//uart_write( g_uart, "run7..", 7, 0 );
			if (timer_expired(g_timer1))
			{  
				txframe.payload[0] = !txframe.payload[0];
				//cc2420_write( g_cc2420, &txframe, 11+10, 0x01 );
				cc2420_write( g_cc2420, &txframe, 0x01 );
				timer_restart( g_timer1, MASTER_BROADCASR_INTERVAL ,1);
				uart_write( g_uart, "run4..", 7, 0 );

			}
			cc2420_read(g_cc2420,&rxframe, 0 ) ;
			
				timer_restart( g_timer1, WAITFOR_MASTER_DURATION,1 );
				state = MODE_SLAVE;	
				uart_write( g_uart, "run5..", 7, 0 );
		
			/* 
			if (cc2420_read(g_cc2420,&rxframe, 0, 0 ) > 0)
			{
				timer_restart( g_timer1, WAITFOR_MASTER_DURATION,1 );
				state = MODE_SLAVE;	
				uart_write( g_uart, "run5..", 7, 0 );
		
			} */
			break;
		}
	}
}


