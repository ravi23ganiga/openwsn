#ifndef _UART_H_2130_
#define _UART_H_2130_

/***************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TUartDriver object
 * Essentially, this is the software mapping of the UART hardware 
 * 
 * @modified by zhangwei on 2006-07-20
 * modify the prototype of uart_configure()
 * update some comments of the functions.
 *
 * @modified by zhangwei on 2006-08-18
 * add support to RS232 and RS485 by using macros
 * the configure macro CONFIG_UART_RS232 and CONFIG_UART_RS485 should be defined
 * in "configall.h" in the application.
 * 
 ****************************************************************************/
 
#include "hal_foundation.h"

#define UART_TXBUFFER_SIZE 0x7F
#define UART_RXBUFFER_SIZE UART_TXBUFFER_SIZE

typedef struct{
  uint8 id;
  char txbuf[UART_TXBUFFER_SIZE]; // @TODO: txbuf can be eliminated in the future 20061010
  char rxbuf[UART_RXBUFFER_SIZE];
  uint8 txlen;
  uint8 rxlen;
  uint8 databits;
  uint8 stopbits;
  uint8 parity;
  uint32 baudrate;
}TUartDriver;

TUartDriver * uart_construct( uint8 id, char * buf, uint16 size );
void uart_destroy( TUartDriver * uart );
int16 uart_configure (TUartDriver * uart,uint32 baudrate, uint8 databits, uint8 stopbits, uint8 parity, uint8 optflag );
void uart_reset( TUartDriver * uart );

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
uint16 uart_read( TUartDriver * uart, char * buf, uint16 size, uint16 opt );
#endif


#ifdef UART_WRITE_ENABLE
/* write the data in the buffer to UART. 
 * 
 * @attention
 * 	you should NOT assume this function can write "len" characters to UART.
 * due to the UART's ability, this function may write less than "len" characters
 * to the UART.
 */
uint16 uart_write( TUartDriver * uart, char * buf, uint16 len, uint16 opt ); 
#endif


/* get one character from the UART input stream. 
 * @return 
 * 	0		success, and "*pc" is the character received.
 * 	-1		failed
 */
int16 uart_getchar( TUartDriver * uart, char * pc );

/* put one character to the UART output stream
 * @return
 * 	0		success
 * 	-1		failed.
 */
int16 uart_putchar( TUartDriver * uart, char ch );

#endif

