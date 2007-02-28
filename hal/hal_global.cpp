//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// Global Variables with initialization and finalization
//
// 	This module is very important in the whole application. It does not only 
// declares and initializes all the global objects, but also create the 
// relationships between different objects.
// 	In TinyOS and nesC systems, these relationships are constructed by a separate
// Configurator object. As you have seem here, this is not necessary. You should
// understand and adopt the OOP programming model and use it in such conditions.
//  
//----------------------------------------------------------------------------
// @history
// @modified by zhangwei on 2006-10-18 
// 	first created
//
//----------------------------------------------------------------------------

#include "hal_foundation.h" 
#include "hal_global.h"

static TUartDriver 			m_uart0;
TUartDriver *				g_uart0 = NULL; 


// an global variable to record whether the system is successfully initialized.
uint8 						g_hal_init = FALSE;

// @attention
//	though the debugio_xxx() used the g_debugio object, it has already declared
// in the debugio module to facilitate programming. so you need NOT declare 
// g_debugio here.
//
// @return
//	0:	success
//	-1 or other negtive:	failed
//
int8 hal_global_construct( void )
{
	g_hal_init = TRUE;
	
	g_uart0 	= uart_construct( 0, (char*)(&m_uart0), sizeof(TUartDriver) ); 

	if ( (g_uart0 == NULL))
	{
		g_hal_init = FALSE;
		assert( FALSE );
	}
	
	// after constructing all the global objects successfully and link them 
	// successfully, you should do more work to configure them to work.
	// here is a good place for you to check and start the hardware.
	//
	if (g_hal_init)
	{
		uart_configure( g_uart0, 9600, 8, 1, 0, 0x00 );
	}
        
	return (g_hal_init == TRUE) ? 0 : -1;
}

int8 hal_global_destroy( void )
{
	uart_destroy( g_uart0 );
	return 0;
}
