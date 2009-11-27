
/*****************************************************************************
 * @author zhangwei on 20070629
 * - first created
 * @modified by zhangwei on 20090707
 * - hal_led module was modified by shi.miaojing(tongji university)
 ****************************************************************************/

#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_target.h"
#include "ledtest.h"

void led_test( void )
{     
	int i;

    target_init();

    led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	
	/* testing each single LED */

	led_off( LED_ALL ); 
	i=0;
	while (i<3)
	{
		led_off(LED_RED) ; 
	    hal_delay(300);
	    led_on(LED_RED) ; 
	    hal_delay(300);
	    i++;
    }
    led_off( LED_RED );
	
	i=0;
	while (i<3)
 	{
	    led_off(LED_GREEN) ; 
	    hal_delay(300);
	    led_on(LED_GREEN) ; 
	    hal_delay(300);
	    i++;
    }
    led_off( LED_GREEN );
    
	i=0;
    while (i<3)
 	{
	    led_off(LED_YELLOW) ; 
	    hal_delay(300);
	    led_on(LED_YELLOW) ; 
	    hal_delay(300);
	    i++;
	}
    led_off( LED_YELLOW );

	/* test the LEDs at the same time */
	
	led_off( LED_ALL ); 
	i = 0;
	while (i<3)
	{
		led_toggle( LED_RED ); 
		led_toggle( LED_YELLOW ); 
		led_toggle( LED_GREEN ); 
	    hal_delay(500);
		i++;
	}

	/* testing LED twinkle 
     * there's a infinite loop in led_twinkle, so it will never come out*/

	led_off( LED_ALL ); 
    // led_twinkle( LED_RED, 300, 5 );
    // led_off( LED_RED );
	led_twinkle( LED_YELLOW, 300, 5 );
    led_off( LED_YELLOW );
    // led_twinkle( LED_GREEN, 300, 5 );
    // led_off( LED_GREEN );
}
