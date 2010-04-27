
#include "..\foundation.h"
#include "..\service\svc.h"
#include "..\global.h"
#include "sinknode.h"

/****************************************************************************** 
 * sinknode
 * this is the gateway node between wireless sensor networks and computer networks.
 * it will send the frames received through UART by wireless and send the one received 
 * through wireless by UART.
 *
 * @author zhangwei on 20090523
 * 	- first created
 * @modified by xxx on 20090524
 *	- revision
 *****************************************************************************/

static TOpenMAC				m_mac;
static TOpenNET				m_net;
static TSensorService		m_sensors;
static TLocationService		m_lcs;

static TOpenFrame			g_txframe;
static TOpenFrame			g_rxframe;
static char *				g_txframebuf = NULL;
static char *				g_rxframebuf = NULL;
static char *				g_txpktbuf = NULL;
static char *				g_rxpktbuf = NULL;


// the first WORD is the PAN id, the second WORD is the node id
// they two forms a standard 802.15.4 short address.
// you can freely changing this setting according to your own requirements.
//
static uint16 LOCAL_ADDRESS[2] = {0x2420, 0x5678};
static void sinknode_execute( void ); 
static int8 sinknode_interpret( TOpenFrame * rxframe, TOpenFrame * txframe );



void sinknode_test( void )
{
	sinknode_execute();	
	return;
}

// for sink nodes in sensor network, the main program is as the following.
// different to generic node in the network, it will forward the packet received
// to host computer through the UART/SIO hardware. while, it will also send 
// the packet received from the host to others nodes in the network.
//
void sinknode_execute( void )
{
	uint8 txlen=0, rxlen=0, count = 0,n;
	
	
	target_init();
	global_construct();


	spi_configure( g_spi );
	uart_configure( g_uart, CONFIG_UART_BAUDRATE, 0, 0, 0, 0 );
	
	g_mac = mac_construct( (char*)(&m_mac), sizeof(m_mac) );
	g_net = net_construct( (char*)(&m_net), sizeof(m_net) );
	g_sensors = sen_construct( (char*)(&m_sensors), sizeof(m_sensors) );
	g_lcs = lcs_construct( (char*)(&m_lcs), sizeof(m_lcs) );
	
	
	//mac_init( g_mac, g_cc2420, g_timer1 );
	mac_configure( g_mac, MAC_BASIC_INIT, 0);
	net_init( g_net, g_mac, NULL );  

	opf_init( g_txframebuf, sizeof(g_txframe) );
	opf_init( g_rxframebuf, sizeof(g_rxframe) );


	g_txframebuf = (char *)(&g_txframe);
	g_rxframebuf = (char *)(&g_rxframe);
	g_txpktbuf = opf_packet( g_txframebuf );
	g_rxpktbuf = opf_packet( g_rxframebuf );

	opf_setpanid( g_txframebuf, 0x2420);
	opf_setpanid( g_txframebuf, 0x2420);
	net_setlocaladdress( g_net, LOCAL_ADDRESS[0], LOCAL_ADDRESS[1] );
	opf_setaddrfrom(g_txframebuf,0x5678);
        opf_setaddrto(g_txframebuf,0x1234);
	//net_configure
	//net_setaddress


        cc2420_receive_on(g_cc2420);  
        IRQEnable(); 


        g_txframebuf[0] = 11 + 10;
		
	txlen = g_txframebuf[0];	
	for (n = 0; n < 10; n++) {
            g_txframebuf[n+10] = 0x77;
	}
	timer_start(g_timer1);
	while (1)
	{
		
		
		// try to read data from the UART/SIO connected to the host.
		// if data received, then try to send them through net_rawwrite()
		if (txlen == 0)
		{
			
			count = uart_read( g_uart, g_txframebuf + txlen, sizeof(TOpenFrame) - txlen, 0x00 );
			txlen += count;
		}
		
		if (opf_length(g_txframebuf) > 0)
		{
                        //led_twinkle(LED_GREEN,2);
			count = net_rawwrite( g_net, g_txframebuf, g_txframebuf[0], 0x00 );
                        	
		}
		
		
		// try to read data from the network. if data received, then try to send 
		// them to the host through the UART/SIO.
		
	
		
		if (rxlen == 0)
		{

			count = net_rawread( g_net, g_rxframebuf, 128, 0x00 );

			if (count > 0)
			{
				uart_write( g_uart, g_rxframebuf, rxlen, 0x00 );
			        rxlen = 0;
			}
		}
		
		if ((rxlen > 0) && (rxlen <= opf_length(g_rxframebuf)))
		{ 

			count = opf_length(g_rxframebuf) - rxlen;
			count = uart_write( g_uart, g_rxframebuf + count, rxlen, 0x00 );

			if (count > 0)
			{
				if (rxlen <= count)
					rxlen = 0;
				else 
					rxlen -= count;
			}
		}
		
		//debug_evolve( g_debugio );
		net_evolve( g_net );
		led_twinkle(LED_GREEN,1);
	}
		
	global_destroy();	
	
	return;
}


// this function will interpret the frame/packet received in txframebuf, and 
// put the processing reply into rxframebuf
//
int8 sinknode_interpret( TOpenFrame * rxframe, TOpenFrame * txframe )
{
	char *txdata, *rxdata;
	int8 ret=0;

	if ((rxframe->length == 0) || (txframe->length > 0))
	{
		return -1;
	}

	rxdata = opt_data(opf_packet((char*)rxframe)); 
	txdata = opt_data(opf_packet((char*)txframe)); 

	// these request are usually sent by the sink node. and the sink node hope the 
	// sensor nodes can return some reply information.
	// rxdata[0] is the data type
	switch (rxdata[0])
	{
	// if the received packet is a LOCATION request, then fill the "txframe" with 
	// location information and send it back. usually, the sink node will flooding 
	// a LOCATION request and expect to receive the location information of all the 
	// node. this is the APPLICATION layer mechanism. don't mix it with the "TLocationService"
	// while, the "TLocationService" will send/received frames based on MAC layer 
	// interface. 
	//
	case ODA_TYPE_LOCATION_REQUEST:
		//lcs_evolve( g_lcs, &m_lcs, g_cc2420 );
		txdata[0] = ODA_TYPE_LOCATION;
		memmove( txdata + 1, (char*)(&m_lcs), sizeof(m_lcs) );
		txframe->length = OPF_HEADER_SIZE + 1 + sizeof(m_lcs);
		break;

	case ODA_TYPE_TEMPSENSOR_REQUEST:
	 	sen_fillframe( g_sensors, ODA_TYPE_TEMPSENSOR, txframe, OPF_FRAME_SIZE );
	 	break;

	case ODA_TYPE_VIBSENSOR_REQUEST:
	 	sen_fillframe( g_sensors, ODA_TYPE_VIBSENSOR, txframe, OPF_FRAME_SIZE );
	 	break;

	case ODA_TYPE_LIGHTSENSOR_REQUEST:
	 	sen_fillframe( g_sensors, ODA_TYPE_LIGHTSENSOR, txframe, OPF_FRAME_SIZE );
	 	break;
	}

	return ret;
}

