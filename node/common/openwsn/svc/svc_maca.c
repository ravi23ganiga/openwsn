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
#include "svc_maca.h"

/* MACA medium access control 
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


static uintx _maca_trysend( TiMACA * mac, TiIoBuf * iobuf, uint8 option );
static uintx _maca_broadcast_rts( TiMACA * mac, TiIoBuf * iobuf );
static uintx _maca_broadcast_cts( TiMACA * mac, TiIoBuf * iobuf );
static uint8 _maca_is_rts( TiMACA * mac, TiIoBuf * iobuf );
static uint8 _maca_is_cts( TiMACA * mac, TiIoBuf * iobuf );
static uintx _maca_tryrecv( TiMACA * mac, TiIoBuf * iobuf, uint8 option );
static uintx _maca_check_rts_cts( TiMACA * mac, TiIoBuf * iobuf );
static uint16 _maca_get_backoff( TiMACA * mac );

TiMACA * maca_construct( char * buf, uintx size )  
{
    TiMACA * mac = (TiMACA *)buf;

	hal_assert( sizeof(TiMACA) <= size );
	memset( buf, 0x00, size );
    mac->txbuf = iobuf_construct( &mac->txbufmem[0], MACA_TXBUF_MEMSIZE );
    mac->rxbuf = iobuf_construct( &mac->rxbufmem[0], MACA_RXBUF_MEMSIZE );
    mac->state = MACA_STATE_NULL;
	return mac;
}

void maca_destroy( TiMACA * mac )
{
    iobuf_destroy( mac->txbuf );
    iobuf_destroy( mac->rxbuf );
}

TiMACA * maca_open( TiMACA * mac, TiFrameTxRxInterface * trx, void * trximpl, 
    TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner )
{
	hal_assert( (trx != NULL) && (timer != NULL) && (mac->state == MACA_STATE_NULL) );

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

	mac->trx->setchannel( mac->trx, CONFIG_MACA_DEFAULT_CHANNEL );
	mac->trx->enable_addrdecode( mac->trx );					
	mac->trx->setpanid( mac->trx, CONFIG_MACA_DEFAULT_PANID );
	mac->trx->setshortaddress( mac->trx, CONFIG_MACA_DEFAULT_LOCAL_ADDRESS );
	mac->trx->switchtomode( mac->trx, FTRX_MODE_RX );

    rand_open( 78 );
	timer_open( timer, 0, NULL, NULL, 0x00 );

    mac->state = MACA_STATE_IDLE;
    return mac;
}

/* TiMACA doesn't call transceiver object's close. so you need to close the 
 * transceiver object manually yourself. 
 */
void maca_close( TiMACA * mac )
{
	timer_close( mac->timer );
    rand_close();
    mac->state = MACA_STATE_NULL;
}

/* this function will try to accept the input data and copy them into internal buffer. 
 * the real data sending is done in maca_evolve(). the caller should 
 * repeatedly call evolve() to drive the MAC to run. 
 *
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully
 */
uintx maca_send( TiMACA * mac, TiIoBuf * iobuf, uint8 option )
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
        // maca_wakeup( mac );

        // @attention
        // if you want to guarantee the frame is sent inside this function, you can 
        // try the following source code:
        // do {
        //      maca_evolve( mac, NULL );
        // }while (mac->state != MACA_STATE_IDLE);
    }

    maca_evolve( mac, NULL );
    return ret;
}

uintx maca_broadcast( TiMACA * mac, TiIoBuf * iobuf, uint8 option )
{
    return 0;
}

/* @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 */
#ifdef CONFIG_MACA_TRX_SUPPORT_ACK
uintx _maca_trysend( TiMACA * mac, TiIoBuf * iobuf, uint8 option )
{
    return mac->trx->send( mac->trx, iobuf, option );
}
#endif

/* call the PHY layer functions and try to send the frame immdietely. if ACK frame 
 * required, then wait for ACK. 
 * 
 * attention: this function is used internally. it doesn't care what kind of state
 * the TiMACA object is in. 
 * 
 * @return
 *	> 0			success
 *	0           failed. no byte has been sent successfully. when it returns 0, 
 *              mac->retry will increase by 1.
 * 
 * @modified by zhangwei on 2010.05.07
 *  - revision
 */
#ifndef CONFIG_MACA_TRX_SUPPORT_ACK
uintx _maca_trysend( TiMACA * mac, TiIoBuf * iobuf, uint8 option )
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
		timer_restart( mac->timer, CONFIG_MACA_ACK_RESPONSE_TIME, 0 ); 
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

uintx _maca_broadcast_rts( TiMACA * mac, TiIoBuf * iobuf )
{
    // todo 
    return 0;
}

uintx _maca_broadcast_cts( TiMACA * mac, TiIoBuf * iobuf )
{
    // todo 
    return 0;
}

uint8 _maca_is_rts( TiMACA * mac, TiIoBuf * iobuf )
{
    // todo 
    return 0;
}

uint8 _maca_is_cts( TiMACA * mac, TiIoBuf * iobuf )
{
    // todo 
    return 0;
}

uintx maca_recv( TiMACA * mac, TiIoBuf * iobuf, uint8 option )
{
    uintx count=0;

    if (iobuf_empty(mac->rxbuf)) 
    {
        hal_assert( mac->state != MACA_STATE_SLEEP );
        hal_assert( mac->state != MACA_STATE_POWERDOWN );

        // if ((mac->state == MACA_STATE_IDLE) || (mac->state == MACA_STATE_RETRY_SEND))
        if (mac->state == MACA_STATE_IDLE)
        {
            _maca_tryrecv( mac, iobuf, option );
            count = _maca_check_rts_cts( mac, iobuf );
        }
    }
    else /* if (!iobuf_empty(mac->rxbuf)) */
    {
        count = iobuf_copyto( mac->rxbuf, iobuf );
        iobuf_clear( mac->rxbuf );
    }

    maca_evolve( mac, NULL );
    return count;
}

/* try to fetch an frame from the transceiver layer. 
 * attention: this function doesn't care what state the TiMACA is in. generally, 
 * you should call this function only in IDLE state. If you call it in other state,
 * you may violate the running of the state machine in the evolve() function.
 */ 
uintx _maca_tryrecv( TiMACA * mac, TiIoBuf * iobuf, uint8 option )
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
		 * only the DATA type frame will be accepted here. The other types, such as 
         * COMMAND, BEACON or ACK will be ignored. 
		 */
        buf = iobuf_ptr(iobuf);
		ctrl = (((uint16)buf[1]) << 8) | (buf[2]);

		if (FCF_FRAMETYPE(ctrl) != FCF_FRAMETYPE_DATA)
        {
			count = 0;
            iobuf_clear( iobuf );
        }
        else
            iobuf_setlength( iobuf, count );
    }

    return count;
}

/* check whether the current frame is an RTS frame or CTS frame. it it is, then do
 * corresponding processing and clear itself from the iobuf. after the call, the iobuf
 * should be empty or contains an data frame. 
 */
uintx _maca_check_rts_cts( TiMACA * mac, TiIoBuf * iobuf )
{
    // until here, only the 802.15.4 DATA type frame is accepted. the other types 
    // are rejected. we need further check whether the incoming frame is RTS 
    // type or not.
    
    // if this frame is an RTS frame
    if (_maca_is_rts(mac, iobuf))
    {
        // if this RTS frame destination is the current node, then it should
        // prepare for receiving, or else it should go into alert sleep mode
        // for some while. 
                
        iobuf_clear( iobuf );
        if (0)
        {
            // mac->sleeprequest_duratioin = 10;
            bit8_set( mac->request, MACA_SLEEP_REQUEST );
        }
        else{
            // the RTS destination is current node. prepare for receving incoming
            // frames. the MACA caller must call maca_evolve() or maca_recv()
            // fast enough to avoid lossing frame.
            _maca_broadcast_cts( mac, mac->ctsbuf );

            // @attention
            // after sending CTS frame related to the RTS received just now,
            // we simply stay in the IDLE state. and in the next call of 
            // the evolve() function or maca_recv() function, the data frame
            // will be retrieved from PHY layer and put into mac->rxbuf.
        }
    }
    // if we receive the CTS frame in IDLE state, just clear the buffer and 
    // ignore it. we only keep the data frames inside mac->rxbuf.
    else if (_maca_is_cts(mac, iobuf))
    {
        iobuf_clear( iobuf );
    }

    return iobuf_length( iobuf );
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
inline uint16 _maca_get_backoff( TiMACA * mac )
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
void maca_evolve( void * macptr, TiEvent * e )
{
	TiMACA * mac = (TiMACA *)macptr;
    TiFrameTxRxInterface * transceiver = mac->trx;
    TiEvent newevent;

    if (mac->state == MACA_STATE_NULL)
        return;

    switch (mac->state)
    {
	case MACA_STATE_IDLE:   
        // try to retrieve frame from PHY layer. we should do receiving process
        // more frequently to avoid frame lossing. 
        // 
        if (_maca_tryrecv(mac, mac->rxbuf, 0x00) > 0)
        {
            // until here, only the 802.15.4 DATA type frame is accepted. the other types 
            // are rejected. we need further check whether the incoming frame is RTS 
            // type or not.

            if (_maca_check_rts_cts(mac, mac->rxbuf) > 0)
            {
                if (mac->listener != NULL)
                {
                    memset( &newevent, 0x00, sizeof(newevent) );
                    newevent.id = MACA_EVENT_FRAME_ARRIVAL;
	                mac->listener( mac->lisowner, &newevent );
                }
            }
        }

        if (!iobuf_empty(mac->txbuf))
        {
            // todo with this function here 2010.05.10
            // need more developing on how to broadcast rts
            if (_maca_broadcast_rts(mac, NULL) > 0)
            {
                timer_restart( mac->timer, CONFIG_MACA_WAIT_CTS_DURATION, 0 );
                mac->state = MACA_STATE_WAIT_CTS;
            }
        }


        if (mac->state == MACA_STATE_IDLE)
        {
            // only in the idle state, we check for the sleeping request bit. if
            // this bit in the request variable is set by the sleep() function, 
            // then we call transceiver's sleep() function and force it into sleep
            // mode. in the sleeping mode, all the frames in the mac layer are still 
            // pending inside it, so the frames won't lost. different to the POWERDOWN
            // mode, the whole application can recover from SLEEP mode fast and continue
            // previous state.
            //
            if (bit8_get(mac->request, MACA_SLEEP_REQUEST))
            {
                transceiver->switchtomode( transceiver->object, FTRX_MODE_SLEEP );
                bit8_clr( mac->request, MACA_SLEEP_REQUEST );
                mac->state = MACA_STATE_SLEEP;
            }
        }
        break;

    case MACA_STATE_WAIT_CTS:
        if (_maca_tryrecv( mac, mac->rxbuf, 0x00) > 0)
        {
            // todo: check whether this is an CTS frame
            if (1)
            {
                // if the CTS frame received, then we start the transmission of 
                // pending frames immediately without hesitation. if we failed
                // in the sending, we should backoff for some time, and restart
                // the RTS-CTS-DATA process again.
                //
                if (_maca_trysend(mac, mac->txbuf, 0x00) > 0)
                {
                    // sending the frame successfully
                    mac->retry = 0;
                    mac->state = MACA_STATE_IDLE;
                }
                else{
                    if (mac->retry > CONFIG_MACA_MAX_RETRY)
                    {
                        // failed to sending the frame after 3 try. we had to accept
                        // the situation and go back to IDLE state
                        //
                        iobuf_clear( mac->txbuf );
                        mac->retry = 0;
                        // mac->failed ++;
                        mac->state = MACA_STATE_IDLE;
                    }
                    else{
                        timer_restart( mac->timer, _maca_get_backoff(mac), 0 );
                        mac->retry ++;
                        mac->state = MACA_STATE_RETRY_SEND;
                    }
                }
            }
            else{
                // @attention
                // another type frame came to this node when it hopes an CTS frame
                // do nothing and continue waiting for CTS. if the caller doesn't
                // retrieve this frame out from "mac->rxbuf" then it will be
                // overwrite by next frame 
            }
        }
        else if (timer_expired(mac->timer))
        {
            // the sending process failed this time. we need to restart the whole
            // sending process now. first, we need to delay some time. this duration
            // is generally known as backoff time. 
            //
            timer_restart( mac->timer, _maca_get_backoff(mac), 0 );
            mac->retry ++;
            mac->state = MACA_STATE_RETRY_SEND;
        }
        break;    

    case MACA_STATE_RETRY_SEND:
        // since the retry backoff time is pretty long, so I design this MACA to
        // retrieve frame from PHY layer in order to decrease receving latency. 
        // this can avoid some frame lossing. 
        // 
        if (_maca_tryrecv(mac, mac->rxbuf, 0x00) > 0)
        {
            // until here, only the 802.15.4 DATA type frame is accepted. the other types 
            // are rejected. we need further check whether the incoming frame is RTS 
            // type or not.

            if (_maca_check_rts_cts(mac, mac->rxbuf) > 0)
            {
                if (mac->listener != NULL)
                {
                    memset( &newevent, 0x00, sizeof(newevent) );
                    newevent.id = MACA_EVENT_FRAME_ARRIVAL;
	                mac->listener( mac->lisowner, &newevent );
                }
            }
        }

        // if the backoff duration expired, then we call PHY layer functions to 
        // send the frame immediately.
        //
        if (timer_expired(mac->timer))
        {
            // for simplicity, assume the CTS already sent successfully here
            _maca_broadcast_rts(mac, NULL);
            timer_restart( mac->timer, CONFIG_MACA_WAIT_CTS_DURATION, 0 );
            mac->state = MACA_STATE_WAIT_CTS;
        }
        break;

    case MACA_STATE_SLEEP:
        if (bit8_get(mac->request, MACA_WAKEUP_REQUEST))
        {
            transceiver->switchtomode( transceiver->object, FTRX_MODE_RX );
            bit8_clr( mac->request, MACA_WAKEUP_REQUEST );
            mac->state = MACA_STATE_IDLE;
        }
        break;

    default:
        // this should never happen.  the following source code can pull the state 
        // to IDLE and let the state machine to continue in case of state disorders.
        hal_assert( false );
        mac->state = MACA_STATE_IDLE;
    }

    if (mac->request > 0)
    {
        if (bit8_get(mac->request, MACA_SHUTDOWN_REQUEST))
        {
            // no matter what the current state is, you can do shutdown. the power will 
            // lost. so the data inside the SRAM will be lost. 

            // if timer is already running, then we should stop it. 
            timer_stop( mac->timer );
            iobuf_clear( mac->txbuf );
            iobuf_clear( mac->rxbuf );

            transceiver->switchtomode( transceiver->object, FTRX_MODE_POWERDOWN );
            bit8_clr( mac->request, MACA_SHUTDOWN_REQUEST );
            mac->state = MACA_STATE_POWERDOWN;
        }

        if (bit8_get(mac->request, MACA_STARTUP_REQUEST))
        {
            if (mac->state == MACA_STATE_POWERDOWN)
            {
                transceiver->switchtomode( transceiver->object, FTRX_MODE_RX );
                bit8_clr( mac->request, MACA_STARTUP_REQUEST );
                mac->state = MACA_STATE_IDLE;
            }
        }
    }

    if (e != NULL)
    {
	    switch (e->id)
	    {
        case MACA_EVENT_FRAME_ARRIVAL:
            // if the incoming event indicates that the transceiver receives an frame, 
            // then it simply pass the event to listener object. the listener object
            // will call maca_recv() to retrieve the frame out. 
            //
            // @attention
            // - if the caller cann't check for incoming frames fast enough, then the 
            // new frames will overwrite the old ones inside the transceiver object.
            // it's the application developer's responsibility to guarantee there
            // no packet loss due to slow querying and insufficient buffering spaces.
            //
            // - however, for some transceiver such as 802.15.4 compatible chip cc2420, 
            // it may report ACK/BECON/COMMAND frames according to 802.15.4 specification. 
            // however, the upper layer doesn't need these frames. the maca receiving 
            // mechanism will filter them, and the RTS/CTS frames. only the valid data
            // frame are reported to the upper layer.
            //            
            if (mac->listener != NULL)
	        {
		        mac->listener( mac->lisowner, e );
	        }
            break;

        default:
		    break;
	    }
    }

	return;
}
