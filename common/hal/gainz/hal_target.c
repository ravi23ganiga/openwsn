#ifndef _HAL_TARGET_C_7483_
#define _HAL_TARGET_C_7483_

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_target.h"
//#include "hpl_atmega128.c" 

void target_init( void )
{
	hal_init( NULL, NULL );
	HAL_SET_PIN_DIRECTIONS();
}

void target_reset( void )
{
	// todo
	target_init();
}

#endif
