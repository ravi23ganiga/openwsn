#ifndef _SIOCOMM_H_1478_
#define _SIOCOMM_H_1478_

/*******************************************************************************
 * @author zhangwei on 20060813
 * TSioComm
 * Serial I/O Communication
 * based on TUartDriver
 * 
 * support both the stream interface and frame interface.
 * however, you'd better use one type interface only at the same time. 
 *
 * TUartDriver => TSioComm => TConsole
 *                            and other applications
 ******************************************************************************/

#include "svc_foundation.h"
#include "..\hal\hal_uart.h"
#include "..\hal\hal_timer.h"

/*******************************************************************************
 * UART PDU format
 * SIO PDU is the data frame transmitted on the physical UART. 
 * 		[1B SOF][1B Framelength][1B Frame Control][nB PSDU/Payload]{[2 FCS]}
 * SOF means start of frame. The length char is the data length + control 1 + length 1
 * 
 * SIO PDU is the part after deleting SOF and length char in UART PDU
 * 		[1B Frame Control][nB PSDU/Payload]
 * 
 * actually, they two share almost the same settings except the first char SOF.
 * when you adopt frame support, you should check every char from the framelength 
 * to the end. if the char equals to SOF, then should insert another SOF to indicate
 * the case.
 ******************************************************************************/

/* configure the SIOCOMM implementation
 * 
 * SIO_CONFIG_VERSION_10
 * this macro will enable the SIOCOMM module adopt direct "pass through" model.
 * this model is very simple and is ideal for testing.
 * 
 * SIO_CONFIG_VERSION_20 
 * has been elminated from the source code
 * 
 * SIO_CONFIG_VERSION_30(default)
 * support smart frame splitting of input stream from UART.
 * support frame & non-frame hybrid programming without data loss.
 * however, hybrid programming may lead to time delay some times! @TODO 20061015
 * this occurs when you receive half a frame in the "rxbuf", however, this case
 * seldom happens. 
 */
#undef  SIO_CONFIG_VERSION_10
#define SIO_CONFIG_VERSION_10


#define SIO_CONFIG_VERSION_30
#undef  SIO_CONFIG_VERSION_30


/* configure TSioComm's internal buffer size
 * MAX_FRAAME_LENGTH should be less than 0xFF. the maximum value of it is 0xFE
 * the default settings in our application is 0x7E (127). not 0xFF(128)
 */
#define SIO_FRAMEBUFFER_LENGTH CONFIG_MAX_UART_FRAME_LENGTH-3
#define SIO_RXBUFFER_CAPACITY SIO_FRAMEBUFFER_LENGTH
#define SIO_TXBUFFER_CAPACITY SIO_FRAMEBUFFER_LENGTH

#define SIO_DEFAULT_ESCAPE CONFIG_START_OF_FRAME 
 
#define SIOCOMM_RXBUFFER_CAPACITY 100

/* this object is built on top of TUartDriver object. It functions like the 
 * data link layer. 
 * 
 * 	rxlen		character count in the rxbuf
 * 	rxbuf		receiving buffer
 * 	sof			start of frame
 * 	lengthindex	the index of length char in the frame. usually it is 1.
 * 				which means the second char in the frame is the length.
 * 				the first char is the start flag of the fame(namely SOF).
 * 	framestart	where's the new frame start
 * 	framestop	where's the new frame stop
 * 
 * @attention
 * due to the memory management, the txqueue and txbuf, rxqueue and txbuf 
 * should be neighbors.
 */
 
#define SIO_OPTION_FRAME_SUPPORT 0x01
#define SIO_OPTION_INTERRUPT_READ 0x02
#define SIO_OPTION_INTERRUPT_WRITE 0x04
#define SIO_OPTION_OTHER5 0x10
#define SIO_OPTION_OTHER6 0x20
#define SIO_OPTION_REMOTE_CONTROL 0x40
 
typedef struct{
  TUartDriver * uart;
  uint8 rxlen;
  uint8 txlen;
  char rxbuf[SIO_RXBUFFER_CAPACITY];
  char txbuf[SIO_TXBUFFER_CAPACITY];  
  uint8 option;
  // the following are to support version 3
  uint8 sof; 
  uint8 fmstate;
  uint8 fmstate_sofcount;
  uint8 fmstart;
  uint8 fmlength;
}TSioComm;

TSioComm * sio_construct( char * buf, uint16 size, TUartDriver * uart, uint8 opt );
void sio_configure( TSioComm * sio, TTimer * timer, uint8 opt, uint8 sof );
void sio_destroy( TSioComm * sio );
uint8 sio_read( TSioComm * sio, char * payload, uint8 size, uint8 opt );
uint8 sio_rawread( TSioComm * sio, char * buf, uint8 size, uint8 opt );
//uint8 sio_write( TSioComm * sio, char * payload, uint8 len, uint8 opt );
uint8 sio_write( TSioComm * sio, char * buf, uint8 len, uint8 opt );
uint8 sio_rawwrite( TSioComm * sio, char * buf, uint8 len, uint8 opt );
void sio_evolve( TSioComm * sio );

#endif
