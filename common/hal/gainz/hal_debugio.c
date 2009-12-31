#include "hal_configall.h"
#include <string.h>
#include <avr/io.h> 
#include "hal_foundation.h"
#include "hal_debugio.h"

#ifdef CONFIG_DEBUG

typedef struct{
  uint8 txlen;
  char txbuf[CONFIG_DBO_TXBUFFER_SIZE];
}TiDebugUart;

static bool g_dbo_init = false;
TiDebugUart g_dbo;

void _dbo_open( uint8 uart_id, uint16 bandrate )
{
	if (g_dbo_init)
		return;

	memset( &g_dbo, 0x00, sizeof(TiDebugUart) );

	#ifdef CONFIG_DBO_UART0
	/* initialize PIN directions. PE0 should be input and PE1 should be output.
	 * actaully, you needn't do so for Atmega128.  
	 */
	DDRE &= (~(1 << DDE0));		// set PE0/USART0 RXD as input
	DDRE |= (1 << DDE1 );		// set PE1/USART0 TXD as output

	/* set baudrate. 
	 * when MCU system clock is 8MHz (for ICT's GAINZ platform)
	 *	UBRR0L=8  => 57600 bps
	 *  UBRR0L=12 => 38400 bps, error 0.2 %  
	 */
	UBRR0L = 12;

	/* enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	#endif
	
	#ifdef CONFIG_DBO_UART1
	// HAL_MAKE_PD2_INPUT();
	// HAL_MAKE_PD3_OUTPUT();

	/* {UBRRH, UBRRL} = system clock / (16 * baudrate) - 1 */
	tmpn = (CONFIG_SYSTEM_CLOCK / (16L * baudrate) - 1);
	UBRR1H = (unsigned char)(tmpn >> 8);
	UBRR1L = (unsigned char)tmpn;

	/* enable receiver and transmitter */
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	#endif

	g_dbo_init = true;
}

void _dbo_close()
{
	g_dbo.txlen = 0;
}

/* getchar from UART. if there's no input from UART, then this function will wait
 * until there's a input. 
 */
char _dbo_getchar()
{
	#ifdef CONFIG_DBO_UART0
	while (!(UCSR0A & (1<<RXC0))) {};
	return UDR0;
	#endif

	#ifdef CONFIG_DBO_UART1
	while (!(UCSR1A & (1<<RXC1))) {};
	return UDR1;
	#endif
}

/* _dbo_putchar()
 * this function sends one character only through the UART hardware. 
 * 
 * @return
 *	0 means success, and -1 means failed (ususally due to the buffer is full)
 *  when this functions returns -1, you need retry.
 */
void _dbo_putchar( char ch )
{
	/* wait for the transmit buffer empty */
	#ifdef CONFIG_DBO_UART0
	while (!(UCSR0A & (1<<UDRE0))) {};
	UDR0 = ch;
	#endif

	#ifdef CONFIG_DBO_UART1
	while (!(UCSR1A & (1<<UDRE1))) {};
	UDR1 = ch;
	#endif

	return;
}

void _dbo_putbyte( uint8 val )
{
	_dbo_putchar(_dbo_digit2hexchar(((val)&0xF0) >> 4));	
	_dbo_putchar(_dbo_digit2hexchar((val)&0x0F));	
}

void _dbo_write( char * buf, uintx len )
{
    uintx i;
    for (i=0; i<len; i++)
	{
		dbo_putchar(buf[i]);
	}
}

void _dbo_write_n8toa( char * buf, uintx len )
{
    uintx i;
    for (i=0; i<len; i++)
	{
		dbo_n8toa( buf[i] );
		dbo_putchar( ' ');
	}
}

void dbo_string( char * string )
{
	uintx i;
	for (i=0; i<strlen(string); i++)
		dbo_n8toa( string[i] );
}

/* simply wrotten to internal memory. this is pretty fast so that this function can
 * help debugging ISR */

uintx _dbo_asyncwrite( char * buf, uintx len )
{
	uintx count = min( CONFIG_DBO_TXBUFFER_SIZE - g_dbo.txlen, len );
	memmove( &(g_dbo.txbuf[g_dbo.txlen]), buf, count );
	g_dbo.txlen += count;
	return count;
}

void _dbo_evolve()
{
	_dbo_write( &(g_dbo.txbuf[0]), g_dbo.txlen );
}

char _dbo_digit2hexchar( uint8 num )
{
	static char g_digit2hextable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	return (g_digit2hextable[num & 0x0F]);
}


#endif /* CONFIG_DEBUG */
