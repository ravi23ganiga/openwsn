
#include "..\foundation.h"
#include "..\hal\hal.h"
#include "..\service\svc.h"
#include "..\global.h"

static void  timer_callback( void * data );
static void timer1_callback( void * data );

void timerupt_test( void )
{
	target_init();
	led_init();

	global_construct();
	timer_init(g_timer0,0,0);
        timer_init(g_timer2,1,0);
        led_twinkle(LED_YELLOW,3);

        IRQEnable();

	timer_configure( g_timer0, (TEventHandler)timer_callback, NULL, 0 ); 
	timer_configure( g_timer2, (TEventHandler)timer1_callback,NULL, 1 );
    

    led_twinkle(LED_YELLOW,3);
    timer_setinterval( g_timer0,4000,1 );
    timer_setinterval( g_timer2,1000,1 );
    led_twinkle(LED_YELLOW,3);
    timer_start( g_timer0 );
    timer_start( g_timer2 );
    led_twinkle(LED_YELLOW,3);

         while(1);
	

	global_destroy();
}

void timer_callback( void * data )
{
  
      led_toggle( LED_GREEN );

}

void timer1_callback( void * data )
{
  
      led_toggle( LED_RED );
 
}
