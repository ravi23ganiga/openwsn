/******************************************************************************
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
 *****************************************************************************/ 

#include "hal_configall.h"
#include <string.h>
#include "hal_foundation.h"
#include "hal_interrupt.h"
#include "hal_assert.h"
#include "hal_led.h"
#include "hal_debugio.h"


/******************************************************************************
 * @attention
 * Q: why some times the program switch on/off the LED without any expected order?
 * R: 
 *  when assert() failed, all the LED should ON/OFF at the same time. 
* however, if your program enable the interrupts, and you have ON/OFF operations in the 
* interrupts, then you will see this phenomena.
*   as a suggest: you'd better not switch the LED in interrupt service routine unless you 
* know your results.
* @ I have been changed "while" in line 67 with "if " to keep the process going on --Shimiaojing  
 *****************************************************************************/ 
 
//void _hal_assert( int cond, char * file, uint16 line )
void _hal_assert( bool cond, char * file, uint16 line )
{
	char * msg = "assert:";
	while (!cond)
	{
		hal_disable_interrupts(); 

		dbo_write( msg, strlen(msg) );
		dbo_write( file, strlen(file) );
		dbo_putchar( '(' );
		dbo_putchar( '0' );
		dbo_putchar( 'x' );
		dbo_n16toa( line );
		dbo_putchar( ')' );

		if (true)
		{
			led_on( LED_RED );
			hal_delay( 500 );
			led_off( LED_RED );
			led_on( LED_GREEN );
			hal_delay( 500 );
			led_off( LED_GREEN );
			led_on( LED_YELLOW );
			hal_delay( 500 );
			led_off( LED_YELLOW );

		}
	}
}
