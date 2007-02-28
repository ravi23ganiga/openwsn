
#include "..\foundation.h"
#include "..\global.h"
#include "uartecho.h"

#define UART_BUF_SIZE 256

void uartecho_run( void )
{

	char memory[UART_BUF_SIZE * 2];
	char * buf1 = (char *)( &memory[0] );
	char * buf2 = buf1 + UART_BUF_SIZE;
	uint8 rxlen1, rxlen2, count;

	target_init();
	global_construct();
	uart_configure( g_uart, 115200, 0, 0, 0, 0 );


	
	memset( buf1, 0x00, UART_BUF_SIZE ); 
	memset( buf2, 0x00, UART_BUF_SIZE ); 
	rxlen1 = 3;
	rxlen2 = 0;

	while (1)
	{
		count = uart_write( g_uart, buf1, rxlen1, 0 );
		if (count > 0)
		{
			rxlen1 -= count;
			memmove( buf1, buf1 + count, rxlen1 );
		}
		
		count = uart_write( g_debuguart, buf2, rxlen2, 0 );
		if (count > 0)
		{
			rxlen2 -= count;
			memmove( buf2, buf2 + count, rxlen2 );
		}
		
		count = uart_read( g_uart, (char*)buf1 + rxlen1, UART_BUF_SIZE - rxlen1, 0 );
		rxlen1 += count;
		
		count = uart_read( g_debuguart, (char*)buf2 + rxlen2, UART_BUF_SIZE - rxlen2, 0 );
		rxlen2 += count;
	}
		
	global_destroy();	
	
	return;
}


