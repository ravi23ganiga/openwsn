#include <../configall.h>
#include <../foundation.h>
#include <../global.h>
#include <../service/svc_debugio.h>
#include "debugio.h"

void debugio_test( void )
{
	char * buf = "debug uart output! \r\n";
	
	global_construct();
	uart_configure( g_uart, 115200, 0, 0, 0, 0 );
	debug_open( g_debugio, g_uart );
	while (1) 
	{       
		debug_write( g_debugio, buf, strlen(buf));
		debug_write( g_debugio, buf, strlen(buf) );
		debug_evolve( g_debugio );
		uart_write(g_uart, "uartecho run3.", 15, 0 );
	}
	
	debug_close( g_debugio );	
	global_destroy();
}

