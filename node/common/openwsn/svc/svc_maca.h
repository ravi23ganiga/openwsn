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

#ifndef _SVC_MACA_H_4829_
#define _SVC_MACA_H_4829_

/*******************************************************************************
 * svc_maca
 *
 * @state
 *  in developing
 *
 * @author zhangwei in 200912
 *  - first created
 * @modified by zhangwei on 2010.05.10
 *  - revision. 
 * 
 ******************************************************************************/

/* hal_transceiver.i contains the transceiver interface definition. generally, you 
 * should use functions only inside the interface to implement your medium access 
 * control layer(MACA).
 */
#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_iobuf.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_frame_transceiver.h"


#define CONFIG_MACA_DEFAULT_PANID				0x0001
#define CONFIG_MACA_DEFAULT_LOCAL_ADDRESS		0x07
#define CONFIG_MACA_DEFAULT_REMOTE_ADDRESS      0x08
#define CONFIG_MACA_DEFAULT_CHANNEL            11
#define CONFIG_MACA_BROADCAST_ADDRESS          0xFFFF

#define CONFIG_MACA_MAX_RETRY                  3
#define CONFIG_MACA_ACK_RESPONSE_TIME          10

#define CONFIG_MACA_TRX_SUPPORT_ACK

#define CONFIG_MACA_WAIT_CTS_DURATION          5
#define CONFIG_MACA_RTS_SLEEPTIME              10

#define MACA_STATE_NULL                        0
#define MACA_STATE_IDLE                        1
#define MACA_STATE_WAIT_CTS                    2
#define MACA_STATE_RETRY_SEND                  5
#define MACA_STATE_SLEEP                       3
#define MACA_STATE_POWERDOWN                   4

#define MACA_SLEEP_REQUEST                     1
#define MACA_WAKEUP_REQUEST                    2
#define MACA_SHUTDOWN_REQUEST                  3
#define MACA_STARTUP_REQUEST                   4

#define MACA_EVENT_FRAME_ARRIVAL               234 // todo

#define MACA_RXBUF_MEMSIZE IOBUF_HOPESIZE(128)
#define MACA_TXBUF_MEMSIZE IOBUF_HOPESIZE(128)


#ifdef __cplusplus
extern "C"{
#endif

/* TiMACA:
 *	- state
 */
typedef struct{
    uint8 state;
    TiTimerAdapter * timer;
	TiFrameTxRxInterface * trx;
    uint8 loadfactor;
    uint8 request;
    uint8 retry;
    uint16 backoff;
    TiFunEventHandler listener;
    void * lisowner;
    TiIoBuf * rxbuf;
    TiIoBuf * txbuf;
    TiIoBuf * rtsbuf;
    TiIoBuf * ctsbuf;
	uint8 option;
	char rxbufmem[MACA_RXBUF_MEMSIZE];
	char txbufmem[MACA_TXBUF_MEMSIZE];
	char rtsmem[36];
	char ctsmem[36];
}TiMACA;

TiMACA * maca_construct( char * buf, uintx size );
void maca_destroy( TiMACA * mac );
TiMACA * maca_open( TiMACA * mac, TiFrameTxRxInterface * trx, void * trximpl, 
    TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner );
void maca_close( TiMACA * mac );

uintx maca_send( TiMACA * mac, TiIoBuf * iobuf, uint8 option );
uintx maca_broadcast( TiMACA * mac, TiIoBuf * iobuf, uint8 option );
uintx maca_recv( TiMACA * mac, TiIoBuf * iobuf, uint8 option );
void  maca_evolve( void * macptr, TiEvent * e );

#define maca_setlocaladdr(mac,addr) mac->trx->setlocaladdr(mac->trx,addr)
#define maca_setremoteaddr(mac,addr) mac->trx->setremoteaddr(mac->trx,addr)
#define maca_setpanid(mac,pan) mac->trx->setpanid(mac->trx,pan)
#define maca_setchannel(mac,chn) mac->trx->setchannel(mac->trx,chn)
#define maca_ischannelclear(mac) mac->trx->ischannelclear(mac->trx)


#ifdef __cplusplus
}
#endif

#endif
