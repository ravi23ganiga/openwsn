#ifndef _SVC_NIO_ALOHA_H_4829_
#define _SVC_NIO_ALOHA_H_4829_
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

/**
 * CONFIG_ALOHA_MAX_BACKOFF
 * Maximum backoff delay time. the really backoff time is a random number between 
 * 0 and CONFIG_ALOHA_MAX_BACKOFF. Currently, it's set to 100 milliseconds. You should 
 * optimize it according to your own network parameters.
 * 
 * CONFIG_ALOHA_MIN_ACK_TIME
 * CONFIG_ALOHA_MAX_ACK_TIME
 * These two macros defines the range for ACK waiting. You should tune it according
 * to the transceiver parameters.
 */

#define CONFIG_ALOHA_DEFAULT_PANID				0x0001
#define CONFIG_ALOHA_DEFAULT_CHANNEL            11
#define CONFIG_ALOHA_BROADCAST_ADDRESS          0xFFFF

#define CONFIG_ALOHA_MAX_RETRY                  3
#define CONFIG_ALOHA_ACK_RESPONSE_TIME          10

#define CONFIG_ALOHA_MAX_FRAME_SIZE             128


#define CONFIG_ALOHA_MIN_ACK_TIME               5
#define CONFIG_ALOHA_MAX_ACK_TIME               200

#define CONFIG_ALOHA_MAX_BACKOFF                100
#define CONFIG_ALOHA_MIN_BACKOFF                0

#define CONFIG_ALOHA_STANDARD

#include "svc_configall.h"
#include "svc_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_debugio.h"
#include "../hal/hal_frame_transceiver.h"
#include "../hal/hal_timer.h"
#include "svc_timer.h"
#include "svc_nio_acceptor.h"

/******************************************************************************* 
 * svc_aloha.c
 * Standard ALOHA medium access control (MAC) protocol.
 * Thought it's called standard, this implementation has an special feature comparing 
 * to standard aloha protocol on that: it will insert a random delay before each 
 * sending in order to decrease wakeup sending conflictions.
 * 
 * @todo
 * you should replace the infinite delay when waiting for ACK frame with timer
 * driven delay in the future!!
 *
 * you may need to add seqid management in MAC layer!
 * 
 * @status
 *	- in testing
 * 
 * @modified by zhangwei on 2010.05.10
 *  - the original aloha module was divided into two branches: simple aloha and 
 *    standard aloha. the simple aloha doesn't use timer so it's easy for demonstration. 
 *    and the standard aloha use timer to control the delay time and waiting time,
 *    thus it's more reasonable and practical. 
 *  - simple aloha and standard aloha are designed to share the same interface.
 * @modified by zhangwei on 2010.08.23
 *	- revision
 * @modified by zhangwei on 2010.08.24
 *  - correct bug in aloha_ischannelclear(). the return value should be bool type
 *
 ******************************************************************************/  

/**
 * @reference
 * - ALOHAnet, http://en.wikipedia.org/wiki/Aloha_protocol#The_ALOHA_protocol;
 */

#define ALOHA_OPTION_ACK                    0x00
#define ALOHA_DEF_OPTION                    0x00

/* standard aloha protocol state:
 * - IDLE: wait for sending and receiving. since the receiving is a fast process
 *         then i give up design a special state for receiving. 
 *         receiving process can be occur in any state.
 * - WAITFOR_SENDING: wait for sending 
 * - SLEEP: sleep mode.
 */

#define ALOHA_STATE_NULL                    0
#define ALOHA_STATE_IDLE                    1
#define ALOHA_STATE_BACKOFF                 2
#define ALOHA_STATE_SLEEPING                3

typedef struct{
	uint8 state;
    TiFrameTxRxInterface * rxtx;
	TiNioAcceptor *nac;
	//TiTimerAdapter * timer;
	TiTimer * timer;
	TiFrame * txbuf;
    uint8 retry;
	uint16 backoff;
    uint16 panto;
    uint16 shortaddrto;
    uint16 panfrom;
    uint16 shortaddrfrom;
    uint8 seqid;
    uint8 sendoption;
    uint8 sendfailed;
    TiIEEE802Frame154Descriptor desc;
    TiFunEventHandler listener;
    void * lisowner;
	uint8 option;
	char txbuf_memory[FRAME_HOPESIZE(CONFIG_ALOHA_MAX_FRAME_SIZE)];
	char rxbuf_ack[FRAME154_ACK_FRAME_SIZE];
	uintx success;//todo
}TiAloha;

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Construct an TiNioAloha object inside the specified memory block. Attention the memory 
 * block must be large enough to hold the object.
 */
TiAloha * aloha_construct( char * buf, uint16 size );

/** 
 * Destroy the TiNioAloha object.
 */
void aloha_destroy( TiAloha * mac );

/**
 * Open the TiNioAloha object for sending and receiving. 
 */
TiAloha * aloha_open( TiAloha * mac, TiFrameTxRxInterface * rxtx, TiNioAcceptor * nac, uint8 chn, uint16 panid, 
	uint16 address, TiTimer * timer, TiFunEventHandler listener, void * lisowner, uint8 option );

/**
 * Close the TiNioAloha object. The closing process will release resource applied by 
 * the aloha object. 
 */
void aloha_close( TiAloha * mac );

/**
 * \brief aloha_send() will send the frame object with the option. This function will
 * duplicate a frame object inside the MAC object, so the original input frame keeps
 * unchanged.
 *
 * @param mac mac component
 * @param frame the frame object to be sent
 * @param option sending option. controls the ACK behavior currently. please refer to 
 *   the transceiver's driver interface. 
 *      current settings: if bit 0 of option is 1, then this function will request ACK 
 *   from the receiver. the ack is handled inside mac or transceiver object.
 * 
 * @return
 *  if the frame can be accepted successfully, then this function will return positive
 * value. however, theoretically speaking, the hardware may haven't really send the 
 * frame out. you should guarantee to repeated call aloha_evolve() to try to send the
 * frame out if this occurs.
 */
uintx aloha_send( TiAloha * mac, uint16 shortaddrto, TiFrame * frame, uint8 option );

/**
 * \brief This function broadcast a frame out.
 */
uintx aloha_broadcast( TiAloha * mac, TiFrame * frame, uint8 option );

/**
 * \brief This function try to receive a frame and place it into parameter "frame".
 * If there's no frame coming, then returns 0.
 */
uintx aloha_recv( TiAloha * mac, TiFrame * frame, uint8 option );


/**
 * \brief Evolve the state machine of aloha protocol.
 */
void aloha_evolve( void * macptr, TiEvent * e );

inline void aloha_setlocaladdress( TiAloha * mac, uint16 addr )
{
    mac->rxtx->setshortaddress( mac->rxtx->provider, addr );
}

inline void aloha_setremoteaddress( TiAloha * mac, uint16 addr )
{
	mac->shortaddrto = addr;
}

inline void aloha_setpanid( TiAloha * mac, uint16 pan )
{
    mac->rxtx->setpanid( mac->rxtx->provider, pan );
    mac->panto = pan;
	mac->panfrom = pan;
}

inline void aloha_setchannel( TiAloha * mac, uint8 chn )
{
    mac->rxtx->setchannel( mac->rxtx->provider, chn );
}

// todo
inline bool aloha_ischannelclear( TiAloha * mac )
{
    // return (mac->rxtx->ischnclear == NULL) ? true : mac->rxtx->ischnclear( mac->rxtx->provider );
    return true;
}

void aloha_statistics( TiAloha * mac, uint16 * sendcount, uint16 * sendfailed );

#ifdef __cplusplus
}
#endif

#endif /* _SVC_NIO_ALOHA_H_4829_ */

