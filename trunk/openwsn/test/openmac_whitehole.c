
#include "..\hal\hal_foundation.h"
#include "..\hal\hal_uart.h"
#include "..\hal\hal_timer.h"
#include "..\hal\hal_global.h"
#include "..\foundation.h"
#include "..\global.h"
#include "openmac_whitehole.h"

#define OPENMAC_BUFFER_SIZE 0x7E

static TOpenMAC m_mac;
static TOpenMAC * g_mac;

void openmac_whitehole()
{
	char txbuf[OPENMAC_BUFFER_SIZE];
	char rxbuf[OPENMAC_BUFFER_SIZE];
	char * buf;

	target_init();
	global_construct();
	g_mac = mac_construct( &m_mac, sizeof(m_mac) );
	mac_init( g_mac, g_cc2420, NULL, g_timer1 );
	timer_init( g_timer1, 1, 2 );

	// 在这里初始化一个OpenFrame
	
	memset( (char*)(&txbuf[0]), 0x00, OPENMAC_BUFFER_SIZE ); 
	memset( (char*)(&rxbuf[0]), 0x00, OPENMAC_BUFFER_SIZE ); 
	txlen = 0;
	rxlen = 0;
	
	// you can change the node address here according to your own testing
	// environment.
	//
	buf = (char*)(txbuf[0]);
	opf_setaddrfrom( txbuf, 3 );
	opf_setaddrto( txbuf, 1 );
	
	while (1)
	{
		timer_stop( g_timer1 );
		timer_setinterval( g_timer1, 500, 0 );
		timer_start( g_timer1 );
		
		while (!timer_expired(g_timer1))
		{
			mac_write( g_mac, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
	}
		
	mac_destroy( g_mac );
	global_destroy();	
	return;
}
