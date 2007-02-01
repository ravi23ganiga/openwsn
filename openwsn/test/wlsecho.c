
#include "..\foundation.h"
#include "..\global.h"
#include "wlsecho.h"

#define MAX_BUFFER_SIZE 128

void wls_test( void )
{
	char memory[MAX_BUFFER_SIZE];
	char * buf = (char *)( &memory[0] );
	uint8 rxlen, count;

	global_construct();
	
	memset( buf, 0x00, MAX_BUFFER_SIZE ); 
	rxlen = 0;
	
	while (1)
	{
		count = wls_read( g_wls, (char*)buf + rxlen, MAX_BUFFER_SIZE - rxlen, 0 );
		rxlen += count;

		count = wls_write( g_wls, buf, rxlen, 0 );
		if (count > 0)
		{
			rxlen -= count;
			memmove( buf, buf + count, rxlen );
		}
	}	
	
	global_destroy();	
}


