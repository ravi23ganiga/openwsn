/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#ifndef _DEBUGIO_H_
#define _DEBUGIO_H_

#include "svc_foundation.h"
#include "..\hal\hal_uart.h"

/*****************************************************************************
 * @todo
 * - this implementation require you have a correct implementation of hal_enter_critical()
 *  and hal_leave_critical(). however, they two in current release are not correct yet!
 *  so you should be very careful when using TDebugIo object in your program, 
 *  especially when you dealing with interrupt service routines.
 *  
 ****************************************************************************/ 

#define CONFIG_DEBUGIO_BUFFER_CAPACITY 127

typedef struct{
  TUartDriver * uart;
  uint16 datalen;
  char buf[CONFIG_DEBUGIO_BUFFER_CAPACITY];
}TDebugIo;

TDebugIo * debug_construct( char * buf, uint16 size );
void debug_destroy( TDebugIo * db );
TDebugIo * debug_open( TDebugIo * db, TUartDriver * uart );
void debug_close( TDebugIo * db );

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