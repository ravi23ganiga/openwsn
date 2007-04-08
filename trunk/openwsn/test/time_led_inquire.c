#include "..\foundation.h"
#include "..\hal\hal.h"
#include "..\global.h"

void timerled_test( void )
{
	target_init();
	
	global_construct();
        led_twinkle(LED_YELLOW,3);



	timer_configure( g_timer0, (TEventHandler)timer_callback, NULL, 0 ); 
	timer_configure( g_timer2, (TEventHandler)timer1_callback,NULL, 1 );
    

         led_twinkle(LED_YELLOW,3);
         timer_setinterval( g_timer0,4000,1 );
         timer_setinterval( g_timer2,1000,1 );
         led_twinkle(LED_YELLOW,3);
         timer_start( g_timer0 );
         timer_start( g_timer2 );
         led_twinkle(LED_YELLOW,3);

	while (1)
	{
	    
	    if( timer_expired(g_timer2) )
	    {
	        led_toggle( LED_RED );
	    }    
	    
	    if( timer_expired(g_timer0) )
	    {    
	        led_toggle( LED_GREEN );    
	    }    

	}

	global_destroy();
}

