#include "hal_configall.h"
#include <stdio.h> 
#include <stdlib.h>
#include <avr/io.h> 
#include "hal_foundation.h"
#include "hal_uart.h"
#include "hal_redirect.h"


TiUartAdapter * m_uart = NULL;

static int _usart_putchar(char c);
static int _usart_getchar(void);


/* redirect the standard input/output stream to the uart. the uart must be 
 * constructed and opened successfully before call this function.
 */
void stdc_redirect( TiUartAdapter * uart )
{
	m_uart = uart;

	/* @todo
     * fdevopen is declared in <stdio.h> but you may need to link with some library 
     * to enable it. i don't know why there's always a warning when calling fdevopen
     */
	fdevopen( _usart_putchar, _usart_getchar, 0 );	
}


int _usart_putchar(char c)
{
	if (m_uart != NULL)
		return uart_putchar( m_uart, c );
	else
		return -1;
}

int _usart_getchar(void)
{
	if (m_uart != NULL)
		return uart_getchar_wait( m_uart );
	else
		return 0;
}
