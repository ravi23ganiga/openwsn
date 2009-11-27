/*******************************************************************************
 * adc_light
 *
 * @state
 *	tested ok
 * @author xu-xizhou(TongJi University), zhangwei(TongJi University) in 200907
 *	- first created
 *
 ******************************************************************************/

#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_light.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_debugio.h"

TiAdcAdapter  g_adc;
TiLightSensor g_light;
TiUartAdapter g_uart;

int main(void)
{
	TiAdcAdapter * adc;
	TiLightSensor * light;
	TiUartAdapter * uart;
    char * welcome = "welcome to adc_light";
    uint16 val;

	// in the future, you should eliminate the following macro
	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );
	dbo_open(0, 38400);

	adc = adc_construct( (void *)&g_adc, sizeof(TiAdcAdapter) );
	light = light_construct( (void *)&g_light, sizeof(TiLightSensor) );
	uart = uart_construct( (void *)&g_uart, sizeof(TiAdcAdapter) );

	adc_open( adc, 0, NULL, NULL, 0 );
	light_open( light, 0, adc );
	uart_open( uart, 0, 38400, 8, 1, 0 );
    uart_write( uart, welcome, sizeof(welcome), 0x00 );
	uart_putchar( ':' );

	while (1)
	{
		dbo_putchar( '>' );
		val = light_value( light );
		dbo_n16toa( val );
		hal_delay( 1000 );
	}

	light_close( light );
	adc_close( adc );
	uart_close( uart );
}
