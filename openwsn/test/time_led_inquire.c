#include "..\foundation.h"
#include "..\hal\hal.h"
#include "..\global.h"


void timerled_test( void )
{
	target_init();
	
	global_construct();
        led_twinkle(LED_YELLOW,3);
        
        timer_init(g_timer0,0,0);
        timer_init(g_timer2,1,0);

	timer_configure( g_timer0,NULL, NULL, 0 ); 
	timer_configure( g_timer2,NULL,NULL, 1 );
    

       
         timer_setinterval( g_timer0,4000,1 );
         timer_setinterval( g_timer2,1000,1 );
         led_twinkle(LED_RED,3);
         timer_start( g_timer0 );
         timer_start( g_timer2 );
         led_twinkle(LED_GREEN,3);
         led_twinkle(LED_YELLOW,3);

	while (1)
	{
	    
	    if( timer_expired(g_timer2) )
	    {
	        led_toggle( LED_RED );
	        led_off(LED_YELLOW) ; 
	    }    
	    
	    if( timer_expired(g_timer0) )
	    {    
	        led_toggle( LED_GREEN );  
	        led_on(LED_YELLOW) ; 
	    }    

	}

	global_destroy();
}


