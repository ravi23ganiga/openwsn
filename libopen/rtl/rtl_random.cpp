#include "rtl_random.h"
#include <stdlib.h>
#include <time.h>

void rand_init()
{
	srand( (unsigned)time( NULL ) );
}

uint32 rand_range( uint32 max_value )
{
	return rand() % max_value; 
}

// this function will generate a vector. the value in this vector is from 0 to len-1. 
// however, the positions they in the buf are random.
//
void rand_vector32( uint32 * buf, uint32 len )
{
	uint32 n, i, j, t;
	for (n=0; n<len; n++)
	{
		buf[n] = n;
	}

	for (n=0; n<len; n++)
	{
		i = rand_range( len );
		j = rand_range( len );
		if (i != j)
		{
			t = buf[i];
			buf[i] = buf[j];
			buf[j] = t;
		}
	}
}

void rand_vector16( uint16 * buf, uint32 len )
{
	uint32 n, i, j;
	uint16 t;
	for (n=0; n<len; n++)
	{
		buf[n] = (uint16)n;
	}

	for (n=0; n<len; n++)
	{
		i = rand_range( len );
		j = rand_range( len );
		if (i != j)
		{
			t = buf[i];
			buf[i] = buf[j];
			buf[j] = t;
		}
	}
}
