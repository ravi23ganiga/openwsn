/****************************************************************************** 
 * svc_aloha.c
 * ALOHA medium access control (MAC) protocol 
 *  
 * @attention
 *	- This version will wait for the ACK frame after sending a DATA frame immediately.
 * If another DATA frame arrives during this period, it will be ignored. This can be 
 * improved in the future. 
 *
 *	- Different to the basic ALOHA protocol, this version will check whether the
 * communication channel is clear or not. It will only send the frame when the channel
 * is clear. This clear channel assesement (CCA) functionality is provided by 
 * the cc2420 transceiver.
 *
 *  - When encounting sending conflicts, this version of ALOHA will retry for some 
 * times. The retry count is configures by macro CONFIG_ALOHA_MAX_RETRY. However, 
 * there's no backoff when retry the sending, so there's still large chance to 
 * be conflicted again. This should be improved before this version of ALOHA can 
 * be used in real applications. 
 *
 *  - This version doesn't depend on "osx" functionalities. So it's easy to be used.
 * The enhanced version of this simple aloha is in svc_openmac and svc_adaptivealoha
 *
 *	In Summary, this version of ALOHA is mainly for demonstration and experiments. 
 * It's a good start to be used for teaching than in real applications. 
 *
 * @author zhangwei and shan-lei on 20050710
 *	- first version on PIC16F876. This version is fully discarded.
 * @modified by zhangwei on 20090610
 *	- fully revised for ICT GAINZ platform
 * @modified by zhangwei on 20090725
 *	- revision. compile passed.
 * @modified by zhangwei on 20090802
 *  - correct bug in _aloha_recv(). The former byte order to create the frame control
 *    word is wrong. 
 * @modified by Shimiaojing on 20091025
 *-add macro_alohachannelclear(mac) HAL_READ_CC_CCA_PIN()and delete line141 142 and match the frame construct of 
 * MAC opf_buffer &opf_datalen id different
 * line 89 147 291 who modified it  
 *  solved ok    2009 .11.17
 *****************************************************************************/

#include "svc_configall.h"
#include <string.h>
#include "svc_aloha.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_cpu.h"
#include "../rtl/rtl_openframe.h"

#define aloha_ischannelclear(mac) cc2420_ischannelclear(mac->tranceiver)

static inline uintx _aloha_trysend( TiAloha * mac, char * buf, uint8 len, uint8 option );
static inline uintx _aloha_send( TiAloha * mac, char * buf, uint8 len, uint8 option );
static inline uintx _aloha_recv( TiAloha * mac, char * buf, uint8 len, uint8 option );


TiAloha * aloha_construct( char * buf, uintx size )
{
	memset( buf, 0x00, size );
	hal_assert( sizeof(TiAloha) <= size );
	return (TiAloha *)buf;
}

void aloha_destroy( TiAloha * mac )
{
	return;
}

TiAloha * aloha_open( TiAloha * mac, TiCc2420Adapter * transceiver, uint8 chn, uint16 panid, 
	uint16 address, TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner, uint8 option )
{
	//hal_assert( (transceiver != NULL) && (timer != NULL) );

	//mac->state = ALOHA_STATE_RXWAIT;
	mac->transceiver = transceiver;  
	mac->timer = timer;
	mac->listener = listener;
	mac->lisowner = lisowner;
	mac->rxlen = 0;
	mac->retry = 0;
	mac->option = option;

	// timer_open( timer, 0, NULL, NULL, 0x00 );
	/* we needn't repeatedly triggered timer */
	// timer_setinterval( mac->timer, CONFIG_ALOHA_ACK_RESPONSE_TIME, 0 );

	// for the aloha module, we should enable ACK mechanism
	cc2420_enable_addrdecode( transceiver );
	cc2420_enable_autoack( transceiver );
	cc2420_setchannel( transceiver, chn );
	cc2420_setpanid( transceiver, panid );
	cc2420_setshortaddress( transceiver, address );
	cc2420_setrxmode( transceiver );							

    return mac;
}

void aloha_close( TiAloha * mac )
{
	// timer_close( mac->timer );
	cc2420_close( mac->transceiver );
}

uintx aloha_send( TiAloha * mac, TiOpenFrame * opf, uint8 option )
{
	// assert the buffer opf has already an complete frame
	return _aloha_send( mac, opf_buffer(opf), opf_datalen(opf), option ); 
}

uintx aloha_broadcast( TiAloha * mac, TiOpenFrame * opf, uint8 option )
{
	uint16 * fcf;

	// assert: The parameter opf has already an complete frame. It must be casted 
	// or parsed before, or else the function call opf_set_shortaddrto() will be failed.

	// get the frame control field
	fcf = (uint16 *)(&opf->buf[1]); 
	opf_set_control( opf, OPF_CLR_ACK_REQUEST(*fcf) );
     
	// we only used PAN inside broadcasting. you may also need PAN-outside broadcasting.
	// However, the current version hasn't support such applications yet.
	opf_set_shortaddrto( opf, OPF_BROADCAST_ADDRESS );
	return _aloha_send( mac, opf_buffer(opf), opf_datalen(opf), option ); 
}

uintx aloha_recv( TiAloha * mac, TiOpenFrame * opf, uint8 option )
{
	uint8 count;
	count = _aloha_recv( mac, opf_buffer(opf), opf_size(opf), option );
	if (count > 0)
	{
		if (opf_parse(opf,0))
		{
			/* Check whether the current frame is a DATA type frame.
			 * only the DATA type frame will be accepted here. The other types, such as COMMAND,
			 * BEACON or ACK will be ignored here.
			 * buf[0] is the length byte. buf[1] and buf[2] are frame control bytes.
			 */		 
			if (FCF_FRAMETYPE(*(opf->ctrl)) != FCF_FRAMETYPE_DATA)
			{
				count = 0;
			}
			else{
				opf->datalen = count;
			}
		}
		else{
			count = 0;
		}
	}

	return count;
}

/* @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully
 */
uintx _aloha_send( TiAloha * mac, char * buf, uint8 len, uint8 option )
{    
	uintx count=0;

	if (len == 0)
		return 0;
  
	while (mac->retry < CONFIG_ALOHA_MAX_RETRY )
	{  
		count = _aloha_trysend(mac, buf, len, option);
		if (count > 0)
		{
			mac->retry = 0;
			break;
		}

		/* Suggest delay a random little period and restart the sending process again 
		 * However, it's not easy to implement the random delay in a simple method. So
		 * we simply wait 300 milli-seconds here.
		 */
		hal_delay( 300 );
		mac->retry++;
	}

	/* This function failed to send the frame for at least CONFIG_ALOHA_MAX_RETRY times. 
	 * we had no other good solution except reporting failure to the master function. */
	if (mac->retry >= CONFIG_ALOHA_MAX_RETRY)
	{    
		mac->retry = 0;
		count = 0;
	}

	return count;	
}

/* @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 */
uintx _aloha_trysend( TiAloha * mac, char * buf, uint8 len, uint8 option )
{
	uintx count = 0;
    //uint16 ctrl;
    //bool expired;
	
	//while (!aloha_ischannelclear( mac->transceiver))
	//continue;
	
	// cc2420_settxmode( mac->transceiver );
	count = cc2420_send( mac->transceiver, buf, len, option );
	if (count > 0)
	{   
	
		//// todo : check whether this frame needs ACK

		///* If the frame has been successfully sent by the transceiver, the program will
		// * start to wait for the ACK frame replied from the receiver. The maximum waiting
		// * duration is configured by macro CONFIG_ALOHA_ACK_RESPONSE_TIME. The time unit
		// * is milli-seconds.
		// */
		//cc2420_setrxmode( mac->transceiver );
		//// todo
		//// whether the cc2420 can be stable enough for receiving 
		//// you may need to check the cc2420 status to guarantee this

		//timer_start( mac->timer );
		//expired = true;
		//while (!timer_expired( mac->timer ))
		//{
		//	if (cc2420_read( mac->transceiver, &(mac->rxbuf[0]), CONFIG_CC2420_RXBUFFER_SIZE, 0x00 ) > 0)
		//	{
		//		/* 2 bytes for frame control */
		//		ctrl = ((uint16)(mac->rxbuf[1]) << 8) | (mac->rxbuf[2]);

		//		/* @attention
		//		 * If the incoming frame is ACK frame, then we can return successfully. 
		//		 * Other type frames will be ignored. 
		//		 * -> This should be improved in the future. Theoretically speaking, the 
		//		 * current frame arrived may possibly be the data type from other nodes. 
		//		 * You should not ignore such types or else encounter unnecessary frame loss.
		//		 */
		//		if (FCF_FRAMETYPE(ctrl) == FCF_FRAMETYPE_ACK)
		//		{
		//			timer_stop( mac->timer );
		//			mac->retry = 0;
		//			expired = false;
		//			break;
		//		}

		//		/* We needn't this frame. Simply ignore it */
		//		mac->rxlen = 0;
		//	}
		//}
		//
		//if (expired)
		//{
		//	mac->retry ++;
		//	count = 0;
		//}
	}

	return count;
}

uintx _aloha_recv( TiAloha * mac, char * buf, uint8 len, uint8 option )
{
	uintx count;
  //  uint16 ctrl=0;

	/* According to ALOHA protocol, the program should send ACK/NAK after receiving
	 * a data frame. however, this is done by cc2420 transceiver. so nothing to 
	 * do here 
	 */
	count = cc2420_recv( mac->transceiver, buf, len, option );
	if (count > 0)
	{   
		
		/* 2 bytes for frame control
		 * only the DATA type frame will be accepted here. The other types, such as COMMAND,
		 * BEACON or ACK will be ignored here.
         * buf[0] is the length byte. buf[1] and buf[2] are frame control bytes.
         */		 
		//ctrl = (((uint16)buf[2]) << 8) | (buf[1]);

		//if (FCF_FRAMETYPE(ctrl) != FCF_FRAMETYPE_DATA)
		{
		//	count = 0;
		}
	}

	return count;
}

/* this function can be used as TiCc2420Adapter's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using aloha_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void aloha_evolve( void * macptr, TiEvent * e )
{
	TiAloha * mac = (TiAloha *)macptr;

	cc2420_evolve( mac->transceiver );

	if (e)
	{
		switch (e->id)
		{
		case EVENT_DATA_ARRIVAL:
			/* If the AOHA listener isn't NULL, then query the transceiver to get possible 
			 * incomming frames. If has, then report to the listener owner. */

			// todo
			if (mac->listener != NULL)
			{
				//mac->rxlen = aloha_recv(mac, &(mac->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00);
				//if (mac->rxlen > 0)
					mac->listener( mac->lisowner, e );
			}

		case 1:
			break;
		}
	}

	return;
}

