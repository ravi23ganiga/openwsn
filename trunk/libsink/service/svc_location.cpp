#include "svc_location.h"

TLocationService * lcs_construct(char * buf, uint16 size)
{
	return NULL;
}

void lcs_destroy( TLocationService * lcs )
{
}

void lcs_init(TLocationService * lcs )
{
}

void lcs_rssi2dist( TLocationService * lcs, uint8 rssi, uint16 * dis )
{
}

int8 lcs_read( TLocationService * lcs, uint16 nodeid, TLocation * loc, uint8 opt )
{
	return 0;
}

int8 lcs_write( TLocationService * lcs, uint16 nodeid, TLocation * loc, uint8 opt )
{
	return 0;
}

int8 lcs_evolve( TLocationService * lcs )
{
	return 0;
}
