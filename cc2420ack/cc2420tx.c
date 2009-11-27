/*****************************************************************************
 * this test demostrate how to send a frame using TCc2420 adapter object
 *
 * @author HuangHuan in 2005.12
 * @modified by MaKun on 2007-04-18
 *	- test sending/TX function of cc2420 driver
 *
 * @modified by zhangwei on 20070418
 *	- just modify the format of the source file and including files
 * 
 * @modified by zhangwei on 20070701
 *	- port to OpenNode-3.0 hardware
 *	- and speed the transimisson rate. 
 * @modified by zhangwei in 2009.07
 *	- ported to ICT GAINZ platform (based on atmega128)
 * @modified by Yan-Shixing in 2009.07
 *	- correct bugs and tested. now the two send functions sendnode1() and sendnode2()
 *    are all work success.

 ****************************************************************************/ 

#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_cc2420.h"
#include "../common/hal/hal_target.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_debugio.h"

#ifdef CONFIG_DEBUG
    #define GDEBUG
#endif

#define TEST_ACK_REQUEST

#define PANID				0x0001
#define LOCAL_ADDRESS		0x01  
#define REMOTE_ADDRESS		0x02
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

static TiCc2420Adapter		g_cc;
static TiUartAdapter		g_uart;

void sendnode(void);


int main(void)
{
	sendnode();
}

void sendnode(void)
{
    char opfmem[OPF_SUGGEST_SIZE];
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	TiOpenFrame * opf;

	char * buf;

	char * msg = "welcome to sendnode...";
	uint8 i, total_length, seqid=0, option, len;
    uint16 fcf;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );
	dbo_open(0, 38400);

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

    opf = opf_construct( (void *)(&opfmem), sizeof(opfmem) );
	#ifdef TEST_ACK_REQUEST
    opf_open( opf, OPF_DEF_FRAMECONTROL_DATA_ACK, OPF_DEF_OPTION );
	#else
    opf_open( opf, OPF_DEF_FRAMECONTROL_DATA_NOACK, OPF_DEF_OPTION );
	#endif

	cc2420_open( cc, 0, NULL, NULL, 0x00 );
	//cc2420_setchannel( cc, DEFAULT_CHANNEL );
	//cc2420_setrxmode( cc );							//Enable RX
	cc2420_enable_addrdecode( cc );					//使能地址译码
	cc2420_setpanid( cc, PANID );					//网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	//网内标识

	#ifdef TEST_ACK_REQUEST
	cc2420_enable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_ACK;             // 0x8821;    
	#else
	cc2420_disable_autoack( cc );
    fcf = OPF_DEF_FRAMECONTROL_DATA_NOACK;			// 0x8801;  
	#endif

	hal_enable_interrupts();

	while(1)  
	{
		total_length = 30;					     
		opf_cast( opf, total_length, fcf );

        opf_set_sequence( opf, seqid ++ );
		opf_set_panto( opf, PANID );
		opf_set_shortaddrto( opf, REMOTE_ADDRESS );
		opf_set_panfrom( opf, PANID );
		opf_set_shortaddrfrom( opf, LOCAL_ADDRESS );

		/* datalen should be opf->msdu_len */
		//for (i=0; i<opf->datalen; i++)
		for (i=0; i<opf->msdu_len; i++)
			opf->msdu[i] = i+5;

		
		

		for (i=0;i<opf->msdu_len; i++)
		{
			dbo_putchar(opf->msdu[i]);
		}
		
		dbo_putchar(0x11);

		#ifdef TEST_ACK_REQUEST
		option = 0x01;
		#else
		option = 0x00;
		#endif

		//dbo_putchar( '>' );
		while (1)
        {
            if ((len=cc2420_write(cc, (char*)(opf_buffer(opf)), opf_datalen(opf), option)) > 0)
            {
				led_off( LED_RED );
				hal_delay( 500 );
				led_on( LED_RED );
				hal_delay( 500 );              
				buf=opf_buffer(opf);
				for(int i=0; i<=opf_datalen(opf); i++)
				{
					dbo_putchar(buf[i]);
				}
	 			// uart_putchar( &g_uart, len );
                // uart_putchar( &g_uart, seqid );
				//dbo_n8toa( len );
				//dbo_n8toa( opf_sequence(opf) );
                break;
            }
            hal_delay(100);
        }
		
		cc2420_evolve( cc );

        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
		hal_delay( 500 );
	}
}
