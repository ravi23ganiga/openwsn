/***************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TUartDriver object
 * Essentially, this is the software mapping of the UART hardware 
 * 
 ****************************************************************************/

#include "hal_foundation.h"
#include <stdlib.h>
#include <string.h>
#include "hal_uart.h"

/* initialze the UART hardware and object
 * @param
 * 	id		0	UART0
 * 			1 or other values	UART1
 * @return 
 * 	0		success
 *  -1		failed
 * 
 * @modified by zhangwei on 20061010
 * @TODO
 * zhangwei kept the old declaration of the function in order to keep other modules running. 
 * this is different to Huanghuan's. Huanghuan's source code should call uart_configure() after
 * uart_construct()  and remove the configure call from construct.
 */
TUartDriver * uart_construct( uint8 id, char * buf, uint8 size )
{
	TUartDriver * uart;
     
        
	if (sizeof(TUartDriver) > size)
		uart = NULL;
	else 
		uart = (TUartDriver *)buf;
	
	if (uart != NULL)
	{
		memset( (char*)uart, 0x00, sizeof(TUartDriver) );
		uart->id = id;
		uart->baudrate = 9600;
		uart->databits = 8;
		uart->stopbits = 1;
		uart->parity = 0;
	}

	return uart;
}

void uart_destroy( TUartDriver * uart )
{
	uart_close( uart );
	NULL;
}

// name = "COM0", "COM1", etc
//
int8 uart_open( TUartDriver * uart, char * name )
{
	char conf[64];
    int8 ret = 0;

    if ((uart->handle != INVALID_HANDLE_VALUE) && (uart->handle != NULL))
    {
		#ifdef GDEBUG
        //MsgBox( "The uart has already in open status!", __FUNC__, MB_OK + MB_ICONSTOP );
		#endif
        ret = -1;
    }

    if (ret == 0)
    {
        DCB dcbCommuart;

		//strncpy( &(uart->name[0]), name, 15 );
		memmove(&(uart->name[0]),name,15);

		// pls modify it for me, zhou songli
        //uart->handle = CreateFile( name, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0 );
        if ( uart->handle == INVALID_HANDLE_VALUE )
        {
			#ifdef GDEBUG
            //MsgBox( "open uart failed! other program using it?", __FILE__, MB_OK + MB_ICONSTOP );
			#endif
			ret = -2;
        }

        // Set the COMM timeout
        if (ret == 0)
        {
            GetCommTimeouts( uart->handle, &(uart->old_timeout) );
            uart->new_timeout.ReadTotalTimeoutConstant = 100;
            uart->new_timeout.ReadTotalTimeoutMultiplier = 0;
            uart->new_timeout.WriteTotalTimeoutMultiplier = 0;
            uart->new_timeout.WriteTotalTimeoutConstant = 0;
            SetCommTimeouts( uart->handle, &(uart->new_timeout) );

            // Set baudrate, parity, word size, and stop bits.
            // there are other ways of doing setting these, but this is the easiest.
            // If you want to later add code for other baudrates, remember that the
            // argument for BuildCommDCB must be a pointer to a String. Also note
            // that BuildCommDCB() defaults to no HANDSHAKING.
            //
			memset( conf, 0x00, 64 );
			/* @modified by zhangwei on 20061108 
			 * zhou: pls modify the following for me
			if (uart->parity)
				snprintf( &(conf[0]), 64, "%d,%c,%d,%d", uart->baudrate, 'Y', uart->databits, uart->stopbits );
			else
				snprintf( &(conf[0]), 64, "%d,%c,%d,%d", uart->baudrate, 'N', uart->databits, uart->stopbits );
			*/
			// for example, conf = "9600,N,8,1";
			//
            memset( &dcbCommuart, 0x00, sizeof(DCB) );
            dcbCommuart.DCBlength = sizeof(DCB);
            GetCommState( uart->handle, &dcbCommuart );
			// pls rewrite the following line to eliminate the warning
            //BuildCommDCB( conf, &dcbCommuart );
            SetCommState( uart->handle, &dcbCommuart );
        }
    }

    if (ret < 0)
    {
        uart->handle = INVALID_HANDLE_VALUE;
    }

    return ret;
}

int8 uart_close( TUartDriver * uart )
{
    if (uart->handle != INVALID_HANDLE_VALUE)
    {
        // Purge the internal COMM buffer, restore the previous Timeout settings.
        // and close the COMM uart.
        //
        PurgeComm( uart->handle, PURGE_RXABORT );
        SetCommTimeouts( uart->handle, &uart->old_timeout );
        CloseHandle( uart->handle );
        uart->handle = INVALID_HANDLE_VALUE;
    }

    return 0;
}


// @TODO 20061013
// you should enable the interrupt in configure function
//
// uart_configure (uart, 9600, 8, 1, 0 );
//
int8 uart_configure (TUartDriver * uart,uint32 baudrate, uint8 databits, uint8 stopbits, uint8 parity, uint8 optflag )
{
	uart->handle = INVALID_HANDLE_VALUE;
	uart->databits = databits;
	uart->stopbits = stopbits;
	uart->parity = parity;
	uart->baudrate = baudrate;

	uart_close( uart );
	return uart_open( uart, "COM0" );
}

/* clear the TUartDriver internal buffer
 */
void uart_reset( TUartDriver * uart )
{
	if (uart->handle != INVALID_HANDLE_VALUE)
		PurgeComm( uart->handle, PURGE_RXABORT );
}

void uart_interrupt_init( TUartDriver * uart )
{
	NULL;
}

/* read data out from the TUartDriver's internal buffer. 
 * this is a non-block operation. it will return 0 if no data received.
 * @param
 * 	buf		the memory buffer to received the data
 * 	size	the capacity of the buffer
 * @return
 * 	the character count wroten to the buf
 * 	return 0 means there's no data in the TUartDriver buffer. i.e. no data received.
 */
#ifdef UART_READ_ENABLE
uint8 uart_read( TUartDriver * uart, char * buf, uint8 size, uint8 opt )
{
	uint32 dwBytesRead=0;
	ReadFile( uart->handle, buf, size, &dwBytesRead, NULL);
	return (uint8)dwBytesRead;
}
#endif


/* write the data in the buffer to UART to sent out
 * this is a non-block operation. 
 * @param
 * 	buf		where's the data
 * 	len		length of the data in the buffer
 * @return
 * 	the count of characters actually sent
 */
#ifdef UART_WRITE_ENABLE
uint8 uart_write( TUartDriver * uart, char * buf, uint8 len, uint8 opt )
{
	uint32 dwBytesWrite=0;
	WriteFile( uart->handle, buf, len, &dwBytesWrite, NULL);
    FlushFileBuffers( uart->handle );
	return (uint8)dwBytesWrite;
}
#endif


/* this function is hardware related
 * you should change the register in this function
 * 
 * @return
 * 	0		success, *ch is char just read from UART
 *  -1		failed
 */
int8 uart_getchar( TUartDriver * uart, char * pc )
{
	uint32 dwBytesRead=0;
	ReadFile( uart->handle, pc, 1, &dwBytesRead, NULL);
	return (dwBytesRead > 0) ? 0 : -1;
}

int8 uart_putchar( TUartDriver * uart, char ch )
{
	uint32 dwBytesWrite=0;
	WriteFile( uart->handle, &ch, 1, &dwBytesWrite, NULL);
    FlushFileBuffers( uart->handle );
	return (dwBytesWrite > 0) ? 0 : -1;
}
