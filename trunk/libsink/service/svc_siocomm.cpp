/*******************************************************************************
 * @author zhangwei on 20060813
 * TSioComm
 * Serial I/O Communication
 * based on TUartDriver
 * 
 * support both the stream interface and frame interface.
 * however, you'd better use one type interface only at the same time. 
 *
 * 构建在TUartDriver基础之上，Uart相当于物理层，SIO相当于Data Link Layer功能
 * 包括串口frame的区分功能
 * 内含一个供TUartDriver使用的通信缓冲区
 * 如果主程序来不及读数据，则可能导致该内部通信缓冲区种数据丢失。
 * 
 * TUartDriver => TSioComm => TConsole
 *                            and other applications
 * 
 * @modified by zhangwei on 20061015
 * revision and released version 3, not test yet!
 * problem: shall i keep the length char of the frame in the buffer?
 ******************************************************************************/

#include "..\hal\hal_foundation.h"
#include "..\hal\hal_uart.h"
#include "svc_foundation.h"
#include "svc_siocomm.h"
#include <stdio.h>
 
#define SIO_STATE_GENERAL 0
#define SIO_STATE_GETSOF 1
 
/* TSioComm object
 * @param
 * 	sio		the object instance pointer
 * 	uart	on which uart this SioComm object is build
 * 	opt		unused now
 */
TSioComm * sio_construct( char * buf, uint16 size, TUartDriver * uart, uint8 opt )
{
	TSioComm * sio;
	
	if (sizeof(TSioComm) > size)
		sio = NULL;
	else
		sio = (TSioComm *)buf;
	
	if (sio != NULL)
	{
		memset( buf, 0x00, size );
		sio->uart = uart;
		sio->rxlen = 0;
		sio->txlen = 0;	
		sio->option = opt;
		//sio->timer = NULL;
		sio->sof = SIO_DEFAULT_ESCAPE;
  		sio->fmstate = SIO_STATE_GENERAL;
  		sio->fmstate_sofcount = 0;
  		sio->fmstart = 0;
  		sio->fmlength = 0;
	}
	
	return sio;
}

void sio_destroy( TSioComm * sio )
{
	if (sio != NULL)
	{
		sio->rxlen = 0;
		sio->txlen = 0;
		// @TODO disable sio interrupt here if interrupt read and write enabled
	}
}

void sio_configure( TSioComm * sio, /*TTimer * timer,*/ uint8 opt, uint8 sof )
{
	sio->option = opt;
	//sio->timer = timer;
	//timer = timer;
	sio->sof = sof;
}

void sio_relation( TSioComm * sio, void * haldriver )
{
	NULL;
}

#ifdef SIO_CONFIG_VERSION_10
/* Direct through version for testing only.
 * 
 * @return
 * 	the char count actually put into the buffer;
 */
uint8 sio_read( TSioComm * sio, char * payload, uint8 size, uint8 opt )
{
	char * buf;
	uint8 avail, count;
	
	do{
		avail = SIOCOMM_RXBUFFER_CAPACITY - sio->rxlen;
		buf = (char *)(sio->rxbuf) + sio->rxlen;
		count = uart_read( sio->uart, buf, avail, opt );
		if (count == 0)
			break;
			
		sio->rxlen += count;
	}while ((avail > 0) && (count > 0)); 
	
	count = min( sio->rxlen, size );
	if (count > 0)
	{
		memmove( payload, &(sio->rxbuf[0]), count );
		sio->rxlen -= count;
		memmove( &(sio->rxbuf[0]), &(sio->rxbuf[count]), sio->rxlen ); 
	}
		
	return count;
}
#endif

#ifdef SIO_CONFIG_VERSION_30
uint8 sio_read( TSioComm * sio, char * payload, uint8 size, uint8 opt )
{
	uint8 count;
	FILE *stream_source;
	sio_evolve( sio );
	count = min( sio->rxlen, size );   
	stream_source=fopen("D:\\source\\sio_raw_revcived(sio_read).txt","w+");
	fseek(stream_source,0L,SEEK_SET); 	
	
	if (count > 0)
	{
		memmove( payload, &(sio->rxbuf[0]), count );
		sio->rxlen -= count;
		memmove( &(sio->rxbuf[0]), &(sio->rxbuf[count]), sio->rxlen ); 
					
		fwrite(&payload,count,1,stream_source);			
	
		/*
		// the following code is to adjust the frame settings. this will enable
		// you to get almost the correct frame when call rawread() after read().
		// thus you can hybrid using of rawread() and read()
		//
		if (sio->fmstart == 0)
		{
			sio->fmlength = (sio->fmlength > count) ? (sio->fmlength - count) : 0;
		}
		else{
			if (sio->fmstart <= count)
			{
				sio->fmstart = 0;
				sio->fmlength = 0;
			}
			else
				sio->fmstart -= count;
		}
		*/
	}
	fclose(stream_source);	
	return count;
}
#endif

#ifdef SIO_CONFIG_VERSION_10
/* 行为与sio_read类似，但sio_read中参数payload指向的是有效载荷，不含串口通信中可能加入的
 * 那些控制字节，如串口frame长度等信息和frame之间的区分字节等。
 * 本函数则是将一个完整的frame原封不动的放入到frame所指内存空间中。
 * this version is for test only
 */
uint8 sio_rawread( TSioComm * sio, char * buf, uint8 size, uint8 opt )
{
	return uart_read( sio->uart, buf, size, opt );
}
#endif

#ifdef SIO_CONFIG_VERSION_30
/* @attention
 * when this function receive an corrupt frame, for example, only a part of a 
 * frame, it will wait for the left data. thus sio_rawread() will not return 
 * if it doesnot receive an full frame. 
 *  
 *    first frame          second frame
 *   0           fmstart                     txlen
 *   |--------------|-------------------------|------
 *                  |-------------fmlength---------|
 * 
 * @param
 * 	opt 	0x00, no use now
 */
uint8 sio_rawread( TSioComm * sio, char * buf, uint8 size, uint8 opt )
{
	uint8 framelength = 0;
	
	sio_evolve( sio );
	if (sio->fmlength > 0) // frame found
	{
		if (sio->fmstart > 0)
			framelength = sio->fmstart;
		else
			framelength = (sio->fmlength < sio->txlen) ? 0 : sio->fmlength;
	}
		
	if (framelength > 0)
	{
		framelength = min( framelength, SIO_FRAMEBUFFER_LENGTH );
		buf[0] = 0x01;  // frame control char
		buf[1] = 0x00;	// address field (no use, but you should keep them here)
		buf[2] = 0x00;  
		assert( framelength+3 <= size ); 
		memmove( &(buf[3]), &(sio->rxbuf[0]), framelength );
		sio->rxlen -= framelength;
		memmove( &(sio->rxbuf[0]), &(sio->rxbuf[framelength]), sio->rxlen );

		if (sio->fmstart == 0)
			sio->fmlength = 0;
		else
			sio->fmstart -= framelength; // should equal to 0 now.
	}
	
	return framelength;
}
#endif

#ifdef SIO_CONFIG_VERSION_10
/*　write a frame to the TSioComm object.
 * 
 *　@attention
 * @warning
 * 	since sio_write() is based on uart_write(), it's behavior is also resemble
 * uart_write(). You may often write multiple times to finish sending one buffer's
 * data.
 * 
 * @return
 * the char count wrotten successfully. 
 */ 
uint8 sio_write( TSioComm * sio, char * payload, uint8 len, uint8 opt )
{
	return sio_rawwrite( sio, payload, len, opt );
}
#endif

#ifdef SIO_CONFIG_VERSION_30
/* write a frame to the TSioComm object.
 * @attention
 * @warning
 * 	since sio_write() is based on uart_write(), it's behavior is also resemble
 * uart_write(). You may often write multiple times to finish sending one buffer's
 * data.
 * 	before really sending to UART, the data may assembled to a frame.
 */
uint8 sio_write( TSioComm * sio, char * buf, uint8 len, uint8 opt )
{
	uint8 count = 0;

	if (sio->txlen == 0)
	{
		count = (uint8)uart_write( sio->uart,(char *) buf, len, opt );
	}
	else{
		count = min( SIO_TXBUFFER_CAPACITY - sio->txlen, len );
		memmove( (char *)(sio->txbuf) + sio->txlen, buf, count );
	}
	sio_evolve( sio );
	
	return count;
}
#endif

#ifdef SIO_CONFIG_VERSION_10
/* write every char in the buffer "sioframe" to UART driver. usually, the data 
 * in the buffer is an entire frame. this version is for test only!
 * 
 * @return
 * char count wrotten successfully to UART.
 */
uint8 sio_rawwrite( TSioComm * sio, char * buf, uint8 len, uint8 opt )
{
	return uart_write( sio->uart, buf, len, opt );
}
#endif

#ifdef SIO_CONFIG_VERSION_30
/* write an entire frame to the uart driver to send it to the HOST/PLC.
 * @param
 * opt = 0x00: the "buf" contains an entire frame with frame header in it. 
 * opt = 0x01: the "buf" contains payload of the frame only. "rawwrite" will 
 * 	construct the frame header (default settings) for the master program.
 * @return
 */
uint8 sio_rawwrite( TSioComm * sio, char * buf, uint16 len, uint8 opt )
{
	uint8 count=0;
	
	if (sio->txlen == 0) 
	{
		if (opt & 0x01)
		{
			sio->txbuf[0] = 0x00; // frame control char
			sio->txbuf[1] = 0x00; // address field
			sio->txbuf[2] = 0x00; 
			count = min( len, SIO_FRAMEBUFFER_LENGTH-3 );
			memmove( &(sio->txbuf[3]), buf, count );  
			sio->txlen = count+3;
		}
		else{
			count = min( len, SIO_FRAMEBUFFER_LENGTH );
			memmove( &(sio->txbuf[0]), buf, count );
			sio->txlen = count;
		}
	}
	sio_evolve( sio );
	
	return count;
}
#endif

#ifdef SIO_CONFIG_VERSION_30
/* @modified by zhangwei on 20061015
 * version 3 evolve function. this version will read and write data in evolve 
 * function instead of sio_read() and sio_write().
 * 
 * @attention
 * @TODO multiple frame identification and queuing
 * 
 * 								ATTENTION
 * this version of evolve() can only recognize 1 frame in the input stream!
 * the later frames in the "rxbuf" will be regarded as one whole frame! you'd 
 * better push these frames into a queue so that we can support multiple frames
 * in the future!
 */
void sio_evolve( TSioComm * sio )
{
	char ch;
	char temp[4];
	uint8 framestart=0, framelength=0, n;
	boolean found = FALSE;
    memset(temp,0,3);

	if (sio->option & SIO_OPTION_FRAME_SUPPORT)
	{
		// execute query-based receiving process	
		// 
		do{
			if (sio->rxlen >= SIO_RXBUFFER_CAPACITY)
				break;
					
			if (uart_getchar(sio->uart, &ch) < 0)
				break;
			
			/* check whether a complete frame has been read.*/		
			if (sio->rxlen == sio->fmlength) break;
			switch (sio->fmstate)
			{
			// initial state, no SOF char received
			case SIO_STATE_GENERAL:
				if (ch == sio->sof)
				{
					sio->fmstate = SIO_STATE_GETSOF;
					sio->fmstate_sofcount = 1;
				}
				else{				
					sio->rxbuf[sio->rxlen++] = ch;
				}
				break; 
					
			// received SOF. and the sofcount will keep how many SOF received
			// in this state.
			case SIO_STATE_GETSOF:
				/* store the framelength if sio->fmstate_sofcount % 2 == 1; */
				if ((sio->fmstate_sofcount == 1) && (sio->fmlength == 0))
				{
					sio->fmlength = ch;
					break;				
				}

				if (ch == sio->sof)
				{
					sio->fmstate_sofcount ++;
					if (sio->fmstate_sofcount % 2 == 0) // sofcount is even 
						sio->rxbuf[sio->rxlen++] = ch;
				}
				else{
			/*		if (sio->fmstate_sofcount % 2 == 1) // sofcount is odd
					{
						framelength = ch; // this is the length char according to UART frame
						framestart = sio->rxlen - (sio->fmstate_sofcount >> 1);
						found = TRUE;
					}
            */
					sio->fmstate = SIO_STATE_GENERAL;
				}
				break;
			}
		}while(TRUE);

		//WORD a = 1234;
		//MessageBox(0,(LPCWSTR)(sio->rxlen),(LPCWSTR)&a,0);
		
		// @TODO now only support 1 frame identification! you should improve it
		// to support multiple frames. this needs an queue. 20061016
		//	
	/*	if (found)
		{
			if (sio->fmlength == 0)
			{
				sio->fmstart = framestart;
				sio->fmlength = sio->rxbuf[framestart]; 
			}
		}
     */
		// execute the write process
		// encapsulate the SioComm frame to PHY frame by adding a SOF start flag
		// char and a length char. 
		//
		if (sio->txlen > 0)
		{
			framelength = min( sio->txlen, SIO_FRAMEBUFFER_LENGTH ) + 1;
			while (uart_putchar( sio->uart, sio->sof ) != 0) NULL;
			// @TODO: whether the length char has already placed into the buffer?
			// confirm it. pls 20061025
			//while (uart_putchar( sio->uart, framelength ) != 0) NULL;
			for (n=0; n<framelength-1; n++)
			{
				while (uart_putchar( sio->uart, sio->txbuf[n] ) !=0) NULL;
				if (sio->txbuf[n] == sio->sof)
					while (uart_putchar( sio->uart, sio->sof ) != 0) NULL;
			}
			
			framelength --;
			memmove( (char*)(sio->txbuf[0]), (char*)(sio->txbuf[0])+framelength, sio->txlen-framelength );
			sio->txlen -= framelength;
		}
	}
	
	// when you choose not to support frame
	// just try uart read and write char by char. 
	else{  
		while(TRUE)
		{
			if (sio->rxlen >= SIO_RXBUFFER_CAPACITY)
				break;
					
			if (uart_getchar(sio->uart, &ch) < 0)
				break;
			
			sio->rxbuf[sio->rxlen++] = ch;
		};
		
		n = (uint8)uart_write( sio->uart, &(sio->txbuf[0]), sio->txlen, 0x00 );
		sio->txlen -= n;
	}
	
	return;		
}		

#undef SIO_STATE_GENERAL 
#undef SIO_STATE_GETSOF
#endif

