#ifndef _HAL_LIGHT_H_8723_
#define _HAL_LIGHT_H_8723_

/******************************************************************************
 * light sensor 
 * @author zhangwei on 200907xx
 * @modified by xuxizhou in 200908xx
 *	- tested ok
 *****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_adc.h"

/* THIS MODULE IS REPLACED by hal_luminance.* */

typedef struct{
	uint8 id;
	TiAdcAdapter * adc;
}TiLightSensor;

TiLightSensor * light_construct( char * buf, uint16 size );
void light_destroy( TiLightSensor * light );
TiLightSensor * light_open( TiLightSensor * light, uint8 id, TiAdcAdapter * adc );
void light_close( TiLightSensor * light );
uint16 light_value( TiLightSensor * light );


#endif