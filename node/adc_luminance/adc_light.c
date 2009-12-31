/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
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
	uart = uart_construct( (void *)&g_uart, sizeof(TiUartAdapter) );

	// xiao-yuezhang changed the second parameter from 5 to 0
	//adc_open( adc, 0, NULL, NULL, 0 );
	adc_open( adc, 0, NULL, NULL, 0 );
	light_open( light, 0, adc );
	uart_open( uart, 0, 38400, 8, 1, 0 );
    uart_write( uart, welcome, strlen(welcome), 0x00 );


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
