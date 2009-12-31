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


/*******************************************************************************
 * @attention
 * the "cc2420" chip has address identification mechanism built. if you want to 
 * use it as a sniffer frontier, you must turn off it. 
 *
 * @author zhangwei(TongJi University) on 20070423
 * 	- revision today.
 * @modified by yan-shixing(TongJi University) on 20090725
 *  - revision. tested ok
 *
 * @modified by zhangwei(TongJi University) on 20091030
 *  - upgrade the frame output function. both the listener mode and non-listener 
 *    mode are using _output_openframe() now.
 * @modified by shi-miaojing(TongJi University) on 20091031
 *	- tested ok.
 * @modified by zhangwei
 *	- support both the binary format and text ascii format. You can use macro
 *    CONFIG_ASCII_OUTPUT to configure it.
 ******************************************************************************/ 

#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_cc2420.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_debugio.h"
#include "../common/rtl/rtl_openframe.h"
//#include "../common/rtl/rtl_ascii.h"

/*******************************************************************************
 * configuration
 * If you want to use the listener version, you should enable macro CONFIG_LISTENER
 * or else disable it. 
 * 
 * the listener of cc2420 adapter is actually invoked in the interrupt mode. 
 ******************************************************************************/ 

#undef  CONFIG_LISTENER    
#define CONFIG_LISTENER    

/* To choose which kind of format to output. Binary format or Text ASCII format. */

#define CONFIG_ASCII_OUTPUT
//#undef  CONFIG_ASCII_OUTPUT

#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

TiCc2420Adapter             g_cc;
TiUartAdapter               g_uart;
char                        g_rxbufmem[ OPF_SUGGEST_SIZE ];

#ifdef CONFIG_LISTENER
static void _cc2420_listener( void * ccptr, TiEvent * e );
#endif

static uint8 len=0;

static void sniffer(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

/*******************************************************************************
 * functions 
 ******************************************************************************/ 

int main(void)
{
    sniffer();
}

void sniffer(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;
	char * msg = "welcome to sniffer...\r\n";

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );
    dbo_open( 0, 38400 );

	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart = uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	#ifdef CONFIG_LISTENER
	cc = cc2420_open( cc, 0, _cc2420_listener, cc, 0x00 );
	#else
    cc = cc2420_open( cc, 0, NULL, NULL, 0x00 );
	#endif

	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							// enable RX mode
	cc2420_setpanid( cc, PANID );					// network identifier, seems no use in sniffer mode
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	// in network address, seems no use in sniffer mode
	cc2420_disable_addrdecode( cc );				// disable address decoding
	cc2420_disable_autoack( cc );
    opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
    // opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem), OPF_DEF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );

	hal_enable_interrupts();
 
	#ifdef CONFIG_LISTENER
	while(1) {}
	#endif

	#ifndef CONFIG_LISTENER
	while(1) 
	{
		len = cc2420_read( cc, opf_buffer(opf), opf_size(opf), 0x00 );
		if (len > 0)
		{
			/* since cc2420_read() only copies the received data into "opf"'s internal 
			 * buffer, we had to call opf_set_datalen() to tell "opf" how long the  
			 * received data is.
			 */
			opf_set_datalen( opf, len );
			_output_openframe( opf, &g_uart );
			led_toggle( LED_YELLOW );
        }
		cc2420_evolve( cc );
	}
	#endif
}

/* _cc2420_listener
 * This is a callback function handler of the TiCc2420Adapter. It will be called 
 * each time a new frame received by the cc2420 adapter. 
 * 
 * @attention
 *	Since the frame maybe queued inside cc2420 adapter, an "while" loop must be 
 * issued here to guarantee all the frames are read out from the adapter. 
 */
#ifdef CONFIG_LISTENER
void _cc2420_listener( void * owner, TiEvent * e )
{
	TiCc2420Adapter * cc = (TiCc2420Adapter *)(owner);
    TiOpenFrame * opf;
	// todo
	// must we call opf_open() every time here? attention we had already opened it 
	// during the initialization. can we open the opf structure for only 1 time 
	// and reuse it in the future?
	opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
	while (1)
	{
		len = cc2420_read( cc, opf_buffer(opf), opf_size(opf), 0x00 );
		if (len > 0)
		{
			/* since cc2420_read() only copies the received data into "opf"'s internal 
			 * buffer, we had to call opf_set_datalen() to tell "opf" how long the  
			 * received data is.
			 */
			opf_set_datalen( opf, len );
			_output_openframe( opf, &g_uart );
			led_toggle( LED_RED );
        }
		else 
			break;
	}
}
#endif

/* todo: you should replace dbo_xxx with sio_write. so that we can identify the frame 
 * in the byte stream in the PC side.
 */
#ifdef CONFIG_ASCII_OUTPUT
void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	//uint8 i;

    // if the opf structure contains an frame, then output it.
	if (opf_datalen(opf) > 0)
	{   
		dbo_putchar( '>' );
	 	dbo_n8toa( opf->datalen );

		if (opf_parse(opf, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbo_n8toa( *opf->sequence );
			dbo_putchar( ':' );
			_dbo_write_n8toa( (char*)&(opf->buf[0]), opf->buf[0]+1 );
		}
		else{
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			_dbo_write_n8toa( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( '\r' );
		dbo_putchar( '\n' );
	}
    else{
        // If the opf structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbo_putchar( '.' );
    }
}
#endif

#ifndef CONFIG_ASCII_OUTPUT
void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
    // if the opf structure contains an frame, then output it.
	if (opf_datalen(opf) > 0)
	{   
		dbo_putchar( 0x88 );
		dbo_putchar( 0x88 );
	 	dbo_putchar( opf->datalen );

		if (opf_parse(opf, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbo_putchar( *opf->sequence );
			dbo_putchar( 0x88 );
			dbo_putchar( 0x88 );
			dbo_write( (char*)&(opf->buf[0]), opf->buf[0]+1 );
		}
		else{
            dbo_putchar( 0x00 );
			dbo_putchar( 0x88 );
			dbo_putchar( 0x88 );
			dbo_write( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( 0x99 );
		dbo_putchar( 0x99 );
	}
    else{
        // If the opf structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbo_putchar( 0x88 );
        dbo_putchar( 0x88 );
		dbo_putchar( 0x99 );
		dbo_putchar( 0x99 );
    }
}
#endif



