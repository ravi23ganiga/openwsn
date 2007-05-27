#ifndef _UART_H_2130_
#define _UART_H_2130_

/***************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TUartDriver object
 * Essentially, this is the software mapping of the UART hardware 
 * 
 ****************************************************************************/
 
#include "hal_foundation.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <mmsystem.h>
#include <assert.h>

#define TUart TUartDriver

#define UART_WRITE_ENABLE 1
#define UART_READ_ENABLE 1

typedef struct{
  uint8 id;
  HANDLE handle;
  char name[5];
  COMMTIMEOUTS  new_timeout;
  COMMTIMEOUTS  old_timeout;
  uint8 databits;
  uint8 stopbits;
  uint8 parity;
  uint32 baudrate;
  bool isOpened;
}TUartDriver;

TUartDriver * uart_construct( uint8 id, char * buf, uint8 size );
void uart_destroy( TUartDriver * uart );

 int8 uart_configure (TUartDriver * uart,uint32 baudrate, uint8 databits, uint8 stopbits, uint8 parity, uint8 optflag );

 void uart_reset( TUartDriver * uart );
 int8 uart_open( TUartDriver * uart, char * name );
 int8 uart_close( TUartDriver * uart );

#ifdef UART_READ_ENABLE
/* read data from UART driver. you can consider the UART's input as an continuous 
 * stream. This function will read the fist of this stream and place the data 
 * into the "buf". the maximum data count is constraint by the buffer's "size". 
 * If the data exceed the buffer's size, then you should call uart_read() for 
 * the second time to retrieve them into the buffer. 
 * 
 * @attention
 * 	for the current implementation, uart_read() will always read upto size
 * characters before it returned! this is due to uart_read adopts "Query and 
 * receive" techniques. you'd better change it to interrupt driven in the future.
 * you should not assume this function always return size characters.
 * 
 * @return
 * 	the data length actually returned in the buffer.
 */
uint8 uart_read( TUartDriver * uart, char * buf, uint8 size, uint8 opt );
#endif


#ifdef UART_WRITE_ENABLE
/* write the data in the buffer to UART. 
 * 
 * @attention
 * 	you should NOT assume this function can write "len" characters to UART.
 * due to the UART's ability, this function may write less than "len" characters
 * to the UART.
 */
uint8 uart_write( TUartDriver * uart, char * buf, uint8 len, uint8 opt ); 
#endif


/* get one character from the UART input stream. 
 * @return 
 * 	0		success, and "*pc" is the character received.
 * 	-1		failed
 */
int8 uart_getchar( TUartDriver * uart, char * pc );

/* put one character to the UART output stream
 * @return
 * 	0		success
 * 	-1		failed.
 */
int8 uart_putchar( TUartDriver * uart, char ch );

#endif

