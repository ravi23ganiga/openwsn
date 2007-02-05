/***************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TUartDriver object
 * Essentially, this is the software mapping of the UART hardware 
 * 
 ****************************************************************************/

#include "hal_foundation.h"
#include <stdlib.h>
#include <string.h>
//#include <windows.h>
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
		uart->baudrate = 57600;
		uart->databits = 8;
		uart->stopbits = 1;
		uart->parity = 0;
		uart->isOpened = false;
		uart->handle = 0;
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
	//char conf[64];
    int8 ret = 0;
   if (uart == NULL) return (-100);	
   if ((uart->handle != INVALID_HANDLE_VALUE) && (uart->handle != NULL))
    {
		#ifdef GDEBUG
        //MsgBox( "The uart has already in open status!", __FUNC__, MB_OK + MB_ICONSTOP );
		#endif		
		uart->isOpened = true;
        ret = -1;
    }

    if (ret == 0)
    {
        DCB dcbCommuart;
        //const char* psDevName = name;
		//strncpy( &(uart->name[0]), name, 15 );
		memmove(&(uart->name[0]),name,4);
/*       
		// pls modify it for me, zhou songli
		uart->handle = CreateFile((LPCSTR) &uart->name[0], 
		          	GENERIC_READ | GENERIC_WRITE,// 允许读写
		        	0,// 此项必须为0
		         	0, // no security attrs
		        	OPEN_EXISTING,//设置产生方式
		        	//FILE_FLAG_OVERLAPPED,//使用异步通信
					0,
			        NULL);
*/		                 

        if ( uart->handle == INVALID_HANDLE_VALUE )
        {
	    	#ifdef GDEBUG
            //MsgBox( "open uart failed! other program using it?", __FILE__, MB_OK + MB_ICONSTOP );
			#endif
			uart->isOpened = true;
			ret = -2;
        }

	    memset( &dcbCommuart, 0x00, sizeof(DCB) );

        dcbCommuart.DCBlength = sizeof(DCB);
		
		// 默认串口参数
        GetCommState( uart->handle, &dcbCommuart );

       //设置波特率参数
		dcbCommuart.BaudRate = uart->baudrate; 

       //设置数据位
		dcbCommuart.ByteSize = uart->databits;     // Number of bits/char, 4-8 
	
		/*奇偶校验位	
		  The value of parity is between 0 and 2;
		  Parity is not used when the value is 0;
		  parity is used in ODD Mode when the value is 1.
		  parity is used in EVEN Mode when the value is 2.
		*/
		dcbCommuart.Parity = uart->parity;

		//停止位设置
		if (uart->stopbits == 1)
		{
			dcbCommuart.StopBits = ONESTOPBIT;
		}
		else if (uart->stopbits == 2)
		{
			dcbCommuart.StopBits = TWOSTOPBITS;
		}	

		// pls rewrite the following line to eliminate the warning
        //BuildCommDCB( conf, &dcbCommuart );
        SetCommState( uart->handle, &dcbCommuart );

		//设置事件驱动的类型
//		SetCommMask(uart->handle, EV_RXCHAR|EV_TXEMPTY );

   		//设置输入、输出缓冲区的大小
//		SetupComm( uart->handle, 128,128) ;
		//清干净输入、输出缓冲区
		PurgeComm( uart->handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ); 

 
       //配置超时值//COMMTIMEOUTS  CommTimeouts;	
		GetCommTimeouts(uart->handle, &uart->old_timeout);
		uart->new_timeout.ReadIntervalTimeout = MAXDWORD;            
        uart->new_timeout.ReadTotalTimeoutConstant = 100;
        uart->new_timeout.ReadTotalTimeoutMultiplier = 0;
        uart->new_timeout.WriteTotalTimeoutMultiplier = 0;
        uart->new_timeout.WriteTotalTimeoutConstant = 0;
        SetCommTimeouts( uart->handle, &(uart->new_timeout) );

		uart->isOpened = true;

            // Set baudrate, parity, word size, and stop bits.
            // there are other ways of doing setting these, but this is the easiest.
            // If you want to later add code for other baudrates, remember that the
            // argument for BuildCommDCB must be a pointer to a String. Also note
            // that BuildCommDCB() defaults to no HANDSHAKING.
            //
			//memset( conf, 0x00, 64 );
			/* @modified by zhangwei on 20061108 
			 * zhou: pls modify the following for me
			if (uart->parity)
				snprintf( &(conf[0]), 64, "%d,%c,%d,%d", uart->baudrate, 'Y', uart->databits, uart->stopbits );
			else
				snprintf( &(conf[0]), 64, "%d,%c,%d,%d", uart->baudrate, 'N', uart->databits, uart->stopbits );
			*/
			// for example, conf = "9600,N,8,1";
			//

     
    }

    if (ret < 0)
    {
        uart->handle = INVALID_HANDLE_VALUE;
    }

    return ret;
}

int8 uart_close( TUartDriver * uart )
{
	if (uart == NULL) return (-100);
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
 int8 uart_configure (TUartDriver *uart,uint32 baudrate, uint8 databits, uint8 stopbits, uint8 parity, uint8 optflag )
{
	if (uart == NULL) return (-100);
	uart->handle = INVALID_HANDLE_VALUE;
	uart->databits = databits;
	uart->stopbits = stopbits;
	uart->parity = parity;
	uart->baudrate = baudrate;
	if (uart->isOpened)
	{
	   uart_close(uart);
	}
	
	switch(uart->id)
	{
	   case 0:
		  return uart_open( uart,"COM1");			
	   case 1:
		  return uart_open( uart,"COM2");		
	   case 2:
		  return uart_open( uart,"COM3");		
	   
	}
	return uart_open(uart,"COM4");

}

/* clear the TUartDriver internal buffer
 */
  void uart_reset( TUartDriver * uart )
{
	if (uart == NULL) return;
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
	uint32 dwcharsRead=0;
	ReadFile( uart->handle, buf, size, &dwcharsRead, NULL);
	return (uint8)dwcharsRead;
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
	uint32 dwcharsWrite=0;
	WriteFile( uart->handle, buf, len, &dwcharsWrite, NULL);
    FlushFileBuffers( uart->handle );
	return (uint8)dwcharsWrite;
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
	uint32 dwcharsRead=0;
	ReadFile( uart->handle, pc, 1, &dwcharsRead, NULL);
	return (dwcharsRead > 0) ? 0 : -1;
}

int8 uart_putchar( TUartDriver * uart, char ch )
{
	uint32 dwcharsWrite=0;
	WriteFile( uart->handle, &ch, 1, &dwcharsWrite, NULL);
    FlushFileBuffers( uart->handle );
	return (dwcharsWrite > 0) ? 0 : -1;
}
