/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

#include "svc_configall.h"
#include <string.h>
#include "svc_foundation.h"
#include "../rtl/rtl_bit.h"
#include "../rtl/rtl_iobuf.h"
#include "../rtl/rtl_random.h"
#include "../rtl/rtl_openframe.h"
#include "../hal/hal_frame_transceiver.h"
#include "svc_adaptaloha.h"

/* ALOHA medium access control (MAC) protocol 
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
 * @author zhangwei on 20060724
 * @modified by zhangwei on 20090725
 *	- revision. compile passed.
 * 
 * @modified by zhangwei on 2010.05.07
 *  - add state transfer machine implementation
 */

#define bit8_set(token,index) ((token) |= (_BV(index)))
#define bit8_get(token,index) ((token) & (_BV(index)))
#define bit8_clr(token,index) ((token) &= (~_BV(index)))


static uintx _adaptive_aloha_trysend( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option );
static uintx _adaptive_aloha_tryrecv( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option );
static uint16 _adaptive_aloha_get_duration( TiAdaptiveAloha * mac );

TiAdaptiveAloha * adaptive_aloha_construct( char * buf, uintx size )  
{
    TiAdaptiveAloha * mac = (TiAdaptiveAloha *)buf;

	hal_assert( sizeof(TiAdaptiveAloha) <= size );
	memset( buf, 0x00, size );
    mac->txbuf = iobuf_construct( &mac->txbufmem[0], IOBUF_HOPESIZE(128) );
    mac->rxbuf = iobuf_construct( &mac->rxbufmem[0], IOBUF_HOPESIZE(128) );
    mac->state = ADPTALOHA_STATE_NULL;
	return mac;
}

void adaptive_aloha_destroy( TiAdaptiveAloha * mac )
{
    iobuf_destroy( mac->txbuf );
    iobuf_destroy( mac->rxbuf );
}

TiAdaptiveAloha * adaptive_aloha_open( TiAdaptiveAloha * mac, TiFrameTxRxInterface * trx, void * trximpl, 
    TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner )
{
	hal_assert( (trx != NULL) && (timer != NULL) && (mac->state == ADPTALOHA_STATE_NULL) );

	mac->trx = trx;
    mac->trx->object = trximpl;
	mac->timer = timer;
    mac->loadfactor = 5;
    mac->request = 0x00;
    mac->retry = 0;
	mac->listener = listener;
	mac->lisowner = lisowner;

    iobuf_clear( mac->rxbuf );
    iobuf_clear( mac->txbuf );

    // assume: the transceiver(mac->trx) has already opened before calling this function

	mac->trx->setchannel( mac->trx, CONFIG_ADPTALOHA_DEFAULT_CHANNEL );
	mac->trx->enable_addrdecode( mac->trx );					
	mac->trx->setpanid( mac->trx, CONFIG_ADPTALOHA_DEFAULT_PANID );
	mac->trx->setshortaddress( mac->trx, CONFIG_ADPTALOHA_DEFAULT_LOCAL_ADDRESS );
	mac->trx->switchtomode( mac->trx, FTRX_MODE_RX );

    rand_open( 78 );
	timer_open( timer, 0, NULL, NULL, 0x00 );

    mac->state = ADPTALOHA_STATE_IDLE;
    return mac;
}

/* TiAdaptiveAloha doesn't call transceiver object's close. so you need to close the 
 * transceiver object manually yourself. 
 */
void adaptive_aloha_close( TiAdaptiveAloha * mac )
{
	timer_close( mac->timer );
    rand_close();
    mac->state = ADPTALOHA_STATE_NULL;
}

/* this function will try to accept the input data and copy them into internal buffer. 
 * the real data sending is done in adaptive_aloha_evolve(). the caller should 
 * repeatedly call evolve() to drive the MAC to run. 
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully
 */
uintx adaptive_aloha_send( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option )
{
    uintx ret=0;

    if (iobuf_empty(mac->txbuf))
    {
        ret = iobuf_copyfrom( mac->txbuf, iobuf );

        // @modified by zhangwei on 2010.05.09
        // in the past, the mac is automatically wakeup by the following source code
        // if there's a frame for sending. however, according to the low low power design
        // principal, we'd better do this manually to avoid uncessary wakeup and 
        // forgotten sleepings. so i giveup automatically wakeup in the end.
        //
        // adaptive_aloha_wakeup( mac );

        // @attention
        // if you want to guarantee the frame is sent inside this function, you can 
        // try the following source code:
        // do {
        //      adaptive_aloha_evolve( mac, NULL );
        // }while (mac->state != ADPTALOHA_STATE_IDLE);
    }

    adaptive_aloha_evolve( mac, NULL );
    return ret;
}

uintx adaptive_aloha_broadcast( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option )
{
    // todo
    return 0;
}

/* @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 */
#ifdef CONFIG_ADPTALOHA_TRX_ACK_SUPPORT
uintx _adaptive_aloha_trysend( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option )
{
    return mac->trx->send( mac->trx, iobuf, option );
}
#endif

/* call the PHY layer functions and try to send the frame immdietely. if ACK frame 
 * required, then wait for ACK
 * 
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 * 
 * @modified by zhangwei on 2010.05.07
 *  - revision
 */
#ifndef CONFIG_ADPTALOHA_TRX_ACK_SUPPORT
uintx _adaptive_aloha_trysend( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option )
{
	uintx count = 0;
    uint16 ctrl;
    bool expired;
    char * ptr;
	
	while (!mac->trx->ischannelclear( mac->trx ))
		continue;

	mac->trx->switchtomode( mac->trx, FTRX_MODE_TX );
    count = mac->trx->send( mac->trx, iobuf, option );
	mac->trx->switchtomode( mac->trx, FTRX_MODE_RX );
	
    if (count > 0)
	{
		/* If the frame has been successfully sent by the transceiver, the program will
		 * start to wait for the ACK frame replied from the receiver. The maximum waiting
		 * duration is configured by macro CONFIG_ALOHA_ACK_RESPONSE_TIME. The time unit
		 * is milli-seconds.
		 */

        /* wait for ack frame from the receiver. if we cannot receive the ack frame
         * in time, then we can regard the frame just send is lost during transmission.
         * this duration here is for the receiver to send the ack back. it consists 
         * the processing time inside receiver node and the transmission time in 
         * the air.
         */ 
		timer_restart( mac->timer, CONFIG_ADAPT_ALOHA_ACK_RESPONSE_TIME, 0 ); 
		expired = true;
		while (!timer_expired( mac->timer ))
		{
            hal_delay( 1 );
			if (mac->trx->recv(mac->trx, mac->rxbuf, 0x00) > 0)
			{
				/* 2 bytes for frame control */
                ptr = iobuf_ptr( mac->rxbuf );
				ctrl = (((uint16)(ptr[1])) << 8) | (ptr[2]);

				/* If the incoming frame is ACK frame, then we can return success. 
				 * other type frames will be ignored. 
                 * 
                 * @attention
				 *  - Theoretically speaking, the current frame arrived may possibly be the 
                 * data type from other nodes. in the current version, this frame will be 
                 * overwrotten, which leads to unnecessary frame loss.
				 */
				if (FCF_FRAMETYPE(ctrl) == FCF_FRAMETYPE_ACK)
				{
					timer_stop( mac->timer );
                    iobuf_clear( mac->rxbuf );
					expired = false;
					break;
				}
			}
		}
		
		if (expired)
		{
			count = 0;
		}
	}

	return count;
}
#endif

uintx adaptive_aloha_recv( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option )
{
    uintx count=0;

    _adaptive_aloha_tryrecv( mac, mac->rxbuf, option );
    if (!iobuf_empty(mac->rxbuf))
    {
        count = iobuf_copyto( mac->rxbuf, iobuf );
        iobuf_clear( mac->rxbuf );
    }
    adaptive_aloha_evolve( mac, NULL );
    return count;
}

uintx _adaptive_aloha_tryrecv( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option )
{
    uintx count=0;
    uint16 ctrl=0;
    char * buf;

    count = mac->trx->recv( mac->trx, iobuf, option );
    if (count > 0)
    {
    	/* according to ALOHA protocol, the program should send ACK/NAK after receiving
	     * a data frame. acoording to 802.15.4 protocol, there maybe COMMAND, BEACON or 
         * ACK frames in the air. however, these type of frames are all used inside the 
         * MAC layer only. we should filter them and keep the DATA frames only.
         * 
		 * 2 bytes for frame control
		 * only the DATA type frame will be accepted here. The other types, such as COMMAND,
		 * BEACON or ACK will be ignored. 
		 */
        buf = iobuf_ptr(iobuf);
		ctrl = (((uint16)buf[1]) << 8) | (buf[2]);

		if (FCF_FRAMETYPE(ctrl) != FCF_FRAMETYPE_DATA)
        {
			count = 0;
            iobuf_setlength( iobuf, 0 );
        }
        else
            iobuf_setlength( iobuf, count );
    }

    return count;
}

/* currently, the duration is configured as:
 * loadfactor => duration
 *      0           1
 *      1           1*8
 *      2           2*8
 *      3           3*8
 *      4           4*8
 *      ...
 */
inline uint16 _adaptaloha_get_duration( TiAdaptiveAloha * mac )
{
    return rand_uint16(((uint16)(mac->loadfactor))  << 3);
}

/* this function can be used as TiFrameTxRxInterface's listener directly. so you can get
 * to know when a new frame arrives through the event's id. however, the TiCc2420Adapter's 
 * listener is fired by ISR. so we don't suggest using aloha_evolve() as TiCc2420Adapter's
 * listener directly.
 *
 * the evolve() function also behaviors like a thread.
 */
void adaptive_aloha_evolve( void * macptr, TiEvent * e )
{
	TiAdaptiveAloha * mac = (TiAdaptiveAloha *)macptr;
    TiFrameTxRxInterface * transceiver = mac->trx;

    if (mac->state == ADPTALOHA_STATE_NULL)
        return;

    switch (mac->state)
    {
	case ADPTALOHA_STATE_IDLE:   
        // if mac->txbuf isn't empty, then 
        //     start the sending random delay timer
        //     goto ADPTALOHA_WAIT_FOR_PHY_SENDING state
        // endif 
        //
        if (!iobuf_empty(mac->txbuf))
        {
            // we elegantly wait for a random time before call the transceiver to 
            // sending the frame out really. the waiting time is estimated according
            // to current traffic load factor. if the traffic load is heavy, then
            // the delay time is increasing according to nearly exponential law. 
            //
            timer_restart( mac->timer, _adaptaloha_get_duration(mac), 0 );
            mac->state = ADPTALOHA_STATE_WAITFOR_PHY_SENDING;
        }        

        if (mac->state == ADPTALOHA_STATE_IDLE)
        {
            // only in the idle state, we check for the sleeping request bit. if
            // this bit in the request variable is set by the sleep() function, 
            // then we call transceiver's sleep() function and force it into sleep
            // mode. in the sleeping mode, all the frames in the mac layer are still 
            // pending inside it, so the frames won't lost. different to the POWERDOWN
            // mode, the whole application can recover from SLEEP mode fast and continue
            // previous state.
            //
            if (bit8_get(mac->request, ADPTALOHA_SLEEP_REQUEST))
            {
                transceiver->switchtomode( transceiver->object, FTRX_MODE_SLEEP );
                bit8_clr( mac->request, ADPTALOHA_SLEEP_REQUEST );
                mac->state = ADPTALOHA_STATE_SLEEP;
            }
        }
        break;

    case ADPTALOHA_STATE_WAITFOR_PHY_SENDING:
        // if sending random delay timer expired, then call PHY layer functions to 
        // send the frame immediately.
        //
        if (timer_expired(mac->timer))
        {
            // three cases for the frame:
            // - it's an broadcasting frame, then we neen't to wait for ACK and 
            // simply goto IDLE state. 
            // - it's an frame which doesn'e need ACK, then goto IDLE state;
            // - it's an frame needing ACK.
            // 
            if (_adaptive_aloha_trysend(mac, mac->txbuf, 0x00) > 0)
            {
                // sending the frame successfully
                mac->retry = 0;
                mac->state = ADPTALOHA_STATE_IDLE;
            }
            else{
                if (mac->retry > CONFIG_ADPTALOHA_MAX_RETRY)
                {
                    // failed to sending the frame after 3 try. we had to accept
                    // the situation and go back to IDLE state
                    //
                    iobuf_clear( mac->txbuf );
                    mac->retry = 0;
                    // mac->failed ++;
                    mac->state = ADPTALOHA_STATE_IDLE;
                }
                else{
                    timer_restart( mac->timer, _adaptaloha_get_duration(mac), 0 );
                    mac->retry ++;
                    mac->state = ADPTALOHA_STATE_WAITFOR_PHY_SENDING;
                }
            }
        }
        break;

    case ADPTALOHA_STATE_SLEEP:
        if (bit8_get(mac->request, ADPTALOHA_WAKEUP_REQUEST))
        {
            transceiver->switchtomode( transceiver->object, FTRX_MODE_RX );
            bit8_clr( mac->request, ADPTALOHA_WAKEUP_REQUEST );
            mac->state = ADPTALOHA_STATE_IDLE;
        }
        break;

    default:
        // this should never happen.  the following source code can pull the state 
        // to IDLE and let the state machine to continue in case of state disorders.
        hal_assert( false );
        mac->state = ADPTALOHA_STATE_IDLE;
    }

    if (mac->request > 0)
    {
        if (bit8_get(mac->request, ADPTALOHA_SHUTDOWN_REQUEST))
        {
            // no matter what the current state is, you can do shutdown. the power will 
            // lost. so the data inside the SRAM will be lost. 

            // if timer is already running, then we should stop it. 
            timer_stop( mac->timer );
            iobuf_clear( mac->txbuf );
            iobuf_clear( mac->rxbuf );

            transceiver->switchtomode( transceiver->object, FTRX_MODE_POWERDOWN );
            bit8_clr( mac->request, ADPTALOHA_SHUTDOWN_REQUEST );
            mac->state = ADPTALOHA_STATE_POWERDOWN;
        }

        if (bit8_get(mac->request, ADPTALOHA_STARTUP_REQUEST))
        {
            if (mac->state == ADPTALOHA_STATE_POWERDOWN)
            {
                transceiver->switchtomode( transceiver->object, FTRX_MODE_RX );
                bit8_clr( mac->request, ADPTALOHA_STARTUP_REQUEST );
                mac->state = ADPTALOHA_STATE_IDLE;
            }
        }
    }

    if (e != NULL)
    {
	    switch (e->id)
	    {
        case ADPTALOHA_EVENT_FRAME_ARRIVAL:
            // if the incoming event indicates that the transceiver receives an frame, 
            // then it simply pass the event to listener object. the listener object
            // will call adaptive_aloha_recv() to retrieve the frame out. 
            //
            // @attention
            // - if the caller cann't check for incoming frames fast enough, then the 
            // new frames will overwrite the old ones inside the transceiver object.
            // it's the application developer's responsibility to guarantee there
            // no packet loss due to slow querying and insufficient buffering spaces.
            //
            // - however, for some transceiver such as 802.15.4 compatible chip cc2420, 
            // it may report ACK/BECON/COMMAND frames according to 802.15.4 specification. 
            // however, the upper layer doesn't need these frames. the program will ignore 
            // them.
            //
            // then the following source code is enabled and the 802.15.4's ACK/BECON/COMMAND
            // frames will be filtered. and only the DATA frame is reported to the upper 
            // layer. this can guarantee every time the listener runs, it can read an 
            // valid frame. 
            //            
            if (_adaptive_aloha_tryrecv(mac, mac->rxbuf, 0x00) > 0)
            {
                if (mac->listener != NULL)
	            {
		            mac->listener( mac->lisowner, e );
	            }
            }
            break;

        default:
		    break;
	    }
    }

	return;
}
