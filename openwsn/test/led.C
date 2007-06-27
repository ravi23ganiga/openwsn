#include "..\configall.h"
#include "..\foundation.h"
#include "..\hal\hal.h"
#include "..\global.h"

void led_test( void )
{    int i;
	target_init();
	global_construct();
    led_twinkle(LED_YELLOW,3);
    led_twinkle(LED_RED,3);
    led_twinkle(LED_GREEN,3);
        i=0;
	while (i<5)
	{
	        led_off(LED_RED) ; 
	        hal_delay(3000);
	        led_on(LED_RED) ; 
	        hal_delay(3000);
	        i++;
    }
	
	i=0;
	while (i<6)
 	{
	        led_off(LED_GREEN) ; 
	        hal_delay(3000);
	        led_on(LED_GREEN) ; 
	        hal_delay(3000);
	        i++;

    }
    
	i=0;
    while (i<8)
 	{
	        led_off(LED_YELLOW) ; 
	        hal_delay(3000);
	        led_on(LED_YELLOW) ; 
	        hal_delay(3000);
	        i++;
	}
	global_destroy();
}