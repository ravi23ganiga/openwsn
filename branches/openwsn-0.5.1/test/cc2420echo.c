 
#include "..\foundation.h"
#include "..\global.h"
#include "cc2420echo.h"

#define CC2420_BUF_SIZE 128

void cc2420_test( void )
{
	char memory[CC2420_BUF_SIZE];
	char * buf = (char *)( &memory[0] );
	uint8 rxlen, count;

	global_construct();
	
	memset( buf, 0x00, CC2420_BUF_SIZE ); 
	rxlen = 0;
	
	while (1)
	{
		//count = cc2420_read( g_cc2420, (char*)buf + rxlen, CC2420_BUF_SIZE - rxlen, 0 );
		rxlen += count;

		//count = cc2420_write( g_cc2420, buf, rxlen, 0 );
		if (count > 0)
		{
			rxlen -= count;
			memmove( buf, buf + count, rxlen );
		}
	}	
}


