
#include "..\foundation.h"
#include "..\hal\hal.h"
#include "..\service\svc.h"
#include "..\global.h"

void timer_callback( void * data );
void timer1_callback( void * data );

void timer_test( void )
{
	uint8 VICcheck;
	uint32 value;
	char * enter      = "\n";
	
	target_init();
	global_construct();
	

    IRQEnable();
        timer_init(g_timer0,0,0);
        timer_init(g_timer1,1,0);
	timer_configure( g_timer0, (TEventHandler)timer_callback, NULL, 0 ); 
	timer_configure( g_timer1, (TEventHandler)timer1_callback,NULL, 1 );
	
	timer_start( g_timer1 );
	while(1)
	{
	    value = timer_getvalue( g_timer1 );	
            uart_putchar(g_uart,(char)(value));
            uart_putchar(g_uart,*enter);
            led_twinkle(LED_RED,4);
	}
    
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
    timer_setinterval( g_timer0,1000,1 );
    timer_setinterval( g_timer1,4000,1 );
    
    timer_start( g_timer0 );
    timer_start( g_timer1 );
 //   timer_enable( g_timer0 );
 //   timer_enable( g_timer1 );
   
 //   timer_disable( g_timer0 );
 //   timer_disable( g_timer1 );
	while(1);
	while (1)
	{
	    
	    if( timer_expired(g_timer1) )
	    {
	        led_toggle( LED_RED );
	    }    
	    if( timer_expired(g_timer0) )
	        led_toggle( LED_GREEN );    
//	   VICcheck = VICRawIntr & (1<<4);
//	   if (VICcheck == (1<<4))     
//	        led_toggle( LED_RED );
	}

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
