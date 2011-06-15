/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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
 * This test demostrate how to send a frame using the TiCc2420Adapter component. 
 * It also demostrates how to manipulate the 802.15.4 frame format by using the 
 * TiFrame and TiIEEE802Frame154Descriptor object.
 * 
 * @status 
 *	Finished developing. Tested. Revised.
 *
 * @author HuangHuan in 2005.12
 * @modified by MaKun on 2007.04.18
 *	- test sending/TX function of cc2420 driver
 *
 * @modified by ZhangWei on 2007.04.18
 *	- just modify the format of the source file and including files
 * 
 * @modified by Zhang Wei on 20070701
 *	- port to OpenNode-3.0 hardware
 *	- and speed the transimisson rate. 
 * @modified by Zhang Wei in 2009.07
 *	- ported to ICT GAINZ platform (based on atmega128)
 * @modified by Yan-Shixing in 2009.07
 *	- correct bugs and tested. now the two send functions sendnode1() and sendnode2()
 *    are all work success.
 * @modified by Zhang Wei in 2011.04.17
 *  - Revised. This application will broadcast frames rapidly and as a "bad" node
 *    to testing communication performance. 
 * @modified by Jiang Ridong in 2011.04.18
 *	- Tested.
 ******************************************************************************/ 

#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../../common/openwsn/hal/hal_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_interrupt.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_assert.h"
#include "../../common/openwsn/hal/hal_uart.h"
#include "../../common/openwsn/hal/hal_cc2420.h"
#include "../../common/openwsn/hal/hal_targetboard.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/hal/hal_debugio.h"
#include "apl_output_frame.h"

/**
 * CONFIG_OPTION_ACK_REQUEST
 * Decides whether require ACK when sending
 * 
 * CONFIG_OPTION_CARRIER_SENSE
 * Decides whether to perform carrier sense before transmission.
 * 
 * CONFIG_SENDING_INTERVAL
 * Decides the sending rate. This macro defines the interval based on milliseconds 
 * between two consecutive sendings. 
 */

#ifdef CONFIG_DEBUG
    #define GDEBUG
#endif

#define CONFIG_OPTION_ACK_REQUEST
#undef CONFIG_OPTION_ACK_REQUEST

#define CONFIG_OPTION_CARRIER_SENSE
#undef CONFIG_OPTION_CARRIER_SENSE

#define CONFIG_SENDING_INTERVAL 	2

#define MAX_IEEE802FRAME154_SIZE    128
#define PANID						0x0001
#define LOCAL_ADDRESS				0x03  
#define REMOTE_ADDRESS				0x04
#define BUF_SIZE					128
#define DEFAULT_CHANNEL     		11

static TiCc2420Adapter		        m_cc;
static char                         m_txbufmem[FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE)];
static TiIEEE802Frame154Descriptor  m_desc;

void sendnode1(void);
//void sendnode2(void);

int main(void)
{
	sendnode1();
	//sendnode2();
}

void sendnode1(void)
{
	char * msg = "welcome to cc2420 sendnode(bad guy)...";
    TiCc2420Adapter * cc;
	TiFrame * frame;
    TiIEEE802Frame154Descriptor * desc;
	char * ptr;
	uint8 i, seqid=0, option, len;
	uint8 first;

	target_init();
	wdt_disable();
	
	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

	// initialize the debug console for input and output
    rtl_init( (void *)dbio_open(38400), (TiFunDebugIoPutChar)dbio_putchar, (TiFunDebugIoGetChar)dbio_getchar, hal_assert_report );
    dbc_mem( msg, strlen(msg) );
	
	// @todo: broadcast address maybe better for this application

	cc = cc2420_construct( (void *)(&m_cc), sizeof(TiCc2420Adapter) );
	cc2420_open( cc, 0, NULL, NULL, 0x00 );
	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							// Enable RX mode
	cc2420_enable_addrdecode( cc );					// Enable network address recognition(ʹ�ܵ�ַ����)
	cc2420_setpanid( cc, PANID );					// Set network identifier(�����ʶ)
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	// Set in-network identifier(���ڱ�ʶ)
    cc2420_enable_autoack( cc );

	option = 0x00;
	
	#ifdef CONFIG_OPTION_ACK_REQUEST
	option |= 0x01;
	cc2420_enable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_ACK;             // 0x8821;    
	#endif
	
	#ifdef CONFIG_OPTION_CARRIER_SENSE
	option |= 0x02;
	cc2420_disable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK;			// 0x8801;  
	#endif
	 
	frame = frame_open( (char*)(&m_txbufmem), FRAME_HOPESIZE(MAX_IEEE802FRAME154_SIZE), 3, 20, 0 );
	desc = ieee802frame154_open( &(m_desc) );

	hal_enable_interrupts();

	while(1)  
	{   
		// Set application layer data in the frame 
		frame_reset( frame, 3, 20, 0 );
        
		ptr = frame_startptr( frame);
		
		for ( i=0;i< 6;i++)
		{
			ptr[i] = 0xff;
		}

		frame_setlength( frame,6);
		// Set mac layer frame format
		frame_skipouter( frame, 12, 2 );  
		desc = ieee802frame154_format( &(m_desc), frame_startptr(frame), frame_capacity(frame), FRAME154_DEF_FRAMECONTROL_DATA  );
		rtl_assert( desc != NULL );

		ieee802frame154_set_sequence( desc, seqid++ );
		ieee802frame154_set_panto( desc, PANID );
		ieee802frame154_set_shortaddrto( desc, REMOTE_ADDRESS );
		ieee802frame154_set_panfrom( desc,PANID );
		ieee802frame154_set_shortaddrfrom( desc, LOCAL_ADDRESS );

		// Set payload of the frame
		//ptr = ieee802frame154_msdu( desc );
		/*for (i=0; i<ieee802frame154_msdu_len(desc); i++)
			ptr[i] = 5;*/
			
		#ifdef CONFIG_DEBUG
		//ieee802frame154_dump( frame );
		_output_frame( frame );
		#endif
		
		// Broadcast a frame out to interfere the communication of other nodes.
		// The "bad guy" node doesn't care whether the sending is really successfully
		// or not.
        frame_setlength( frame,20);
		first = frame_firstlayer(frame);
		// @todo
		// Q: shall we replace cc2420_write with broadcast and the destination address with 0xFFFF?
		// len = cc2420_write(cc, frame_layerstartptr(frame,first), frame_layercapacity(frame,first), 0x00);
		len = cc2420_write(cc, frame_startptr(frame), frame_length(frame), option);
        if (len > 0)
        {   
			led_toggle( LED_GREEN);
	 		dbc_putchar( len );
            //dbc_putchar( ieee802frame154_seqid(frame) );
        }
		
		// @important 
        // @attention This delay controlls how fast this node broadcast frames. 
		// Decrease this value to generate more conflictions and enlarge this value 
		// to generate fewer conflictions.
		//
		// This delay cannot be missed because the call to cc2420_write() will start
		// wireless transmission immediately. If you omit this delay between two 
		// subsequent sendings, then all the neighbor nodes may failed sending because
		// this bad guy node occpys the channel. It should give up the channel 
		// occasionally to give other nodes the chance to sending.
		
 		//hal_delay( CONFIG_SENDING_INTERVAL );
		hal_delayus( 5000);
		//hal_delay( 1000 );

		// @modified by zhangwei in 2008
		// You needn't call cc2420_evolve() because we don't use it to control the 
		// cc2420 internal state transfer for this application.
		//
		// cc2420_evolve( cc );
	}
}

/*
void sendnode2(void)
{
    TiCc2420Adapter * cc;
    TiUartAdapter * uart;
    char * msg = "welcome to sendnode...";
    uint8 frame[BUF_SIZE];
    uint8 len, j;
    #ifdef GDEBUG
    uint8 status;
    #endif

    target_init();
    HAL_SET_PIN_DIRECTIONS();
    wdt_disable();

    led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

    cc = cc2420_construct( (void *)(&m_cc), sizeof(TiCc2420Adapter) );
    uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

    uart_open( uart, 0, 38400, 8, 1, 0x00 );
    uart_write( uart, msg, strlen(msg), 0x00 );
    cc2420_open( cc, 0, NULL, NULL, 0x00 );

    cc2420_setchannel( cc, DEFAULT_CHANNEL );
    cc2420_setrxmode( cc );								//Enable RX
    //cc2420_enable_addrdecode( cc );                    //
    cc2420_setpanid( cc, PANID );						//�����ʶ
    cc2420_setshortaddress( cc, LOCAL_ADDRESS );		//���ڱ�ʶ

    hal_enable_interrupts();

    while(1)
    {
        memset( &frame[0], 0x00, sizeof(frame) );

		// the test frame is 22 byte in total including the first length byte. 
		// the value of length byte frame[0] doesn't count itself.

		len = 17;
        frame[0] = len;// length of MPDU

        frame[1] = 0x01;//FCF, low byte comes first. And I thought it should be 0x8801, not 0x8841,
        frame[2] = 0x88;// because when we write into the source PANID, THE INTRA_PAN BIT should be 0.

		frame[3] = 0x01;//sequence.

        frame[4] = 0x01;//destination PANID, also low byte comes first.
        frame[5] = 0x00;

        frame[6] = 0x01;//destination address, also low byte comes first.
        frame[7] = 0x00;

        frame[8] = 0x01;//source PANID, also low byte comes first.
		frame[9] = 0x00;

        frame[10] = 0x00;//sourse address, also low byte comes first.
        frame[11] = 0x00;

        frame[12] = 0x11;//data
        frame[13] = 0x12;
        frame[14] = 0x13;
        frame[15] = 0x14;
        frame[16] = 0x15;//the last two bytes are automatically change to CRC.
        frame[17] = 0x16;

        //for(j=3; j<19; j++)
        //{
        //    frame[j] =j;
        //}
        //frame[19] = 19;
        //frame[20] = 20;

        //frame[len] =21;

		#ifndef GDEBUG
        while (1)
        {
			len = cc2420_write( cc, (char*)(&frame[0]), len, 0x00 );
			if (len > 0)
			{
                uart_putchar( uart, '>' );
				// now len means the bytes actually wrotten to cc2420, but it does not mean 
				// the frame is actualy sent out.
				uart_putchar( uart, len );
                led_toggle( LED_RED );
                break;
            }

			// add a little delay here, so that we can catch up with the switching of led.
            hal_delay(100);
        }
		#endif

		#ifdef GDEBUG
		len = cc2420_write( cc, (char*)(&frame[0]), len, 0x00 );
        status = cc2420_sendcmd( cc, CC2420_SNOP);
        uart_putchar( uart, status );//if this output 0x48, it means the transmit is operating
		uart_putchar( uart,len );
        //while (!(PIND & (1<<4))) {uart_putchar(uart, 0x11);};//if  this output 0x11, it means SFD is low now.
        //while ((PIND & (1<<4))) {uart_putchar(uart, 0x22);}; //if  this output 0x22, it means SFD is high now.
        //while (!(PIND & (1<<4))) {led_on(LED_GREEN);};
        //while (PIND & (1<<4)) {led_off(LED_GREEN);};
        led_on(LED_RED); hal_delay(300); //when the red led triggle, it means SFD is low again and the sending is over.
		led_off(LED_RED); hal_delay(300);
		#endif

        hal_delay(1000);
    }
}
*/
