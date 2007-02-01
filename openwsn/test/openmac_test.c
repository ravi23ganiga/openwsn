
#include "..\hal\hal_foundation.h"
#include "..\hal\hal_uart.h"
#include "..\foundation.h"
#include "..\global.h"
#include "openmac_test.h"

/* the testing of OpenMAC assumes you have 3 node and a host computer. they 
 * three communicate each other at the same. the host program will send packet
 * and try to receive it. the host program will compare the sending and received
 * together so that to judge whether there are data collision during the test.
 * 
 * Host <===> Node A <--------> Node B
 *             
 *            Node C 
 * 
 * Node C is a whitehole. it will send frames to Node B occasionally based on a 
 * timer driven. and Node A (the sink node) will communicate with Node B at the 
 * same time. Node A will accept frames from host and sends the frames received
 * to host to. while, Node B will simply forward the frame received back to its
 * sender, for example, the frame from C is return back C, the frame from A is 
 * return back to A.
 * 
 * Node C is the disturbence of A to B's communication.
 */ 

/* the value of NODETYPE can be 0,1,2
 * it is related to address
 * for node A, B, C, the address is 0x01, 0x02, 0x03 respectively.
 */
#define CONFIG_NODETYPE 0
#define CONFIG_ADDRESS CONFIG_NODETYPE

#define UART_BUF_SIZE 256

void openmac_run( void )
{
	char txbuf[OPENMAC_BUFFER_SIZE];
	char rxbuf[OPENMAC_BUFFER_SIZE];
	uint8 txlen, rxlen, count;
	uint16 nodefrom, nodeto;
	TOpenMAC m_mac;
	TOpenMAC * g_mac;
	char * buf;

	target_init();
	global_construct();
	uart_configure( g_uart, 115200, 0, 0, 0, 0 );
	timer_init( g_timer1, 1, 2 );
	g_mac = mac_construct( (char*)(&m_mac), sizeof(m_mac) );
	mac_init( g_mac, g_cc2420, g_timer1 );
	
	memset( (char*)(&txbuf[0]), 0x00, OPENMAC_BUFFER_SIZE ); 
	memset( (char*)(&rxbuf[0]), 0x00, OPENMAC_BUFFER_SIZE ); 
	txlen = 0;
	rxlen = 0;
	
	nodefrom = CONFIG_ADDRESS;
	nodeto = (nodefrom+1) % 3;
	
	// sink node
	#if (CONFIG_NODETYPE == 0)
	while (1)
	{
		buf = (char*)(rxbuf[0]);
		if (rxlen == 0)
		{
			rxlen = mac_rawread( g_mac, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
		if (rxlen > 0)
		{
			if (uart_write( g_uart, buf, rxlen, 0x00 ) > 0)
				rxlen = 0;
		}

		buf = (char*)(txbuf[0]);
		if (txlen == 0)
		{
			txlen = uart_read( g_uart, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
		if (txlen > 0)
		{
			if (mac_rawwrite( g_mac, buf, txlen, 0x00 ) > 0)
				txlen = 0;
		}
	}
	#endif

	#if (CONFIG_NODETYPE == 1)
	txlen = 0;
	rxlen = 0;
	while (1)
	{
		if (rxlen == 0)
		{
			rxlen = mac_rawread( g_mac, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
		
		if ((rxlen > 0) && (txlen == 0))
		{
			nodefrom = opf_nodefrom( rxbuf );
			nodeto = opf_nodeto( rxbuf );
			memmove( txbuf, rxbuf, OPENMAC_BUFFER_SIZE );
			opf_setnodefrom( txbuf, nodefrom );
			opf_setnodeto( txbuf, nodeto );
			txlen = rxlen;
		}
		
		if (txlen > 0)
		{
			if (mac_rawwrite( g_mac, buf, rxlen, 0x00 ) > 0)
				rxlen = 0;
		}
	}
	#endif
	
	
	#if (CONFIG_NODETYPE == 2)
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
			mac_rawwrite( g_mac, buf, OPENMAC_BUFFER_SIZE, 0x00 );
		}
	}
	#endif
		
	mac_destroy( g_mac );
	global_destroy();	
	return;
}


