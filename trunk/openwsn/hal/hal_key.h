

#ifndef _KEY_H_5897_
#define _KEY_H_5897_

#include "hal_foundation.h"

#define KEY_RESET	1
#define KEY_CONFIG	2
#define KEY_TEST	3

#define KEY_ON 		0x01
#define KEY_OFF		0x02

uint8 key( uint8 idx );

#endif