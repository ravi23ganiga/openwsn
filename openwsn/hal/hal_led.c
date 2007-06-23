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
#include "hal_foundation.h"
#include "hal_led.h"
#include "hal_cpu.h"

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

#ifdef CONFIG_TARGET_OPENNODE_30
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


/*
void led( uint8 id, uint8 state )
{
	
}
*/
void led_off( uint8 id )
{
	switch(id)
	{
	    case LED_GREEN:    
	         #if LED_GREEN_PORT == 0 
	         IO0SET  = BM(LED_GREEN_PIN);    break;
	         #endif
	         
	         #if LED_GREEN_PORT == 1 
	         IO1SET  = BM(LED_GREEN_PIN);    break;
	         #endif
	         
	    case LED_RED:    
	         #if LED_RED_PORT == 0 
	         IO0SET  = BM(LED_RED_PIN);      break;
	         #endif
	         
	         #if LED_RED_PORT == 1 
	         IO1SET  = BM(LED_RED_PIN);      break;
	         #endif
	         
	    case LED_YELLOW:   
	         #if LED_YELLOW_PORT == 0 
	         IO0SET  = BM(LED_YELLOW_PIN);   break;
	         #endif
	         
	         #if LED_YELLOW_PORT == 1 
	         IO1SET  = BM(LED_YELLOW_PIN);   break;
	         #endif
	         
	}
}

void led_on( uint8 id )
{
	switch(id)
	{
	    case LED_GREEN:    
	         #if LED_GREEN_PORT == 0 
	         IO0CLR  = BM(LED_GREEN_PIN);   break;
	         #endif
	         
	         #if LED_GREEN_PORT == 1 
	         IO1CLR  = BM(LED_GREEN_PIN);   break;
	         #endif
	         
	    case LED_RED:    
	         #if LED_RED_PORT == 0 
	         IO0CLR  = BM(LED_RED_PIN);     break;
	         #endif
	         
	         #if LED_RED_PORT == 1 
	         IO1CLR  = BM(LED_RED_PIN);     break;
	         #endif
	         
	    case LED_YELLOW:   
	         #if LED_YELLOW_PORT == 0 
	         IO0CLR  = BM(LED_YELLOW_PIN);   break;
	         #endif
	         
	         #if LED_YELLOW_PORT == 1 
	         IO1CLR  = BM(LED_YELLOW_PIN);   break;
	         #endif
	         
	}
}

void led_toggle( uint8 id )
{
	switch(id)
	{
	    case LED_GREEN:    
	         {
	         	#if LED_GREEN_PORT == 0
	         	if(IO0SET & BM(LED_GREEN_PIN))  IO0CLR  = BM(LED_GREEN_PIN); 
	         	else                            IO0SET  = BM(LED_GREEN_PIN);   	     
	         	break;
	         	#endif
	         	
	         	#if LED_GREEN_PORT == 1
	         	if(IO1SET & BM(LED_GREEN_PIN))  IO1CLR  = BM(LED_GREEN_PIN); 
	         	else                            IO1SET  = BM(LED_GREEN_PIN);   	     
	         	break;
	         	#endif
	         }
	    case LED_RED:      
	         {
	         	#if LED_RED_PORT == 0
	         	if(IO0SET & BM(LED_RED_PIN))  IO0CLR  = BM(LED_RED_PIN); 
	         	else                          IO0SET  = BM(LED_RED_PIN);   	     
	         	break;
	         	#endif
	         	
	         	#if LED_RED_PORT == 1
	         	if(IO1SET & BM(LED_RED_PIN))  IO1CLR  = BM(LED_RED_PIN); 
	         	else                          IO1SET  = BM(LED_RED_PIN);   	     
	         	break;
	         	#endif
	         }
	    case LED_YELLOW:   
	         {
	         	#if LED_YELLOW_PORT == 0
	         	if(IO0SET & BM(LED_YELLOW_PIN))  IO0CLR  = BM(LED_YELLOW_PIN); 
	         	else                             IO0SET  = BM(LED_YELLOW_PIN);   	     
	         	break;
	         	#endif
	         	
	         	#if LED_YELLOW_PORT == 1
	         	if(IO1SET & BM(LED_YELLOW_PIN))  IO1CLR  = BM(LED_YELLOW_PIN); 
	         	else                             IO1SET  = BM(LED_YELLOW_PIN);   	     
	         	break;
	         	#endif
	         }
	}
}

void led_twinkle( uint8 id ,uint16 delay)
{
  	switch(id)
	{
	    case LED_GREEN:
	         {
	         	led_toggle(LED_GREEN);
	                halWait( delay << 7 );	
	                led_toggle(LED_GREEN);
	                halWait( delay << 7 );
	                break;
	         }
	    case LED_RED:
	         {
	         	led_toggle(LED_RED);
	                halWait( delay << 7 );	
	                led_toggle(LED_RED);
	                halWait( delay << 7 );
	                break;
	         }
	    case LED_YELLOW:
	         {
	         	led_toggle(LED_YELLOW);
	                halWait( delay << 7 );	
	                led_toggle(LED_YELLOW);
	                halWait( delay << 7 );
	                break;
	         }	
	}
}


