/******************************************************************************
 * light sensor 
 * @author zhangwei on 200907xx
 * @modified by xuxizhou in 200908xx
 *	- tested ok
 *****************************************************************************/

#include "hal_configall.h"
#include <string.h>
#include "hal_foundation.h"
#include "hal_uart.h"
#include "hal_led.h"
#include "hal_light.h"

/* Construct a empty TiLightSensor object in the memory */
TiLightSensor * light_construct( char * buf, uint16 size )
{
	hal_assert( sizeof(TiLightSensor) <= size );
	memset( buf, 0x00, size );
	return (TiLightSensor *)buf;
}

/* Destroy the TiLightSensor object */
void light_destroy( TiLightSensor * light )
{
	light_close( light );
}

/* Open the light sensor object for reading values. The sensor is usually powered 
 * on or wakeup from sleep mode in the sensor's open() function.
 * 
 * @attention
 * @todo
 *	The PIN initialization can be executed in target_init() or in light_open().
 * The following implementation assumes you have already initialized PIN configurations
 * before calling light_open() or in adc_open().
 */
TiLightSensor * light_open( TiLightSensor * light, uint8 id, TiAdcAdapter * adc )
{
	light->id = id;
	light->adc = adc;
	adc_open( adc, adc->id, NULL, NULL, 0 );
    return light;
}

/* Close the sensor. Usually power off the force the sensor module into sleep mode
 * in the close function 
 */
void light_close( TiLightSensor * light )
{
	adc_close( light->adc );
}

/* Read the sensor value. Ususally the measurement is already started in sensor's 
 * open() function. For those who can started immediately, you can also power on 
 * the sensor in this function and power off it after the measurement.
 */
uint16 light_value( TiLightSensor * light )
{
	uint16 value=0;
	uint8 count=0;
	adc_start( light->adc );
	while (count == 0)
	{
		// dbo_putchar( '*' );
		count = adc_read( light->adc, (char*)&value, sizeof(value), 0x00 );
		// dbo_putchar( '&' );
	}
	return value;
}

