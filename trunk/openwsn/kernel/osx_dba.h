#ifndef _OSX_DEBUGAGENT_H_4778_
#define _OSX_DEBUGAGENT_H_4778_

/*****************************************************************************
 * dba: debug agent
 *
 * @state
 *	- in developing. not tested
 *
 * @author zhangwei on 200507
 * @modified by zhangwei in 200903
 *	- revision
 ****************************************************************************/

#include "osx_configall.h"
#include "../hal/hal_uart.h"
#include "osx_foundation.h"

/*****************************************************************************
 * dba: debug agent
 * this is an runnable object which can help debugging. it will send the system
 * output to the host computer through one of the UART/RS232 interface.
 *
 * most of the I/O functions can be called safely even in interrupt service routines
 * except the dba_evolve(). this feature can help debugging the interrupt services.
 *
 * debug agent (dba) provides an input/output interface for the device. it can
 * be configured with output only by disable  CONFIG_DBA_INPUT
 * or bi-directional.
 *
 * if CONFIG_DBA_INPUT is defined, then the dba_interpret() is effective
 * and it can execute some simple commands such as (similar to the ancient dos
 * debug program):
 *
 *  m <addr>  print memory content at the specified address
 *  r <addr>  print register content
 *  rst reset
 *  rbt reboot
 *
 * dba will use one uart for input/output. it needs the TiUartAdapter's support.
 *
 * @todo
 * - this implementation require you have a correct implementation of hal_enter_critical()
 *  and hal_leave_critical(). however, they two in current release are not correct yet!
 *  so you should be very careful when using TiDebugAgent object in your program,
 *  especially when you dealing with interrupt service routines.
 *
 ****************************************************************************/


/* controls whether the debug agent can accept incoming commands from uart */

#define CONFIG_DBA_INPUT   1
#define CONFIG_DBA_OUTPUT  1

#define CONFIG_DBA_TXBUF_SIZE 127
#define CONFIG_DBA_RXBUF_SIZE 20

/* when data arrival from uart, then the following event will be sent to debug agent
 * through the dba_evolve(e) function. */

#ifdef CONFIG_DBA_OUTPUT
#define dba_putchar(dba,c)      uart_putchar(dba->uart,c)
#define dba_getchar(dba,pc)     uart_getchar(dba->uart,pc)
#define dba_read(dba,buf,size)  uart_read(dba->uart,buf,size,0)
#define dba_writechar(dba,ch)   dba_write(dba,&ch,1)
#define dba_writeint8(dba,n)    dba_write(dba,&n,1)
#define dba_writeuint8(dba,n)   dba_write(dba,&n,1)
#define dba_writeint16(dba,n)   dba_write(dba,&n,2)
#define dba_writeuint16(dba,n)  dba_write(dba,&n,2)
#define dba_writeint32(dba,n)   dba_write(dba,&n,4)
#define dba_writeuint32(dba,n)  dba_write(dba,&n,4)
#define dba_writestring(dba,pc) dba_write(dba,pc,strlen(pc))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
  TiUartAdapter * uart;
  #ifdef CONFIG_DBA_OUTPUT
  uintx txlen;
  char txbuf[CONFIG_DBA_TXBUF_SIZE];
  #endif
  #ifdef CONFIG_DBA_INPUT
  uintx rxlen;
  char rxbuf[CONFIG_DBA_RXBUF_SIZE];
  #endif
}TiDebugAgent;

TiDebugAgent *  dba_construct( char * buf, uintx size );
void            dba_destroy( TiDebugAgent * dba );
TiDebugAgent *  dba_attach( TiDebugAgent * dba, TiUartAdapter * uart );
void            dba_detach( TiDebugAgent * dba );

/* the evolve function should be called frequvently to send data to UART
 * or else all your debug operations only place data in the internal buffer
 * without sending them out.
 *
 * dba_evolve() should be called only in non-interrupt mode.
 */
void            dba_evolve( TiDebugAgent * dba, TiEvent * e );

#ifdef CONFIG_DBA_OUTPUT
uintx           dba_write( TiDebugAgent * dba, char * buf, uintx size );
#endif

#ifdef CONFIG_DBA_INPUT
void            dba_interpret( TiDebugAgent * dba, char * input, uintx len, char * output, uintx * size );
#endif


#ifdef __cplusplus
}
#endif

#endif
