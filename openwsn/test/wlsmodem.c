//----------------------------------------------------------------------------
// @author zhangwei on 2006-10-10
// @note wlsmodem V 1.0
//
// 一个实用的wireless modem / wlsmodem / wlsmodem 测试程序
// 演示如何使用wlscomm，同时也测试wlscomm
//
// @modified by zhangwei on 20061024
// add support to two work mode. stream mode and frame-based group mode
//
// @modified by zhangwei on 20061025
// revised the frame-based group mode. add support that the HOST/PLC can decide 
// the frame destination by allowing it to change the frame header.
//----------------------------------------------------------------------------

#include <../configall.h>
#include <../foundation.h>
#include <../hal/hal_foundation.h>
#include <../hal/hal_key.h>
#include <../service/svc_foundation.h>
#include <../service/svc_siocomm.h>
#include <../service/svc_wlscomm.h>
#include <../service/svc_debugio.h>
#include <../service/svc_configure.h>
#include <../global.h>
#include "wlsmodem.h"

#define MODEM_BUFFER_SIZE (CONFIG_MAX_UART_FRAME_LENGTH * 2)

#ifdef GDEBUG
static char * m_debugwelcome = "Wireless Modem Started...";
#endif

static char m_txbuf[MODEM_BUFFER_SIZE];
static char m_rxbuf[MODEM_BUFFER_SIZE];
static uint8 m_txlen=0;
static uint8 m_rxlen=0;

int8 on_frame_notify( void * data );

static void wlsmodem_stream( void );
static void wlsmodem_group( void );
static void wlsmodem_openmac( void );
static void wlsmodem_configure( void );

// Q: how to distinguish TX and RX?
// R: 站在PLC/Host角度区分术语 TX 还是 RX
// 队列中每一个元素都是一个有头有尾的frame
// 
// @note
// @TODO you'd better replace the current "txbuf" and "rxbuf" with an queue 
// to improve the performance. every item in the queue is an entire frame's 
// payload with address information.
//
void wlsmodem_start( void )
{
	global_construct();
	debug_write( g_debugio, m_debugwelcome, strlen(m_debugwelcome)+1 );

	if (key(KEY_CONFIG) == KEY_ON)
	{
		config_execute( g_config );
		hal_reboot();
		return;
	}

	wlsmodem_configure();
	
	memset( (char*)(&m_txbuf[0]), 0x00, MODEM_BUFFER_SIZE ); 
	memset( (char*)(&m_rxbuf[0]), 0x00, MODEM_BUFFER_SIZE ); 
	m_txlen = 0;
	m_rxlen = 0;

	switch (g_config->data.mode)
	{
	case CONFIGURE_MODE_MODEMGROUP:
		wlsmodem_group();
		break;
	case CONFIGURE_MODE_STREAM:
		wlsmodem_stream();
		break;
	case CONFIGURE_MODE_OPENMAC:
		wlsmodem_openmac();
		break;
	}
		
	global_destroy();	
	
	return;
}

/* the modem architecture support the following work mode:
 * - transparent STREAM mode
 * this mode only allow two modem to communicate each other. they pair functions 
 * like they were a wire. any data sending to the TX modem will be transfered to 
 * the receiver device by RX modem. 
 * 
 * - modem GROUP mode
 * this mode allow two or more modems to communicate each other. each modem has 
 * an unique identifier called "address". when the sender device want to send a 
 * packet to receiver device, it must encapsulate the data into one standard 
 * frame, and send this frame to the TX modem. then TX modem will analyze the 
 * frame to extract the "address" information and finally send it out to the RX 
 * modem. the RX data to the receiver device is also a standard frame containing 
 * "sender" address information.
 * 
 * - modem OPENMAC mode
 * in group working mode, every device can send frames to other frames, including 
 * broadcasting. however, there are no collision detection or avoidance mechanism.
 * namely, two device may send frames at the same time, thus collision occured. 
 * OPENMAC mode will do some cooperation among these devices. however, this may 
 * lead to degrade the performance than simple GROUP mode.
 */
void wlsmodem_group( void )
{
	char * buf;
	uint16 addr, lastaddr;
	uint8 fmctrl, count;
	
	lastaddr = g_config->data.rmtaddress;
	while (1)
	{
		// send data out through the wireless channel
		// attention that the payload is started from the third byte in the buffer.
		if (m_txlen > 0)
		{
			buf = (char*)(m_txbuf[3]);
			count = wls_rawwrite( g_wls, buf, m_txlen, 0x01 );
			if (count > 0)
			{
				m_txlen -= count;
				memmove( buf, buf + count, m_txlen );
			}
		}
			
		// send data out through the serial channel
		// attention that the buffer should contain an valid frame now. 
		if (m_rxlen > 0)
		{
			buf = (char*)(m_rxbuf[0]);
			count = sio_rawwrite( g_sio, buf, m_rxlen, 0 );
			if (count > 0)
			{
				m_rxlen -= count;
				memmove( buf, buf + count, m_rxlen );
			}
		}
			
		if (m_txlen == 0)
		{
			buf = (char*)(m_txbuf[0]) + m_txlen;
			count = sio_rawread( g_sio, buf, MODEM_BUFFER_SIZE - m_txlen, 0x00 );
			m_txlen += count;
			if (count > 0) 
			{
				fmctrl = buf[0];
				// if this is a standard data frame 
				if (fmctrl & 0x81 == 0x00)
				{
					addr = (uint16)buf[2];
					addr = (addr << 8) | (uint16)buf[1];
					if (addr != lastaddr)
					{
						wls_setrmtaddress( g_wls, addr );
						lastaddr = addr;
					}
				} 
			}
		}
			
		if (m_rxlen == 0)
		{
			buf = (char*)(m_rxbuf[6]) + m_rxlen;
			count = wls_rawread( g_wls, buf, MODEM_BUFFER_SIZE - m_rxlen, 0x00 );
			m_rxlen += count;
			// construct the frame header
			buf[0] = 0x00;
			buf[1] = 0x00;
			* (uint16 *)(&buf[2]) = wls_getrmtaddress( g_wls );
			m_rxlen += 4;
		}
			
		// send debug data to debug UART if exists.
		debug_evolve( g_debugio );
	}
}

void wlsmodem_stream( void )
{
	/* 如下版本与上一版本大部分相同，仅仅在于用sio_read() sio_write() wls_read() wls_write()
	 * 替换了sio_rawread() sio_rawwrite() wls_rawread() wls_rawwrite()
	 */
	char * buf;
	uint8 count;
	
	wls_setrmtaddress( g_wls, g_config->data.rmtaddress );
	while (1)
	{
		if (m_txlen > 0)
		{
			buf = (char*)(m_txbuf[0]);
			count = wls_rawwrite( g_wls, buf, m_txlen, 0 );
			if (count > 0)
			{
				m_txlen -= count;
				memmove( buf, buf + count, m_txlen );
			}
		}
			
		if (m_rxlen > 0)
		{
			buf = (char*)(m_rxbuf[0]);
			count = sio_rawwrite( g_sio, buf, m_rxlen, 0 );
			if (count > 0)
			{
				m_rxlen -= count;
				memmove( buf, buf + count, m_rxlen );
			}
		}
	
		buf = (char*)(m_txbuf[0]) + m_txlen;
		count = sio_read( g_sio, buf, MODEM_BUFFER_SIZE - m_txlen, 0x00 );
		m_txlen += count;
			
		buf = (char*)(m_rxbuf[0]) + m_rxlen;
		count = wls_read( g_wls, buf, MODEM_BUFFER_SIZE - m_rxlen, 0x00 );
		m_rxlen += count;
			
		// send debug data if exists.
		debug_evolve( g_debugio );
	}
}

void wlsmodem_openmac( void )
{
	NULL;
}

void wlsmodem_configure()
{
	TConfigStore * data;
	data = &(g_config->data);

	uart_configure( g_uart0, data->uart_baudrate, data->uart_databits, data->uart_stopbits, data->uart_parity, 0x01 );
	cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, data->panid, 1 );
	cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, data->localaddress, 2 );
 	cc2420_configure( g_cc2420, CC2420_CONFIG_TUNNING_POWER,(uint16) data->wlspower, 1 );
	cc2420_configure( g_cc2420, CC2420_CONFIG_CHANNEL, (uint16) data->wlschannel, 1 );
	cc2420_configure( g_cc2420, CC2420_CONFIG_APPLY, NULL, 0 );
}

int8 on_frame_notify( void * data )
{
	TUartDriver * uart = (TUartDriver *)data; 
	uint8 cmd0 = uart->rxbuf[0];
	uint8 addr = uart->rxbuf[1];
	
	cmd0 = cmd0;
	addr = addr;
	return 1;
}

#undef MODEM_BUFFER_SIZE

