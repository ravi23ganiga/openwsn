
/*****************************************************************************
 * @author zhangwei on 20070629
 * first created
 ****************************************************************************/

#include "../configall.h"
#include "../foundation.h"
#include "../hal/hal.h"
#include "../global.h"

void led_test( void )
{     
	int i;
	
	target_init();
	global_construct();
    led_init();
	
	// testing LED twinkle
	
	led_off( LED_RED ); 
	led_off( LED_YELLOW ); 
	led_off( LED_GREEN ); 

	led_twinkle( LED_YELLOW, 500 );
    led_twinkle( LED_RED, 500 );
    led_twinkle( LED_GREEN, 500 );
    
	hal_delay( 1000 );
	led_off( LED_RED ); 
	led_off( LED_YELLOW ); 
	led_off( LED_GREEN ); 
	
	// testing each single LED
	
	i=0;
	while (i<3)
	{
		led_off(LED_RED) ; 
	    hal_delay(500);
	    led_on(LED_RED) ; 
	    hal_delay(500);
	    i++;
    }
	
	i=0;
	while (i<3)
 	{
	    led_off(LED_GREEN) ; 
	    hal_delay(500);
	    led_on(LED_GREEN) ; 
	    hal_delay(500);
	    i++;
    }
    
	i=0;
    while (i<3)
 	{
	    led_off(LED_YELLOW) ; 
	    hal_delay(500);
	    led_on(LED_YELLOW) ; 
	    hal_delay(500);
	    i++;
	}
	
	led_off( LED_RED ); 
	led_off( LED_YELLOW ); 
	led_off( LED_GREEN ); 
	hal_delay( 1000 );
	
	// test the LEDs at the same time
	
	i = 0;
	while (i<3)
	{
		led_off( LED_RED ); 
		led_off( LED_YELLOW ); 
		led_off( LED_GREEN ); 
	    hal_delay(500);
		led_on( LED_RED ); 
		led_on( LED_YELLOW ); 
		led_on( LED_GREEN ); 
	    hal_delay(500);
		i++;
	}
	hal_delay( 1000 );
	
	led_off( LED_RED ); 
	led_off( LED_YELLOW ); 
	led_off( LED_GREEN ); 
	i = 0;
	while (i<3)
	{
		led_toggle( LED_RED ); 
		led_toggle( LED_YELLOW ); 
		led_toggle( LED_GREEN ); 
	    hal_delay(500);
		i++;
	}
	
	while (TRUE)
	{
		led_off( LED_RED ); 
		led_off( LED_YELLOW ); 
		led_off( LED_GREEN ); 
	    hal_delay(300);
		led_on( LED_RED ); 
		led_on( LED_YELLOW ); 
		led_on( LED_GREEN ); 
	    hal_delay(300);
		i++;
	}
	
	global_destroy();
}