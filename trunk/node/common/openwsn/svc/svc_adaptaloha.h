#ifndef _SVC_ADAPTALOHA_H_4829_
#define _SVC_ADAPTALOHA_H_4829_
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
#include "svc_foundation.h"
#include "../rtl/rtl_iobuf.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_frame_transceiver.h"

/*******************************************************************************
 * svc_adaptaloha
 * adaptive aloha protocol implementation
 * the sending process will automatically tuning its delay duration according to 
 * the current traffic load.
 *
 * @state
 *  in developing
 *
 * @author zhangwei in 200912
 *  - first created
 * @modified by zhangwei on 2010.05.08
 *  - revision. replace original "cc2420.h" with "transceiver.h"
 * 
 ******************************************************************************/

#define CONFIG_ADPTALOHA_DEFAULT_PANID				0x0001
#define CONFIG_ADPTALOHA_DEFAULT_LOCAL_ADDRESS		0x07 
#define CONFIG_ADPTALOHA_DEFAULT_REMOTE_ADDRESS		0x08
#define CONFIG_ADPTALOHA_DEFAULT_CHANNEL            11
#define CONFIG_ADPTALOHA_BROADCAST_ADDRESS          0xFFFF

#define CONFIG_ADPTALOHA_MAX_RETRY                  3
#define CONFIG_ADPTALOHA_ACK_RESPONSE_TIME          10

#define CONFIG_ADPTALOHA_TRX_ACK_SUPPORT

#define ADPTALOHA_STATE_NULL                        0
#define ADPTALOHA_STATE_IDLE                        1
#define ADPTALOHA_STATE_WAITFOR_PHY_SENDING         2
#define ADPTALOHA_STATE_SLEEP                       3
#define ADPTALOHA_STATE_POWERDOWN                   4

#define ADPTALOHA_SLEEP_REQUEST                     1
#define ADPTALOHA_WAKEUP_REQUEST                    2
#define ADPTALOHA_SHUTDOWN_REQUEST                  3
#define ADPTALOHA_STARTUP_REQUEST                   4

#define ADPTALOHA_EVENT_FRAME_ARRIVAL               234 // todo

#ifdef __cplusplus
extern "C"{
#endif

/* 
 * TiAdaptiveAloha is a enhanced version of the fundamental ALOHA medium access
 * protocol. It has the same interface with TiAloha.
 */
    
typedef struct{
    uint8 state;
    TiTimerAdapter * timer;
	TiFrameTxRxInterface * trx;
    uint8 loadfactor;
    uint8 request;
    uint8 retry;
    TiFunEventHandler listener;
    void * lisowner;
    TiIoBuf * rxbuf;
    TiIoBuf * txbuf;
	char rxbufmem[IOBUF_HOPESIZE(128)];
	char txbufmem[IOBUF_HOPESIZE(128)];
}TiAdaptiveAloha; 

TiAdaptiveAloha * adaptive_aloha_construct( char * buf, uintx size );
void adaptive_aloha_destroy( TiAdaptiveAloha * mac );
TiAdaptiveAloha * adaptive_aloha_open( TiAdaptiveAloha * mac, TiFrameTxRxInterface * trx, void * trximpl, 
    TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner );
void adaptive_aloha_close( TiAdaptiveAloha * mac );

uintx adaptive_aloha_send( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option );
uintx adaptive_aloha_broadcast( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option );
uintx adaptive_aloha_recv( TiAdaptiveAloha * mac, TiIoBuf * iobuf, uint8 option );
void adaptive_aloha_evolve( void * macptr, TiEvent * e );

#define aloha_setlocaladdr(mac,addr) mac->trx->setlocaladdr(mac->trx,addr)
#define aloha_setremoteaddr(mac,addr) mac->trx->setremoteaddr(mac->trx,addr)
#define aloha_setpanid(mac,pan) mac->trx->setpanid(mac->trx,pan)
#define aloha_setchannel(mac,chn) mac->trx->setchannel(mac->trx,chn)
#define aloha_ischannelclear(mac) mac->trx->ischannelclear(mac->trx)


#ifdef __cplusplus
}
#endif
#endif
