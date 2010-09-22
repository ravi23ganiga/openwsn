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

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/hal/hal_assert.h"
//#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "../../common/openwsn/rtl/rtl_ascii.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
//#include "../../common/openwsn/rtl/rtl_openframe.h"
#include "apl_ieee802frame154_dump.h"

/* CONFIG_LISTENER
 * If you define this macro, the sniffer will be driven by the listener function. 
 * attention currently, the listener of cc2420 adapter is actually called in the 
 * interrupt mode. so you cannot do complicated time consumption operations inside
 * the listener.
 * 
 * CONFIG_ASCII_OUTPUT
 * To choose which kind of format to output. Binary format or Text ASCII format.
 * to decide the output format of the sniffer. If you define this macro, the sniffer
 * will convert the binary frame values into hex ascii format. each byte will be 
 * represented as two ascii characters.
 */

#undef  CONFIG_LISTENER    
#define CONFIG_LISTENER    

#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

#define MAX_IEEE802FRAME154_SIZE    128



static TiCc2420Adapter             g_cc;
//static TiUartAdapter             g_uart;
//static char                        g_rxbufmem[ OPF_SUGGEST_SIZE ];
static char                        m_frame[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];

#ifdef CONFIG_LISTENER
static void _cc2420_listener( void * ccptr, TiEvent * e );
#endif

//static uint8 len=0;

static void sniffer(void);
//static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

/*******************************************************************************
 * functions 
 ******************************************************************************/ 

int main(void)
{
    sniffer();
}

void sniffer(void)
{
    //uint8 count;
    TiCc2420Adapter * cc;
    TiFrame * frame;
	//TiUartAdapter * uart;
	//TiOpenFrame * opf;
	//TiFrame * rxbuf;
	char * msg = "welcome to sniffer binary...";

	#ifndef CONFIG_LISTENER
    int8 len=0;
    #endif

	target_init();

	led_open();
	led_on( LED_RED );
	hal_delay( 500 );
	led_off( LED_ALL );

    //dbo_open( 38400 );
    rtl_init( dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_putchar( 0xFF );
    dbc_mem( msg, strlen(msg) );
    dbc_putchar( 0xFF );

	//uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	//uart = uart_open( uart, 0, 38400, 8, 1, 0x00 );
	//uart_write( uart, msg, strlen(msg), 0x00 );

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

    //rxbuf = frame_open( (char*)(&g_rxbufmem),  FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	//opf = opf_open( (void *)(&g_rxbufmem[0]), sizeof(g_rxbufmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
    // opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem), OPF_DEF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
    frame = frame_open( (char*)(&m_frame), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 0, 0, 0 );

	hal_enable_interrupts();
 
	#ifdef CONFIG_LISTENER
	while(1) {}
	#endif

	#ifndef CONFIG_LISTENER
	while(1) 
	{
        frame_reset(frame, 0, 0, 0);
        len = cc2420_read( cc, frame_startptr(frame), frame_capacity(frame), 0x00 );
        if (len > 0)
        {
            frame_setlength( frame, len );
            ieee802frame154_dump( frame );
			led_toggle( LED_YELLOW );
        }
        /*
		len = cc2420_read( cc, opf_buffer(opf), opf_size(opf), 0x00 );
		if (len > 0)
		{
			// since cc2420_read() only copies the received data into "opf"'s internal 
			// buffer, we had to call opf_set_datalen() to tell "opf" how long the  
			// received data is.
			//
			frame_setlength( rxbuf, count );
			_output_openframe( opf, &g_uart );
			led_toggle( LED_YELLOW );
        }
        */
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
    TiFrame * frame = (TiFrame *)&m_frame;
	//TiFrame * rxbuf;
    int8 len=0;

	// the following flag indicates the _cc2420_listener() function is called.
	dbc_putchar( 0xF0 );
	
	// todo
	// must we call opf_open() every time here? attention we had already opened it 
	// during the initialization. can we open the opf structure for only 1 time 
	// and reuse it in the future?
	while (1)
	{
        frame_reset(frame, 0, 0, 0);
        len = cc2420_read( cc, frame_startptr(frame), frame_capacity(frame), 0x00 );
        if (len > 0)
        {
            frame_setlength( frame, len );
            ieee802frame154_dump( frame );
			led_toggle( LED_YELLOW );
        }
        else
            break;

        /*
		len = cc2420_read( cc, opf_buffer(opf), opf_size(opf), 0x00 );
		if (len > 0)
		{
			// since cc2420_read() only copies the received data into "opf"'s internal 
			// buffer, we had to call opf_set_datalen() to tell "opf" how long the  
			// received data is.
			//
			frame_setlength( rxbuf, count );
			_output_frame( rxbuf, &g_uart );
			led_toggle( LED_RED );
        }
		else 
			break;
        */
	}
}
#endif
