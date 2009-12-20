/*******************************************************************************
 * gateway node (gateway, G nodes)
 * This demonstration/testing program will send frames from the gateway
 * node and then wait for the feedback. 
 * 
 * @author Yan-shixing ,zhangwei(TongJi University) and Shi miaojing in 200911
 *	- first created
 ******************************************************************************/

#include "../common/hal/hal_configall.h"  
#include "../common/svc/svc_configall.h"  
#include "../common/rtl/rtl_foundation.h"
#include "../common/rtl/rtl_iobuf.h"
#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_timer.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_luminance.h"
#include "../common/hal/hal_assert.h"
#include "../common/svc/svc_foundation.h"
#include "../common/svc/svc_aloha.h"
#include "../common/svc/svc_timer.h"
#include "../common/svc/svc_datatree.h"
#include "../common/hal/hal_adc.h"
#include "../common/hal/hal_luminance.h"
#include "../common/svc/svc_datatree.h"
#include "ledtune.h"
#include "output_frame.h"


#define CONFIG_NODE_ADDRESS             0x0001
#define CONFIG_NODE_PANID               0x0001
#define CONFIG_NODE_CHANNEL             11
#define CONFIG_REMOTE_PANID				0x0002
#define CONFIG_REMOTE_ADDR				0x0001

#define GATW_VTM_RESOLUTION 7

#define GATW_PHY_HEADER_LENGTH 1
#define GATW_MAC_HEADER_LENGTH 13

#define GATW_MAX_HOPCOUNT 5
#define GATW_PAYLOAD_LENGTH 2

#define GATW_TOTAL_SIZE (GATW_PHY_HEADER_LENGTH + GATW_MAC_HEADER_LENGTH + DTP_HEADER_SIZE(GATW_MAX_HOPCOUNT) + GATW_PAYLOAD_LENGTH)

#define HIGHBYTE(w) ((uint8)(w>>8))
#define LOWBYTE(w) ((uint8)(w & 0xFF))

static TiTimerAdapter 		m_hwtimer0;
static TiTimerManager 		m_vtm;
static TiCc2420Adapter		m_cc;
static TiAloha              m_aloha;
static TiUartAdapter		m_uart;
static TiDataTreeNetwork    m_dtp;
static char                 m_opfmem[ OPF_SUGGEST_SIZE ];	
static TiAdcAdapter         m_adc;
static TiLumSensor          m_lum;
static TiLedTune            m_ledtune;

void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );


int main(void)
{
	uint8 len, i, count=0;
	char * msg = "welcome to gateway node...";
	char * request, * response, * payload;
	uint16 value;

	TiTimerAdapter * hwtimer0;
	TiTimerManager * vtm;
	TiTimer * vti;
	TiTimer * ledtimer;
	TiCc2420Adapter * cc;
    TiAloha * mac;
	TiUartAdapter * uart;
	TiOpenFrame * rxbuf;
	TiOpenFrame * txbuf;
	TiDataTreeNetwork * dtp;
	TiAdcAdapter * adc;
	TiLumSensor * lum;
	TiLedTune * ledtune;

	target_init();
	wdt_disable();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	dbo_open(0, 38400);

	hwtimer0        = timer_construct( (void *)(&m_hwtimer0), sizeof(m_hwtimer0) );
	vtm             = vtm_construct( (void*)&m_vtm, sizeof(m_vtm) );
	cc              = cc2420_construct( (char *)(&m_cc), sizeof(TiCc2420Adapter) );
	mac             = aloha_construct( (char *)(&m_aloha), sizeof(TiAloha) );
	uart            = uart_construct( (void *)(&m_uart), sizeof(TiUartAdapter) );
	rxbuf           = opf_construct( (void *)(&m_opfmem[0]), sizeof(m_opfmem), OPF_FRAMECONTROL_UNKNOWN, OPF_DEF_OPTION );
	txbuf           = rxbuf;
	adc             = adc_construct( (void *)&m_adc, sizeof(TiAdcAdapter) );
	lum             = lum_construct( (void *)&m_lum, sizeof(TiLumSensor) );

	cc              = cc2420_open(cc, 0, NULL, NULL, 0x00 );
	//cc				= cc2420_settxpower( cc, CC2420_POWER_8);
	mac             = aloha_open( mac, cc, CONFIG_NODE_CHANNEL, CONFIG_NODE_PANID, CONFIG_NODE_ADDRESS,hwtimer0, NULL, NULL,0x01);
	uart            = uart_open( uart, 0, 38400, 8, 1, 0x00 );
	rxbuf           = opf_open( &m_opfmem[0], sizeof(m_opfmem), OPF_FRAMECONTROL_UNKNOWN, 0x00 );

	// configure the time interval as 2 seconds. non-periodical
	// the "vti" timer will automatically stopped when it's expired.
	hwtimer0	   	= timer_open( hwtimer0, 0, vtm_inputevent, vtm, 0x01 ); 
	vtm             = vtm_open( vtm, hwtimer0, GATW_VTM_RESOLUTION );
	vti             = vtm_apply( vtm );
	vti             = vti_open( vti, NULL, NULL );
	dtp             = dtp_construct( (void *)(&m_dtp), sizeof(m_dtp) );
	dtp             = dtp_open( dtp, mac, CONFIG_NODE_ADDRESS, NULL, NULL );
	adc             = adc_open( adc, 0, NULL, NULL, 0 );
	lum             = lum_open( lum, 0, adc );
	//ledtimer        = vtm_apply( vtm );
	//ledtimer        = vti_open( ledtimer, NULL, NULL );
	//ledtune         = ledtune_construct( (void*)(&m_ledtune), sizeof(m_ledtune), ledtimer );
	//ledtune         = ledtune_open( ledtune );
	cc				= cc2420_settxpower( cc, CC2420_POWER_8);

	dbo_write( msg, strlen(msg) );
	hal_enable_interrupts();

	count = 0;
	while(1)
	{
		/* Maintain the data tree structure in the network. This tree is used when 
		 * the sensor nodes send data response back to the root node. 
		 * 
		 * send maintain request every 4 data request.
		 */
		if (count % 4 == 0)
		{
			dtp_maintain( dtp, GATW_MAX_HOPCOUNT );
			hal_delay( 300 );
		}

		// sending DTP_DATA_REQUEST packet and try to receive the DTP_DATA_RESPONSE.
        //
		// assume the frame length is 60 bytes. This length should larger than the
		// total value of PHY length byte(1B), MAC header size and DTP header size. 
		// You can adjust this value to adapt to your own application requirement.
		//
		opf_cast( txbuf, GATW_TOTAL_SIZE, OPF_DEF_FRAMECONTROL_DATA_NOACK );

		// move the luminance sensor value into payload field of the request
        request = opf_msdu( txbuf );
		payload = DTP_PAYLOAD_PTR(request);
		value = lum_value( lum ); 
		payload[0] = LOWBYTE( value );
		payload[1] = HIGHBYTE( value );

		for(i=DTP_HEADER_SIZE(GATW_MAX_HOPCOUNT)+2; i<txbuf->msdu_len; i++)
			request[i] = 0x81;

        while (1)
        {
		    if (dtp_send_request(dtp, txbuf, GATW_MAX_HOPCOUNT) > 0)
                break;
            hal_delay( 100 );
        }

		// todo: you should check whether the packet is data response type.
		// try to receive the DTP_MAINTAIN_RESPONSE. If time out, then go back to the loop.
		// todo: you should check whether the packet is maintain response type.
		
		vti_setscale( vti, 1 );
		vti_setinterval( vti, 3000, 0x00 );
		vti_start( vti );
		while (!vti_expired(vti))
		{
			len = dtp_recv( dtp, rxbuf, 0x00 );
			if (len > 0)
			{	
				response = opf_msdu( rxbuf );
				payload = DTP_PAYLOAD_PTR(response);
				//ledtune_write( ledtune, MAKE_WORD(payload[1], payload[0]) );
				output_openframe( rxbuf, uart );
				led_toggle( LED_GREEN );
			}
		}
		
		dtp_evolve(dtp, NULL);
		hal_delay(500);
		count ++;
	} /* while (1) */

	dtp_destroy( dtp );
	//ledtune_destroy( ledtune );
}


