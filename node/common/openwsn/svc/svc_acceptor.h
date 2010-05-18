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

#ifndef _SVC_NETWORKACCEPTOR_H_8432_
#define _SVC_NETWORKACCEPTOR_H_8432_

/* TiWirelessCommunication / TiWlsComm
 * This service enhances the fundamental hardware adapter such as TiCc2420Adapter by:
 *	- Provide an frame queue for frame buffering
 *	- Provide high level send()/recv() function and listening mechanism. But all
 *    these are running in non-interrupt mode. So you can freely use them.
 *    While, the TiCc2420Adapter's listener is actually running in interrupt mode.
 * 
 * The design of this service is affected greatly by Apache MINA Project:
 *	- http://mina.apache.org/
 *  - http://mina.apache.org/conferences.data/Mina_in_real_life_ASEU-2009.pdf
 * 
 * The architecture of MINA is:
 *	Remote Peer ~~~ IoService -- IoFilterChain{IoFilters} -- IoHandler
 *  IoService := [IoAcceptor + IoProcessor] | [IoConnector]
 * 
 * We simplified MINA's architecture by eliminating the processor part.
 */


/* Default settings: use TiAloha as the data adapter */
#define CONFIG_ACCEPTER_ALOHA

#include "svc_configall.h"
#include "../rtl/rtl_openframe.h"
#include "../rtl/rtl_framequeue.h"
#include "svc_foundation.h"

#ifdef CONFIG_ACCEPTER_CC2420
#include "../hal/hal_cc2420.h"
#endif

#ifdef CONFIG_ACCEPTER_ALOHA
#include "svc_aloha.h"
#endif

#ifdef CONFIG_ACCEPTER_OPENMAC
#include "svc_openmac.h"
#endif

#ifdef CONFIG_ACCEPTER_OPENLLC
#include "svc_openllc.h"
#endif

/* Configure which data adapter to use by the acceptor 
 * 
 * - CONFIG_ACCEPTER_CC2420
 *   use TiCc2420Adapter 
 * 
 * - CONFIG_ACCEPTER_ALOHA
 *   use TiAloha 
 */
#define CONFIG_ACCEPTER_CC2420
#define _TiNetworkAdapter TiCc2420Adapter
#define _adt_recv(obj,buf,size,opt) cc2420_recv(obj,buf,size,opt)
#define _adt_send(obj,buf,len,opt) cc2420_send(obj,buf,len,opt)
#endif

#define CONFIG_ACCEPTER_ALOHA
#define _TiNetworkAdapter TiAloha
#define _adt_recv(obj,buf,size,opt) aloha_recv(obj,buf,size,opt)
#define _adt_send(obj,buf,len,opt) aloha_send(obj,buf,len,opt)
#endif

#define CONFIG_ACCEPTER_OPENMAC
#define _TiNetworkAdapter TiOpenMAC
#define _adt_recv(obj,buf,size,opt) mac_recv(obj,buf,size,opt)
#define _adt_send(obj,buf,len,opt) mac_send(obj,buf,len,opt)
#endif

#define CONFIG_ACCEPTER_OPENLLC
#define _TiNetworkAdapter TiOpenLLC
#define _adt_recv(obj,buf,size,opt) llc_recv(obj,buf,size,opt)
#define _adt_send(obj,buf,len,opt) llc_send(obj,buf,len,opt)
#endif
#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
	uint8 state;
	_TiNetworkAdapter * adapter;
	TiFrameQueue * rxque;
	TiFrameQueue * txque;
	TiOpenFrame * txframe;
    TiIoHandler iohandler;
	char rxbufmem[OPF_SUGGEST_SIZE];
}TiNetworkAcceptor;

TiNetworkAcceptor * nac_construct( void * mem, uint16 size );
void nac_destroy( TiNetworkAcceptor * nac );
TiNetworkAcceptor * nac_open( TiNetworkAcceptor * nac, _TiNetworkAdapter * adt, TiFilterChain * filters );
void nac_close( TiNetworkAcceptor * nac );
void nac_evolve( TiNetworkAcceptor * nac, TiEvent * e );
#ifdef __cplusplus
}
#endif
#endif
