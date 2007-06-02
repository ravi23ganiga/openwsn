#include <../configall.h>
#include "..\foundation.h"
#include "..\global.h"
#include "uartecho.h"
#include "uartanddebug.h"

#define UART_BUF_SIZE 256

char * m_pbuf = "debug uart output! \r\n";	

void uartanddebug_run( void )
{
	char memory[UART_BUF_SIZE * 2];
	char * buf1 = (char *)( &memory[0] );
	char * buf2 = buf1 + UART_BUF_SIZE;
	uint8 rxlen1, rxlen2, count;

	target_init();
	global_construct();
	
	debug_write( g_debugio, m_pbuf, strlen(m_pbuf)+1 );
	debug_write( g_debugio, m_pbuf, strlen(m_pbuf) );

	memset( buf1, 0x00, UART_BUF_SIZE ); 
	memset( buf2, 0x00, UART_BUF_SIZE ); 
	rxlen1 = 0;
	rxlen2 = 0;
	
	while (1)
	{
		count = uart_write( g_uart, buf1, rxlen1, 0 );
		if (count > 0)
		{
			rxlen1 -= count;
			memmove( buf1, buf1 + count, rxlen1 );
			debug_write( g_debugio, "uart_write ...", strlen("uart_write...") );
		}
		
		count = uart_read( g_uart, (char*)buf1 + rxlen1, UART_BUF_SIZE - rxlen1, 0 );
		rxlen1 += count;
		if (count > 0)
		{
			debug_write( g_debugio, "uart_read ...", strlen("uart_read...") );
		}
				
		// try to send the debug data in the buffer by background 
		debug_evolve( g_debugio );		
	}
		
	global_destroy();	
	
	return;
}


