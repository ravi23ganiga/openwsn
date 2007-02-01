#include "hal_foundation.h"
#include "hal_key.h"

/* @TODO 20061015
 * you should read the hardware state to decide whether the key's status 
 * is on or not. 
 */
uint8 key( uint8 idx )
{
	uint8 state;
	
	switch (idx)
	{
	case KEY_RESET: 
		state = KEY_OFF;
		break;
		
	case KEY_CONFIG:
		state = KEY_OFF;
		break;
		
	case KEY_TEST:
		state = KEY_ON;
		break;
		
	default:
		state = KEY_OFF;
	}
	return state;
}
