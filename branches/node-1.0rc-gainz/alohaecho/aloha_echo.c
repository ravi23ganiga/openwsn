/******************************************************************************
 * aloha_echo
 * The receiving test program based on ALOHA medium access control. It will try 
 * to receive the frames to itself, and then sent a character to the computer 
 * through UART as a indication. 
 *
 * @state
 *	still in developing. test needed
 *
 * @author Shi-Miaojing on 20090802
 *	- first created
 * @modified by zhangwei on 20090802
 *	- revisioin. compile passed.
 * @tested by ShiMiaojing on 20090804 
 * @ modified by Shimiaojing on 20091103/04 test ok works well.
 * about frame structure : source addr & destination may be defined different with 
 * cc2420 protocal. 
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
#include "../common/svc/svc_aloha.h"

#define CONFIG_TEST_ADDRESSRECOGNITION
//#define CONFIG_TEST_ACK

#define LOCAL_ADDRESS		        0x02
#define REMOTE_ADDRESS		        0x01

static  TiCc2420Adapter             g_cc;
static  TiUartAdapter	            g_uart;
static  TiAloha						g_aloha;
static  char                        g_rxbufmem[OPF_SUGGEST_SIZE];
static  TiTimerAdapter              g_timer;

static void echonode(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

int main(void)
{
	echonode();
}

void echonode(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
    TiAloha * mac;
	TiOpenFrame * opf;
	TiTimerAdapter   *timer;
	char * msg = "welcome to aloha_echo-RX";
	uint8 len;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
	dbo_open( 0, 38400 );
    dbo_putchar('1');
	led_open();
	led_on( LED_RED );
	hal_delay( 1000 );
	led_off( LED_ALL );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );
    mac = aloha_construct( (char *)(&g_aloha), sizeof(TiAloha) );
    timer= timer_construct(( char *)(&g_timer),sizeof(TiTimerAdapter));
    
	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
    cc2420_open(cc, 0, NULL, NULL, 0x00 );
	mac = aloha_open( mac, cc, CONFIG_ALOHA_DEFAULT_CHANNEL, CONFIG_ALOHA_DEFAULT_PANID, LOCAL_ADDRESS, timer,NULL, NULL,0x00);
    dbo_putchar('3');
	
    #ifdef CONFIG_TEST_ADDRESSRECOGNITION
    cc2420_enable_addrdecode( cc );					//使能地址译码
	#endif
	
	#ifdef CONFIG_TEST_ACK
	cc2420_enable_autoack( cc );
	#endif

    opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
    opf = opf_open( (void *)(&g_rxbufmem), sizeof(g_rxbufmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
	hal_enable_interrupts();
	
    //led_toggle( LED_RED );
	while(1)
	{
		//aloha_evolve( mac, NULL );

		len = aloha_recv( mac, opf, 0x00 );
		if (len > 0)
		{    
			dbo_putchar( 'R' );
			_output_openframe(opf,uart);
			opf_swapaddress( opf );
			//hal_assert( opf_type(opf) == FCF_FRAMETYPE_DATA );
		
            // todo 200911
            // Shimiaojing: 如下两种情况：要求ACK和不要求ACK都要测试通过才行
			//while (aloha_send(mac,opf,0x01)==0){}
			while (aloha_send(mac,opf,0x00)==0){}
			dbo_putchar( 'S' );
			led_off( LED_RED );
            
			/* warning: You shouldn't wait too long in the while loop, or else 
			 * you may encounter cc2420 RXFIFO overflow and frame loss. However, 
			 * the program should still work properly even the delay time is an 
			 * arbitrary value. No error are allowed in this case. 
			 */
			hal_delay( 500 );
			led_on( LED_RED );
			hal_delay( 500 );
			//break;
			}
		
	}		
}	 

void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	if (opf->datalen > 0)
	{
		dbo_putchar( '>' );
	 	dbo_n8toa( opf->datalen );

		if (!opf_parse(opf, 0))
		{
	        dbo_n8toa( *opf->sequence );
			dbo_putchar( ':' );
			dbo_write( (char*)&(opf->buf[0]), opf->buf[0] );
		}
		else{
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			dbo_write( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( '\n' );
	}
}
