/******************************************************************************
 * floodsender.c
 * command sender node in the network
 *
 * Periodically sending LED on/off commands to control all the LEDs in the network.
 * 
 * @author zhangwei on 20061106
 *	- first created
 * @modified by zhangwei on 20090801
 *	- revision. compile passed.
 * @modified by Shi-Miaojing and Yan-Shixing (TongJi University) on 20090802
 *	- testing...
 * @modified by ShiMiaojing on 20091101 uncompiled need to modified svc_flood.c
 * test ok on November 17,2009  good performance.
 *****************************************************************************/

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
#include "../common/svc/svc_aloha.h"
#include "../common/svc/svc_flood.h"

#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define PANID				0x0001
#define BROADCAST_PAN       0xffff
#define LOCAL_ADDRESS		0x0009   
#define BROADCAST_ADDRESS	0xffff
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
static TiUartAdapter		m_uart;
static TiFloodNetwork       m_net;
static TiTimerAdapter       m_timer;

uint16  len,seqid=0;

static void ledflood( uint16 ontime, uint16 offtime );

int main(void)
{
	// all the led will be turned on for 500 ms and off for 5000 ms 
	ledflood( 500, 3000 );
}

void ledflood( uint16 ontime, uint16 offtime )
{
    TiCc2420Adapter * cc;
    TiAloha * mac;
	TiUartAdapter * uart;
	TiFloodNetwork * net;
	TiOpenFrame * opf;
    TiTimerAdapter * timer;
    char opfmem[OPF_SUGGEST_SIZE];
	char * msg = "welcome to floodsender";
	uint8 total_length,seqid=0,i;
    uint16 fcf;
	int k;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED);
	led_on(LED_YELLOW);

	cc = cc2420_construct( (char *)&m_cc, sizeof(TiCc2420Adapter) );
	mac = aloha_construct( (char *)&m_aloha, sizeof(TiAloha) );
	uart = uart_construct( (void *)&m_uart, sizeof(TiUartAdapter) );
	net = flood_construct( (void*)&m_net, sizeof(TiFloodNetwork) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
    
	cc2420_open(cc, 0, NULL, NULL, 0x00 );
    cc2420_settxpower( cc, CC2420_POWER_3);
	//  to set the power of CC2420 in order to adjust our test.3 is suitable.

	aloha_open( mac,cc,DEFAULT_CHANNEL,PANID,LOCAL_ADDRESS,timer, NULL, NULL,0x01);


	flood_open( net, mac, NULL, NULL, PANID, LOCAL_ADDRESS );

	hal_enable_interrupts();


	fcf = OPF_DEF_FRAMECONTROL_DATA_ACK;        // = 0x8821;
	total_length = 30;                            // equal to frame length + 1 due to the first byte 
		                                          // in the buffer is the length byte. 
    opf = opf_open( (void *)(&opfmem[0]), sizeof(opfmem), OPF_DEF_FRAMECONTROL_DATA, OPF_DEF_OPTION );

	opf_cast( opf, total_length, fcf );           // cast the TiOpenFrame object as 
		                                          // our expected frame type.

   opf_set_panto( opf, BROADCAST_PAN );           // be careful panto is concrete however panfrom is broadcast_pan
   opf_set_shortaddrto( opf, BROADCAST_ADDRESS );
   opf_set_panfrom( opf, PANID);
   opf_set_shortaddrfrom( opf, LOCAL_ADDRESS );
   
   opf->msdu[2] =0x06;                             // 0 means turn off and nonzeros means turn on
   opf_framelength(opf);
	while(1) 
	{
		opf->msdu[2] = !(opf->msdu[2]);
		opf->msdu[3] = 0x99;                     //data an be  arbituary set
		for(k=4;k<20;k++)
		opf->msdu[k]=k;
		 opf_set_sequence( opf, seqid ++ );
        while (1)
        {
            if (flood_broadcast(net, opf, 0x01) > 0)
            {   
			 dbo_putchar(opf->buf[0]);
                led_toggle( LED_RED );
                break;
            }
            hal_delay(100);                       // delay some time before retry
        }
		
		aloha_evolve( mac, NULL );
		flood_evolve( net, NULL );

		if (opf->msdu[1])
			hal_delay( ontime );
		else
			hal_delay( offtime );
	}
}
