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

#include "hal_foundation.h"
#include "hal_configall.h"

 
#define LED_GREEN 	0
#define LED_YELLOW 	1
#define LED_RED		2

#ifdef CONFIG_TARGET_OPENNODE_10
#define LED_GREEN_PIN 	25  
#define LED_YELLOW_PIN 	21 
#define LED_RED_PIN	21  

#define LED_GREEN_PORT 	1
#define LED_YELLOW_PORT 0
#define LED_RED_PORT	0
#endif

#ifdef CONFIG_TARGET_OPENNODE_20
#define LED_GREEN_PIN 	25  
#define LED_YELLOW_PIN 	18  
#define LED_RED_PIN	16  

#define LED_GREEN_PORT 	0
#define LED_YELLOW_PORT 1
#define LED_RED_PORT	1
#endif

#ifdef CONFIG_TARGET_WLSMODEM_11
#define LED_GREEN_PIN 	19  
#define LED_YELLOW_PIN 	19  
#define LED_RED_PIN	18  

#define LED_GREEN_PORT 	0
#define LED_YELLOW_PORT 0
#define LED_RED_PORT	0
#endif


 /* 	idx	= { LED_GREEN, LED_YELLOW, LED_RED } */

void led_on( uint8 idx );
void led_off( uint8 idx );
void led_toggle( uint8 idx );
void led_twinkle( uint8 idx , uint16 delay);

#endif
