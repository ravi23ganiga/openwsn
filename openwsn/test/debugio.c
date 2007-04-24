
#include <string.h>
#include "..\foundation.h"
#include "..\global.h"
#include "..\service\svc_debugio.h"
#include "debugio.h"

void debuguart_test( void )
{
	char * buf = "debug uart output! \r\n";
	
	global_construct();
	debug_open( g_debugio, g_uart0 );
	
	while (1) 
	{
		debug_write( g_debugio, buf, strlen(buf)+1 );
		debug_write( g_debugio, buf, strlen(buf) );
				
		debug_evolve( g_debugio );
	}
	
	debug_close( g_debugio );	
	global_destroy();
}

