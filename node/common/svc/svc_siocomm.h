/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

#include "svc_configall.h"
#include "svc_foundation.h"
#include "../hal/hal_uart.h"
#include "../hal/hal_timer.h"
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_textspliter.h"

#ifndef SIO_RXBUFFER_CAPACITY  
  #define SIO_RXBUFFER_CAPACITY 0xFF
#endif

#ifndef SIO_TXBUFFER_CAPACITY  
  #define SIO_TXBUFFER_CAPACITY SIO_RXBUFFER_CAPACITY
#endif

#define SIO_START 0x01
#define SIO_END   0x99

/* TiSioComm
 * serial communication service based on TiUartAdapter, Spliter, Codec and Packet Queue 
 */
typedef struct{
  char txmem[ IOBUF_HOPESIZE(0x7F) ];
  char rxmem[ IOBUF_HOPESIZE(0x7F) ];
  char quemem[ IOBUF_HOPESIZE(0x7F) ];

  TiIoBuf * txbuf;
  TiIoBuf * rxbuf;
  TiIoBuf * rxque;

  TiUartAdapter * uart;
  TiTextSpliter * spliter;
  uint8 option;
}TiSioComm;

/* Construct an TiSioComm service in the specified memory block */
TiSioComm * sio_construct( char * buf, uint16 size );
/* Destroy an TiSioComm service */
void sio_destroy( TiSioComm * sio );

/* Open a TiSioComm service*/
TiSioComm * sio_open( TiSioComm * sio, TiUartAdapter * uart, TiTextSpliter * spliter, uint8 opt );

/* close the TiSioComm service*/
TiSioComm * sio_close( TiSioComm * sio );


/* Configure an TiSioComm service */
void sio_configure( TiSioComm * sio, TiTimerAdapter * timer, uint8 opt );

/* Read a packet from TiSioComm service. The packet is in the internal queue of TiSioComm */
uint8 sio_read( TiSioComm * sio, TiIoBuf * iobuf );

/* Write a packet into TiSioComm. The packet will be sent out through UART interface */
uint8 sio_write( TiSioComm * sio, TiIoBuf * iobuf );


/* Evolve the TiSioComm object */
void sio_evolve( TiSioComm * sio, TiEvent * e  );



