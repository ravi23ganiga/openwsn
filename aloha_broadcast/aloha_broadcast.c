/*
*****************************************************************************
 * @ authored by Shimiaojing
 * this module  mainly test for flood test.
 ******************************************************************************/

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


#ifdef CONFIG_DEBUG   
    #define GDEBUG
#endif

#define BROADCAST_PAN       0xFFFF
#define LOCAL_ADDRESS		0x00
#define BROADCAST_ADDRESS   0xFFFF
#define CHANNEL            	11
#define PANID               0x0001 //??  whether panto can  be arbituary 0xffff?? 

static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
static TiUartAdapter		m_uart;
static TiTimerAdapter       m_timer;

uint16  len;

void aloha_broadcastnode(void);

int main(void)
{
	aloha_broadcastnode();
}

void aloha_broadcastnode(void)
{   
    TiCc2420Adapter * cc;
    TiAloha * mac;
	TiUartAdapter * uart;
	TiTimerAdapter   *timer;
	
    char opfmem[OPF_SUGGEST_SIZE];
	TiOpenFrame * opf;

	char * msg = "welcome to aloha sendnode...";
	uint8 i, total_length, seqid=0, option;
    uint16 fcf;

	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	led_on( LED_RED );
	dbo_open( 0, 38400 );

	cc = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	uart = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
    timer= timer_construct(( char *)(&m_timer),sizeof(TiTimerAdapter));
    	

	uart_open( uart, 0, 38400, 8, 1, 0x00 );
	uart_write( uart, msg, strlen(msg), 0x00 );
	cc2420_open(cc, 0, NULL, NULL, 0x00 );
	dbo_putchar(2);

	aloha_open( mac,cc,CHANNEL,PANID,LOCAL_ADDRESS,timer, NULL, NULL,0x01);

    opf = opf_open( (void *)(&opfmem[0]), sizeof(opfmem), OPF_DEF_FRAMECONTROL_DATA_NOACK, OPF_DEF_OPTION );
	cc2420_enable_autoack( cc );

	hal_enable_interrupts();

	while(1) 
	{
        fcf = 0x8801;   
		total_length = 50;					   // equal to frame length, which is the length 
		                                       // of PSDU plus 1.
        
		opf_cast( opf, total_length, fcf );

        opf_set_sequence( opf, seqid ++ );
		opf_set_panto( opf, BROADCAST_PAN );// be careful panto is concrete however panfrom is broadcast_pan
		opf_set_shortaddrto( opf, BROADCAST_ADDRESS );
		opf_set_panfrom( opf, PANID);
		opf_set_shortaddrfrom( opf, LOCAL_ADDRESS );

           

		for (i=0; i<opf->msdu_len; i++)
			opf->msdu[i] = i; // msdu is just is signal nothing with the  data frame 

	
		option = 0x01;//NOACK ,since brocadcast does not need to verify ack.


        while (1)
        {
		 
            if (aloha_broadcast(mac, opf, option) > 0)
            {
			
                led_toggle( LED_YELLOW );
                dbo_putchar( 0x11);
                dbo_putchar( seqid );
                break;
            }
			else{
				dbo_putchar(0x22);
                dbo_putchar( seqid );
			}
            hal_delay(1000);
        }
		
        // controls the sending rate. if you want to test the RXFIFO overflow processing
        // you can decrease this value. 
		hal_delay(1000);

		//break;
	}
}
