/******************************************************************************
 * aloha_recv
 * The receiving test program based on ALOHA medium access control. It will try 
 * to receive the frames to itself, and then sent a character to the computer 
 * through UART as a indication. 
 *
 * @state
 *	still in developing. test needed
 *
 * @author Shi-Miaojing on 20090801
 *	- first created
 * @modified by  ShiMiaojing but not tested   
 *  - some wrong with assert.h  so  interupt is wrong.
 * @modified by zhangwei on 20090804
 *	- revisioin. compile passed.
 *modified  by ShMiaojing test ok
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

//#define CONFIG_TEST_LISTENER  
#define CONFIG_TEST_ADDRESSRECOGNITION
#define CONFIG_TEST_ACK

//#define PANID						0x0001
//#define LOCAL_ADDRESS				0x01
//#define REMOTE_ADDRESS				0x00
//#define DEFAULT_CHANNEL				11
//#define BROADCAST_ADDRESS			0xFFFF

static  TiCc2420Adapter             m_cc;
static  TiUartAdapter	            m_uart;
static  TiAloha						m_aloha;
static  char                        m_rxbufmem[OPF_SUGGEST_SIZE];
static TiTimerAdapter               m_timer;
uint8   chn=11;
uint16  panid=0x0001; 
uint16  address=0x01;


#ifdef CONFIG_TEST_LISTENER
static void _aloha_listener( void * ccptr, TiEvent * e );
#endif

static void recvnode(void);
static void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

int main(void)
{
	recvnode();
}

void recvnode(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
    TiAloha * mac;
	TiOpenFrame * opf;
	TiTimerAdapter   *timer;
	char * msg = "welcome to aloha recv test...";
    #ifndef CONFIG_TEST_LISTENER
    uint8 len;
    #endif

	target_init();
	OS_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_RED );
	//hal_delay( 1000 );
	//led_off( LED_ALL );
	dbo_open( 0, 38400 );

	cc = cc2420_construct( (void *)(&m_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
    mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );

    opf = opf_construct( (void *)(&m_rxbufmem), sizeof(m_rxbufmem) );
    opf_open( opf, 0x00 );

	#ifdef CONFIG_TEST_LISTENER
	mac = aloha_open( mac, cc,chn,panid,address,timer, _aloha_listener, NULL,0x00 );
	#else
	mac = aloha_open( mac, cc,chn,panid,address,timer,NULL, NULL,0x00 );
	#endif
    
	//aloha_setchannel( mac, DEFAULT_CHANNEL );
	//aloha_setpanid( mac, PANID );				 //网络标识, seems no use in sniffer mode
	//aloha_setlocaladdress( mac, LOCAL_ADDRESS );	 //网内标识, seems no use in sniffer mode
	
	#ifdef CONFIG_TEST_ADDRESSRECOGNITION
	cc2420_enable_addrdecode( cc );
	#else
	cc2420_disable_addrdecode( cc );
	#endif

	#ifdef CONFIG_TEST_ACK
	cc2420_enable_autoack( cc );
	#endif

	opf = opf_construct( (void *)(&m_rxbufmem), sizeof(m_rxbufmem) );
    opf_open( opf, 0x00 );

    hal_enable_interrupts();

	/* Wait for listener action. The listener function will be called by the TiCc2420Adapter
	 * object when a frame arrives */
	#ifdef CONFIG_TEST_LISTENER	
	while (1) {}
	#endif
    
	/* Query the TiCc2420Adapter object if there's no listener */
	#ifndef CONFIG_TEST_LISTENER
	while(1) 
	{
       	len = aloha_recv( mac, opf, 0x00 );
		if (len > 0)
		{   
			//dbo_putchar(0x88);
			_output_openframe( opf, uart );
			led_off( LED_RED );

			/* warning: You shouldn't wait too long in the while loop, or else 
			 * you may encounter frame loss. However, the program should still 
			 * work properly even the delay time is an arbitrary value. No error 
			 * are allowed in this case. 
			 */
			hal_delay( 500 );
			led_on( LED_RED );
			hal_delay( 500 );
        }

		//aloha_evolve(mac,NULL );
	}
	#endif
}


#ifdef CONFIG_TEST_LISTENER
void _aloha_listener( void * owner, TiEvent * e )
{
	TiAloha * mac = &m_aloha;
    TiOpenFrame * opf = (TiOpenFrame *)m_rxbufmem;
    
	uart_putchar( &m_uart, 0x77 );
	led_toggle( LED_RED );
	while (1)
	{
       	len = aloha_recv( mac, opf, 0x00 );
		if (len > 0)
		{   
			_output_openframe( opf, uart );
			led_toggle( LED_RED );

			/* warning: You cannot wait too long in the listener. Because in the 
			 * current version, the listener is still run in interrupt mode. 
			 * you may encounter unexpect error at the application error in real-time
			 * systems. However, the program should still work properly even the 
			 * delay time is an arbitrary value here. No error are allowed in this case. 
			 *
			 * => That's why we cannot use hal_delay() to control the LED to make
			 * it observable for human eye. 
			 */
			// hal_delay( 500 );
			break;
        }
	}
}
#endif

void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	uart_putchar( uart, 0x66 );
	uart_putchar( uart, opf->len );
	uart_putchar( uart, 0x77);
      dbo_open(0,38400);


	if (opf->len > 0)
	{
		if (!opf_parse(opf))
		{
			uart_putchar( uart, 0x55 );
            dbo_putchar (*opf->sequence);

			//uart_write( uart, (char*)&(opf->buf[0]), opf->buf[0], 0x01 );
		}
		else
			//uart_write( uart, (char*)&(opf->buf[0]), opf->len, 0x01 );
		uart_putchar( &m_uart, '\n' );
		dbo_putchar (*opf->sequence);
	}
}


