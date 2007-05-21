#ifndef _RANDOM_H_4387_
#define _RANDOM_H_4387_

#include "rtl_foundation.h"

void rand_seed();
uint32 rand_range( uint32 max_value );
//uint32 rand_next();
//uint32 rand_get( buf, size );
void rand_vector32( uint32 * buf, uint32 len );
void rand_vector16( uint16 * buf, uint32 len );

#endif
