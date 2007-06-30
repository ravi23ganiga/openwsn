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
#ifndef _LED_H_1329_
#define _LED_H_1329_

/*****************************************************************************
 * @modified by zhangwei on 20070623
 * add support to OpenNODE 3.0 hardware 
 * add function led_init();
 ****************************************************************************/ 
 
#include "hal_configall.h"
#include "hal_foundation.h"
 
#define LED_ALL 	0
#define LED_GREEN 	1
#define LED_YELLOW 	2
#define LED_RED		3

/*****************************************************************************
 * the parameter "id" should be the macro defined above
 *	id	= { LED_GREEN, LED_YELLOW, LED_RED } 
 *
 * @attention: you must call led_init() before you call other LED functions
 * @attention: you should NOT use "led_twinkle()" in interrupts. because it may 
 *	lead to quite a long delay.
 ****************************************************************************/ 

void led_init( void ); 
void led( uint8 id, bool state );
void led_on( uint8 id );
void led_off( uint8 id );
void led_toggle( uint8 id );
void led_twinkle( uint8 id , uint16 interval );

#endif
