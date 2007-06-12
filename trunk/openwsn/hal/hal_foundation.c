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
#include "hal_foundation.h"

/******************************************************************************
 * @author zhangwei on 20070610
 * @description
 * "hal_foundation" module, contains some general utility functions used frequvently
 * in other modules.
 *  
 * @state
 *  developing
 * 
 * @modified by zhangwei on 20070610
 *   add hal_delay()
 *   add the interface functions on interrupt management
 *  
 *****************************************************************************/ 

void _hal_pause()
{
	static delaycounter=0;
	delaycounter ++;
}

/* @param
 * 	delay		delay period by millisecond
 * 				1000 means delay for 1 second
 * @attention
 *  you should porting this function to adapt to new MCU and hardware platforms!
 * 
 * @state
 * 	developing, needs testing!
 */
void hal_delay16( uint16 dly  )
{
	uint16 i;
   	for(; dly>0; dly--) 
		for(i=0; i<8000; i++)
			_hal_pause();	
}

void hal_delay32( uint32 dly  )
{
	uint32 i;
	for(; dly>0; dly--) 
    	for(i=0; i<8000; i++);
			_hal_pause();	
}

void hal_reboot( void )
{
}

void hal_sleep( void )
{}

void hal_sleepwait( uint32 delay )
{}

void hal_wakeup( void )
{}

void hal_powerdown( void )
{}





 