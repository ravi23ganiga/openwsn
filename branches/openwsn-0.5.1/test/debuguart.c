
#include <string.h>
#include "..\foundation.h"
#include "..\global.h"
#include "debuguart.h"

void debuguart_test( void )
{
	char * buf = "debug uart output! \r\n";
	
	global_construct();
	
	while (1) 
	{
		debug_write( g_debugio, buf, strlen(buf)+1 );
		debug_write( g_debugio, buf, strlen(buf) );
				
		debug_evolve( g_debugio );
	}
	
	global_destroy();
}

