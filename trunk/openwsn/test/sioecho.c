
#include "..\foundation.h"
#include "..\global.h"
#include "sioecho.h"

#define SIO_BUF_SIZE 128

void siocomm_test( void )
{
	char memory[SIO_BUF_SIZE];
	char * buf = (char *)( &memory[0] );
	uint8 rxlen, count;

	global_construct();
	memset( buf, 0x00, SIO_BUF_SIZE ); 
	rxlen = 0;
	
	while (1)
	{
		count = sio_read( g_sio, (char*)buf + rxlen, SIO_BUF_SIZE - rxlen, 0 );
		rxlen += count;

		count = sio_write( g_sio, buf, rxlen, 0 );
		if (count > 0)
		{
			rxlen -= count;
			memmove( buf, buf + count, rxlen );
		}
	}	
	global_destroy();
}


