#include "hal_configall.h"
#include "hal_assert.h"
#include "hal_led.h"

void hal_assert( int v )
{
	if (!v)
	{
		while (true)
		{
			led_twinkle( LED_GREEN, 1 );
			led_twinkle( LED_YELLOW, 5 );
			led_twinkle( LED_RED, 20 );
		}
	}
}
