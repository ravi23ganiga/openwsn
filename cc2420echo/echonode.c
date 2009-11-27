
#include "../common/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h> 
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_cc2420.h"
#include "../common/hal/hal_debugio.h"
#include "echonode.h"

#define PANID				0x0001
#define LOCAL_ADDRESS		0x02
#define REMOTE_ADDRESS		0x01
#define BUF_SIZE			128
#define DEFAULT_CHANNEL     11

TiCc2420Adapter             g_cc;
TiUartAdapter	            g_uart;

static void _change(uint8 x,uint8 y);

void echonode(void)
{
    TiCc2420Adapter * cc;
	TiUartAdapter * uart;
	char * msg = "welcome to echonode...";
	uint8 buf[BUF_SIZE];
	uint8 len;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_off( LED_ALL );
	hal_delay( 500 );
	led_on( LED_RED );

	cc = cc2420_construct( (void *)(&g_cc), sizeof(TiCc2420Adapter) );
	uart = uart_construct( (void *)(&g_uart), sizeof(TiUartAdapter) );

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	cc2420_open( cc, 0, NULL, NULL, 0x00 );

	cc2420_setchannel( cc, DEFAULT_CHANNEL );
	cc2420_setrxmode( cc );							// Enable RX
	cc2420_enable_addrdecode( cc );					// 使能地址译码
	cc2420_setpanid( cc, PANID );					// 网络标识
	cc2420_setshortaddress( cc, LOCAL_ADDRESS );	// 网内标识

	hal_enable_interrupts();
 
	while(1) 
	{
		cc2420_evolve( cc );
		len = cc2420_read( cc, (char*)(&buf[0]), BUF_SIZE, 0x00 );
		if (len > 6)
		{
			uart_putchar( uart, '>' );

			// switch source address and destination address
			_change(buf[3], buf[7]);
			_change(buf[4], buf[8]);
			_change(buf[5], buf[9]);
			_change(buf[6], buf[10]);
	  
			buf[29] = 56; // simulate data read from sensor
			cc2420_write( cc, (char*)(&buf[0]), len, 0x00 );
			hal_delay( 10 );
		
			// @attention
			// You needn't call cc2420_setrxmode() here, because the hal_cc2420 
			// module will handle it inside. 
			// cc2420_setrxmode( cc ); 
		}
	}
}

void _change(uint8 x,uint8 y)
{
	char t;
	t=x;
	x=y;
	y=t;
}

