
#ifndef _LED_H_1329_
#define _LED_H_1329_

#include "hal_foundation.h"
#include "hal_configall.h"

 
#define LED_GREEN 	0
#define LED_YELLOW 	1
#define LED_RED		2

#ifdef TARGET_OPENNODE_10
#define LED_GREEN_PIN 	25  
#define LED_YELLOW_PIN 	21 
#define LED_RED_PIN	21  

#define LED_GREEN_PORT 	1
#define LED_YELLOW_PORT 0
#define LED_RED_PORT	0
#endif

#ifdef TARGET_OPENNODE_20
#define LED_GREEN_PIN 	25  
#define LED_YELLOW_PIN 	18  
#define LED_RED_PIN	16  

#define LED_GREEN_PORT 	0
#define LED_YELLOW_PORT 1
#define LED_RED_PORT	1
#endif

#ifdef TARGET_WLSMODEM_11
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
