
#include "..\foundation.h"
#include "..\hal\hal.h"
#include "..\service\svc.h"
#include "..\global.h"

void timer_callback( void * data );
void timer1_callback( void * data );

void timer_test( void )
{
	target_init();
	
//	uint8 VICcheck;

	global_construct();
        led_twinkle(LED_YELLOW,3);

        IRQEnable();

	timer_configure( g_timer0, (TEventHandler)timer_callback, NULL, 0 ); 
	timer_configure( g_timer2, (TEventHandler)timer1_callback,NULL, 1 );
    
//    timer_VICinit( g_timer0 );
//    timer_VICinit( g_timer1 );
//	if(g_timer0 -> callback == (TEventHandler)timer_callback)
//	{
//	    led_on( LED_RED );
//	}
//	if(g_timer0 -> id == 0x00) led_on( LED_RED );
	
//	timer_enable( g_timer0 );
/*	
	led_off( LED_RED );
	timer_setfuture( g_timer2, 5000 );
	while (!timer_expired( g_timer2 ))
		NULL;
	led_on( LED_RED );
	
	timer_disable( g_timer2 );
	timer_setperiod( g_timer2, 5000 ); 
	timer_enable( g_timer2 );
*/	
    led_twinkle(LED_YELLOW,3);
    timer_setinterval( g_timer0,4000,1 );
    timer_setinterval( g_timer2,1000,1 );
    led_twinkle(LED_YELLOW,3);
    timer_start( g_timer0 );
    timer_start( g_timer2 );
    led_twinkle(LED_YELLOW,3);
 //   timer_enable( g_timer0 );
 //   timer_enable( g_timer1 );
   
 //   timer_disable( g_timer0 );
 //   timer_disable( g_timer1 );
         while(1);
	

	global_destroy();
}

void timer_callback( void * data )
{
   // uint32 lastvalue,timervalue;
    
  //  timer_restart( g_timer1, 32, 1);
      led_toggle( LED_GREEN );
//	timer_stop( g_timer0 );
//	timer_disable( g_timer0 );
 //   timervalue = 0;
 //   timervalue = timer_elapsed( g_timer1 );
    
//    if((timervalue-Fpclk) <= 100 )
//        led_toggle( LED_GREEN );
/*
    if( timervalue > 0x02A30000)
    {	
        led_off( LED_GREEN );
    }
    else
    {
        led_on( LED_GREEN );
    }
*/        
 //   lastvalue = timervalue;	
}

void timer1_callback( void * data )
{
   // timer_disable( g_timer0 );
   //   timer_restart( g_timer0, 4, 1 );
   //   timer_enable( g_timer0 );
  //  timer_stop( g_timer0 );
   //   led_on( LED_RED );
      led_toggle( LED_RED );
    //  NULL;
}
