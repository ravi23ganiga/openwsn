
#include "..\foundation.h"
#include "..\global.h"
#include "sniffer.h"

#define MAX_BUFFER_SIZE 128

// 站在PLC角度区分TX还是RX

void sniffer_run( void )
{
	char rxbuf[MAX_BUFFER_SIZE];
	char * buf;
	uint8 rxlen, count;

	global_construct();
	
	memset( (char*)(&rxbuf[0]), 0x00, MAX_BUFFER_SIZE ); 
	rxlen = 0;
	
	while (1)
	{
		buf = (char*)(rxbuf[0]);
		count = sio_write( g_sio, buf, rxlen, 0 );
		if (count > 0)
		{
			rxlen -= count;
			memmove( buf, buf + count, rxlen );
		}

		buf = (char*)(rxbuf[0]) + rxlen;
		count = wls_read( g_wls, buf, MAX_BUFFER_SIZE - rxlen, 0 );
		rxlen += count;
		
		// send debug data if exists.
		debug_evolve( g_debugio );
	}
		
	global_destroy();	
	
	return;
}

