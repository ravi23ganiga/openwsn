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
#include "../foundation.h"
#include "../hal/hal.h"
#include "../service/svc_openmac.h"
#include "blink.h"

#define PANID 0x2420
#define WAITFOR_MASTER_DURATION 3000
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
 

    target_init();
    global_construct();
    spi_configure( g_spi );
    txframe.nodefrom=0x1234;
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, PANID, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, txframe.nodefrom, 0);
    //cc2420_open();

	timer_configure( g_timer1, NULL, NULL,1);	//?how much about timetimer->data timer->priority
	state = MODE_SLAVE;
	timer_setinterval( g_timer1, WAITFOR_MASTER_DURATION,1 );

	while (1)
	{
		memset( &rxframe, 0x00, sizeof(TOpenFrame) );
		memset( &txframe, 0x00, sizeof(TOpenFrame) );

		switch (state)
		{
		// the GREEN LED on indicates the current node running in slave mode
		case MODE_SLAVE:
			led_off( LED_GREEN );
			if (cc2420_read(g_cc2420, &rxframe, sizeof(rxframe), 0x00) > 0)
			{
				if (rxframe.payload[0] == 0)
					led_off( LED_RED );
				else
					led_on( LED_RED );
				
				timer_restart( g_timer, WAITFOR_MASTER_DURATION, NULL );
			}
			
			if (timer_expired(g_timer))
			{
				timer_restart( g_timer, MASTER_BROADCASR_INTERVAL,NULL );
				state = MODE_MASTER;
			}
			break;
			
		// the green LED off indicates the current node running in master mode
		case MODE_MASTER:
			led_on( LED_GREEN );
			if (timer_expired(g_timer))
			{
				//txframe.payload[0] = !led_state();?led_state() does not exist
				cc2420_write( g_cc2420, &txframe, sizeof(txframe), 0x01 );
				timer_restart( g_timer, MASTER_BROADCASR_INTERVAL ,NULL);
			}
			
			if (cc2420_read(g_cc2420,&rxframe, sizeof(rxframe), 0x01 ) > 0)
			{
				timer_restart( g_timer, WAITFOR_MASTER_DURATION,NULL );
				state = MODE_SLAVE;			
			} 
			break;
		}
	}
}


