#include "svc_foundation.h"
#include "svc_openpacket.h"

char * opt_ownerframe( char * packet )
{
	return packet-1;
}

char * opt_data( char * packet )
{
	return packet+8;
	
}

