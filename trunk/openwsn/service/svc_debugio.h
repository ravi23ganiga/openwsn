
#ifndef _DEBUGIO_H_
#define _DEBUGIO_H_

#include "svc_foundation.h"
#include "..\hal\hal_uart.h"

#define DEBUGIO_BUF_CAPACITY 128

typedef struct{
  TUartDriver * uart;
  uint8 datalen;
  char buf[DEBUGIO_BUF_CAPACITY];
}TDebugIo;

TDebugIo * debug_construct( char * buf, uint16 size, TUartDriver * uart );
void debug_destroy( TDebugIo * db );

/* this function should be called frequvently to send data to UART
 * or else all your debug operations only place data in the internal buffer
 * without sending them out.
 */
#define debug_backgnd_sending(db) debug_evolve(db)
void debug_evolve( TDebugIo * db );

#define debug_putchar(db,c) uart_putchar(db->uart,c)
#define debug_getchar(db,pc) uart_getchar(db->uart,pc)
#define debug_read(db,buf,size) uart_read(db->uart,buf,size,0)
#define debug_writechar(db,ch) debug_write(db,&ch,1)
#define debug_writeint8(db,n)  debug_write(db,&n,1)
#define debug_writeuint8(db,n) debug_write(db,&n,1)
#define debug_writeint16(db,n) debug_write(db,&n,2)
#define debug_writeuint16(db,n) debug_write(db,&n,2)
#define debug_writeint32(db,n) debug_write(db,&n,4)
#define debug_writeuint32(db,n) debug_write(db,&n,4)
#define debug_writestring(db,pc) debug_write(db,pc,strlen(pc))

uint16 debug_write( TDebugIo * db, char * buf, uint16 size );

#endif