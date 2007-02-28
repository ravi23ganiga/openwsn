//----------------------------------------------------------------------------
// @author zhangwei on 2006-10-25
// @note simpelmodem V 1.0
//----------------------------------------------------------------------------

#include <string.h>
#include "..\foundation.h"
#include "..\hal\hal_foundation.h"
#include "..\hal\hal_key.h"
#include "..\service\svc_foundation.h"
#include "..\service\svc_siocomm.h"
#include "..\service\svc_wlscomm.h"
#include "..\service\svc_debugio.h"
#include "..\service\svc_configure.h"
#include "..\global.h"
#include "wlsmodem.h"

#define MODEM_BUFFER_SIZE 0xFF

#ifdef GDEBUG
static char * m_debugwelcome = "Wireless Modem Started...";
#endif

// Q: how to distinguish TX and RX?
// R: 站在PLC/Host角度区分术语 TX 还是 RX
// 队列中每一个元素都是一个有头有尾的frame
// 
// @note
// @TODO you'd better replace the current "txbuf" and "rxbuf" with an queue 
// to improve the performance. every item in the queue is an entire frame's 
// payload. 
//
void simplemodem_start( void )
{
	char txbuf[MODEM_BUFFER_SIZE];
	char rxbuf[MODEM_BUFFER_SIZE];
	uint8 txlen=0;
	uint8 rxlen=0;
	
	char * buf;
	uint8 count;

	global_construct();
	debug_write( g_debugio, m_debugwelcome, strlen(m_debugwelcome)+1 );
	memset( (char*)(&txbuf[0]), 0x00, MODEM_BUFFER_SIZE ); 
	memset( (char*)(&rxbuf[0]), 0x00, MODEM_BUFFER_SIZE ); 
	txlen = 0;
	rxlen = 0;

	wls_setrmtaddress( g_wls, g_config->data.rmtaddress );

	while (1)
	{
		if (txlen > 0)
		{
			buf = (char*)(txbuf[0]);
			count = wls_write( g_wls, buf, txlen, 0 );
			if (count > 0)
			{
				txlen -= count;
				memmove( buf, buf + count, txlen );
			}
		}
		
		if (rxlen > 0)
		{
			buf = (char*)(rxbuf[0]);
			count = sio_write( g_sio, buf, rxlen, 0 );
			if (count > 0)
			{
				rxlen -= count;
				memmove( buf, buf + count, rxlen );
			}
		}

		buf = (char*)(txbuf[0]) + txlen;
		count = sio_read( g_sio, buf, MODEM_BUFFER_SIZE - txlen, 0x00 );
		txlen += count;
		
		buf = (char*)(rxbuf[0]) + rxlen;
		count = wls_read( g_wls, buf, MODEM_BUFFER_SIZE - rxlen, 0x00 );
		rxlen += count;
		
		// send debug data if exists.
		debug_evolve( g_debugio );
	}
		
	global_destroy();	
	
	return;
}
