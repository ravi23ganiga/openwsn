/*
 * rtl_vectorx.c
 *
 * Created on: 2009-3-29
 *      Author: Allen
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_vectorx.h"

/* TiVector memory structure
 * [HEADER][ITEM STATE ARRAY][ITEM DATA ARRAY]
 */
TiVectorX * vex_construct( void * buf, uint16 size, uint16 itemlen )
{
	TiVectorX * vex;

	assert( sizeof(TiVectorX) < size );
	assert( itemlen > 0 );

	vex = (TiVectorX *)buf;
	vex->size = size;
	vex->itemlen = itemlen;
	vex->capacity = (size - sizeof(TiVectorX)) / (itemlen+1);
	vex->count = 0;
	vex->index = 0;
	vex->offset = sizeof(TiVectorX) + vec->capacity;
}

void vex_destroy( TiVectorX * vex )
{
	return;
}

uint16 vex_capacity( TiVectorX * vex )
{
	return vex->capacity;
}

uint16 vex_count( TiVectorX * vex )
{
	return vex->count;
}

bool vex_empty( TiVectorX * vex )
{
	return vex->count = 0;
}

bool vex_full( TiVectorX * vex )
{
	return vex->count = vex->capacity;
}

bool vex_apply( TiVectorX * vex, uint16 * idx )
{
	uint16 i;
	uint8 * pstate;
	bool found=false;

	for (i=vex->count; i<vex->capacity; i++)
	{
		pstate = vex_states(vex) + idx;
		if (*pstate == 0)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		for (i=0; i<vex->count; i++)
		{
			pstate = vex_states(vex) + idx;
			if (*pstate == 0)
			{
				found = true;
				break;
			}
		}
	}

	if (found)
	{
		*idx = i;
		*pstate = 1;
		vex->count ++;
	}

	return found;
}

void vex_release( TiVectorX * vex, uint16 idx )
{
	vex_state(vex)[idx] = 0;
}

bool vex_isavailable( TiVectorX * vex, uint16 idx )
{
	return (vex_state(vex, idx) == 0);
}

uint8 * vex_states( TiVector * vex )
{
	return (uint8 *)vex + sizeof(TiVector);
}

uint8 vex_state( TiVectorX * vex, uint16 idx )
{
	return *((uint8 *)vex + sizeof(TiVector) + idx);
}

void * vex_items( TiVectorX * vex )
{
	return (char*)vex + vex->offset;
}

void * vex_getbuf( TiVectorX * vex, uint16 idx )
{
	return (char*)vex + vex->offset + idx * vex->itemsize;
}

bool vex_get( TiVectorX * vex, uint16 idx, void * buf )
{
	void * data;

	assert( idx < vex->capacity );
	if (vex_state(vex,idx) == 1)
	{
		data = vex_getbuf(vex, idx);
		memmove( buf, data, vex->itemsize );
		return true;
	}
	else
		return false;
}

bool vex_set( TiVectorX * vex, uint16 idx, void * item )
{
	void * data;

	assert( idx < vex->capacity );

	data = vex_getbuf(vex, idx);
	memmove( data, buf, vex->itemsize );
	vex_state(vex)[idx] = 1;
	return true;
}

bool vex_put( TiVectorX * vex, void * item )
{
	uint8 idx;
	
	if (vex_apply(vex, &idx))
	{
		vex_set(vex,idx,item);
		return true;
	}
	else
		return false;
}

bool vex_remove( TiVectorX * vex, uint16 idx )
{
	uint8 * state;
	state = vex_states(vex) + idx;
	if (*state != 0)
	{
		vex->count --;
		*state = 0;
	}
}

void vex_clear( TiVectorX * vex )
{
	uint8 * state;
	uint16 i;

	state = vex_states(vex);
	for (i=0; i<vex->capacity; i++)
	{
		state[i] = 0;
	}
	vex->count = 0;
}

void vex_swap( TiVectorX * vex, uint16 i, uint16 j )
{
	uint8 state1 = vex_state(vex, i);
	uint8 state2 = vex_state(vex, j);
	char * p1 = vex_getbuf(vex, i);
	char * p2 = vex_getbuf(vex, j);

	if ((state1 != 0) && (state2 != 0))
	{
		for (k=0; k<vex->itemlen; k++)
		{
			p1 += k;
			p2 += k;
			*p1 = (*p1)^(*p2);
			*p2 = (*p2)^(*p1);
			*p1 = (*p1)^(*p2);
		}
	}
	else if (state1 != 0) && (state2 == 0))
	{
		memmove( p2, p1, vex->itemlen );
		vex_state(vec)[i] = 0;
		vex_state(vec)[j] = 1;
	}
	else if (state1 == 0) && (state2 != 0))
	{
		memmove( p1, p2, vex->itemlen );
		vex_state(vec)[i] = 1;
		vex_state(vec)[j] = 0;
	}
}
