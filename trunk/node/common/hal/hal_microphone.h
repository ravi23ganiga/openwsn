#ifndef _HAL_MICROPHONE_H_8723_
#define _HAL_MICROPHONE_H_8723_

/******************************************************************************
 * microphone sensor 
 * author xxx on 200907xx
 *
 *****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_adc.h"

typedef struct{
	uint8 id;
	TiAdcAdapter * adc;
}TiMicrophone;

TiMicrophone * mic_construct( char * buf, uint16 size );
void mic_destroy( TiMicrophone * mic );
TiMicrophone * mic_open( TiMicrophone * light, uint8 id, TiAdcAdapter * adc );
void mic_close( TiMicrophone * mic );
uint16 mic_value( TiMicrophone * mic, char * buf, uint16 size );


#endif