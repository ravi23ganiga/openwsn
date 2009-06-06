
#include "osx_configall.h"
#include <string.h>
#include <assert.h>
#include "osx_foundation.h"
#include "../hal/hal_cpu.h"
#include "../hal/hal_interrupt.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_uart.h"
#include "osx_dba.h"

/*****************************************************************************
 * @todo
 * - this implementation require you have a correct implementation of hal_enter_critical()
 *  and hal_leave_critical(). however, they two in current release are not correct yet!
 *  so you should be very careful when using TiDebugAgent object in your program, 
 *  especially when you dealing with interrupt service routines.
 *  
 ****************************************************************************/ 

/* debug agent command format
 *  - debug agent command are visible characters which can be input through telnet 
 *    or other UART debugging tools
 *  - packet format: the debugging packet contains serveral strings, however, they
 *    are separated by blank character ' ' rather than the standard '\0', so you 
 *    had to do the command parsing with caution because there's no room for the 
 *    last '\0'.
 *     
 *    [COMMAND_ID, 1B-nB]{[' ' 1B]{PARAM mB}}
 *
 * new version debug agent command will be:
 *		[1B length][1B command_id]{[' ' 1B]{PARAM mB}}
 */

static void _dba_hardwrite( TiDebugAgent * db );

TiDebugAgent * dba_construct( char * buf, uintx size )
{
	TiDebugAgent * dba;
	
	dba = (sizeof(TiDebugAgent) <= size) ? (TiDebugAgent *)buf : NULL;
	if (dba != NULL)
	{
		memset( buf, 0x00, size );
		dba->txlen = 0;
		dba->rxlen = 0;
	}
	
	return dba;
}

void dba_destroy( TiDebugAgent * dba )
{
	return;
}

TiDebugAgent * dba_attach( TiDebugAgent * dba, TiUartAdapter * uart )
{
	if (dba != NULL)
	{
		dba->txlen = 0;
		dba->rxlen = 0;
		dba->uart = uart;
	}
	return dba;
}

void dba_detach( TiDebugAgent * dba )
{
	dba = dba;
	return;
}

/* this function should be called frequvently to send data to UART
 * or else all your debug operations only place data in the internal buffer
 * without sending them out.
 *
 * @todo
 * actually, the input stream from uart should be divided into correct segments
 * and be interpreted later. if the segments division is incorrect, then the
 * interpretation will get incorrect results.
 */
void dba_evolve( TiDebugAgent * dba, TiEvent * e )
{
	//uintx count, available;
	//char * buf;

	/* if there're data pending in the internal buffer, then try to send them out */
	if (dba->txlen > 0)
		_dba_hardwrite( dba );

	/* check whether there's data received by UART adapter. they're assumed to be
	 * the commands, then we should interpret and execute these commands.
	 */
	#ifdef CONFIG_DBA_INPUT
	/* todo
	if ((e != NULL) && (e->id == EVENT_UART_DATA_ARRIVAL))
	{
		while (1)
		{
			available = CONFIG_DBA_RXBUF_SIZE - dba->rxlen;
			if (available <= 0)
				break;

			count = uart_read( dba->uart, &(dba->rxbuf[dba->rxlen]), available, 0x00 );
			if (count == 0)
				break;
			else
				dba->rxlen += count;
		}

		if ((dba->rxlen > 0) && (dba->txlen == 0))
		{
			dba_interpret( dba, &(dba->rxbuf[0]), dba->rxlen, &(dba->txbuf[0]), &(dba->txlen) );
			dba->rxlen = 0;

			if (dba->txlen > 0)
				_dba_hardwrite( dba );
		}
	}
	*/
	#endif
}

/* if there're data pending in the internal buffer, then try to send them out */
void _dba_hardwrite( TiDebugAgent * dba )
{
	uintx count;
	char * buf;
	cpu_atomic_t atom;

	atom = _cpu_atomic_begin();
	count = uart_write( dba->uart, &(dba->txbuf[0]), dba->txlen, 0 );
	if (dba->txlen >= count)
		dba->txlen -= count;
	else
		dba->txlen = 0;

	if (count > 0)
	{
		buf = (char *)(&(dba->txbuf[0]));
		memmove( buf, buf+count, dba->txlen );
	}
	_cpu_atomic_end(atom);
}

uintx dba_write( TiDebugAgent * dba, char * buf, uintx size )
{
	uintx copied;
	uintx i;
	cpu_atomic_t atom;

	atom = _cpu_atomic_begin();
	i = CONFIG_DBA_TXBUF_SIZE - dba->txlen;
	// assert( i <= CONFIG_DBA_TXBUF_SIZE );

	copied = min(i, size);
	if (copied > 0)
	{
		for (i=0; i<copied; i++)
		{
		   dba->txbuf[i+dba->txlen] = buf[i];
		}
		dba->txlen += copied;
	}
	_cpu_atomic_end( atom );

	return copied;
}


/* {input, len} is the frame received from the host computer, which contains a command
 * and serveral parameters
 *
 * @TODO: command parsing and execution.
 */
#ifdef CONFIG_DBA_INPUT
void dba_interpret( TiDebugAgent * dba, char * input, uintx len, char * output, uintx * size )
{
	char * cmd, * param;

	if (len > 0)
	{
		cmd = input;

		switch (cmd[0])
		{
		case 'm':
		case 'p':
		case 'b':
			param = &(cmd[1]);
			param = param;
			break;
		case 'r':
			switch (cmd[1])
			{
			case 's':
				//cpu_reboot();
			case 'e':
				//cpu_reboot();
				break;
			}
			break;
		case 'u':
			/* upgrade */
			break;
		}
		output[0] = 'R';
		output[1] = '\0';
	}

	*size = 1;
}
#endif


