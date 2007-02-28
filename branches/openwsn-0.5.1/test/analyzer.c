
#include "..\foundation.h"
#include "..\global.h"
#include "..\service\svc_debugio.h"
#include "analyzer.h"

#define MAX_BUFFER_SIZE 128

// 站在PLC角度区分TX还是RX

void analyzer_run( void )
{
	char txbuf[MAX_BUFFER_SIZE];
	char rxbuf[MAX_BUFFER_SIZE];
	char * buf;
	uint8 txlen, rxlen, count;

	global_construct();
	
	memset( (char*)(&txbuf[0]), 0x00, MAX_BUFFER_SIZE ); 
	memset( (char*)(&rxbuf[0]), 0x00, MAX_BUFFER_SIZE ); 
	txlen = 0;
	rxlen = 0;
	
	while (1)
	{
		buf = (char*)(txbuf[0]);
		count = wls_write( g_wls, buf, txlen, 0 );
		if (count > 0)
		{
			txlen -= count;
			memmove( buf, buf + count, txlen );
		}
		
		buf = (char*)(rxbuf[0]);
		count = sio_write( g_sio, buf, rxlen, 0 );
		if (count > 0)
		{
			rxlen -= count;
			memmove( buf, buf + count, rxlen );
		}

		buf = (char*)(txbuf[0]) + txlen;
		count = sio_read( g_sio, buf, MAX_BUFFER_SIZE - txlen, 0 );
		txlen += count;
		
		buf = (char*)(rxbuf[0]) + rxlen;
		count = wls_read( g_wls, buf, MAX_BUFFER_SIZE - rxlen, 0 );
		rxlen += count;
		
		// send debug data if exists.
		debug_backgnd_sending( g_debugio );
	}
		
	global_destroy();	
	
	return;
}


