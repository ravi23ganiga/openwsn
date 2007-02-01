/*****************************************************************************
* This file is part of OpenWSN, the Open Wireless Sensor Network System.
*
* Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
* 
* OpenWSN is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 or (at your option) any later version.
* 
* OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
* WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
* 
* You should have received a copy of the GNU General Public License along
* with eCos; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
* 
* As a special exception, if other files instantiate templates or use macros
* or inline functions from this file, or you compile this file and link it
* with other works to produce a work based on this file, this file does not
* by itself cause the resulting work to be covered by the GNU General Public
* License. However the source code for this file must still be made available
* in accordance with section (3) of the GNU General Public License.
* 
* This exception does not invalidate any other reasons why a work based on
* this file might be covered by the GNU General Public License.
* 
****************************************************************************/ 
#ifndef _WLSCOMM_H_4239_
#define _WLSCOMM_H_4239_

/*******************************************************************************
 * @author zhangwei on 2006-07-20
 * Wireless Communication (Network's MAC Layer)
 * 
 * 该模块构建在TCc2420Driver模块基础之上。TCc2420Driver模块提供cc2420 PHY/MAC芯片的
 * 软件抽象。大致提供PHY层的功能，兼及少量MAC层功能。主要的MAC层功能(例如ARQ)都是在本模块中实现的。
 * 
 * 这种层次化设计是为了最大限度提高系统的硬件无关性。只要提供不同的driver，本模块就可以实现
 * 轻松的移植。
 * 
 * 对RfModem应用，不需要NET层功能，故可以基于本层接口开发
 * 
 * This is a network's MAC layer implementation. Most of the other network services
 * are implemented based on this layer. such as the following:
 * 
 * 		NET layer address and MAC layer address translation (ARP)
 * 		neighbor discovering protocol (NDP)
 * 		network self configuration and re-configuration (NFG)
 * 	 	routing protocol (ROUTING)
 * 		time synchronication...
 * 		localization...
 * 
 * Different to the PHY layer, MAC layer introduced serveral important features:
 * 		frame assembly and de-assembly
 * 		reliable transmission (ARQ)
 * 		frame splitting in the received binary streams 
 * 		frame buffer for packet queuing
 * 		
 * @todo
 * 	- packet queuing in the future.
 * 	you can construct a queue, and place all the packets need to be sent into 
 * this queue, and let the transmission engine (TxEngine) to send them out. 
 * 	this feature may improve the performance. however, it will consume more memory.
 * 
 *  Zhang Wei's Viewpoint: since the memory is a valuable resource in the embedded
 * systems, you'd better choose the current implementation rather than the queue
 * version to decrease the memory usage. low cost should be more important than 
 * high performance for these deep-embedded systems.
 *
 *  
 * @modified by zhangwei on 2006-08-21
 * add some comment today.
 ******************************************************************************/
 
/*******************************************************************************
 * IEEE 802.15.4 PPDU format
 * [4B Preamble][1B SFD][7b Framelength, 1b Reserved][nB PSDU/Payload]
 * 
 * IEEE 802.15.4 MAC DATA format (the payload of PHY frame)
 * Beacon Frame
 * [2B Frame Control] [1B Sequence Number][4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 * 
 * Data Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][n Data Payload][2 FCS]
 * 
 * ACK Frame
 * [2B Frame Control] [1B Sequence Number][2 FCS]
 * 
 * MAC Control Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 ADdress][1 Command Type][n Command Payload][2 FCS]
 * 
 ******************************************************************************/
  
#include "svc_foundation.h"
#include "..\hal\hal_cc2420.h"
#include "svc_actsche.h"

#define TWirelessCommunication TWirelessComm
#define TRfDriver TCc2420Driver

#define WLS_FRAMEBUFFER_SIZE CONFIG_MAX_PHY_FRAME_LENGTH

/* Q: leaved problems
 * R: 如果需要CRC校验的话，应该在这个模块中完成，但cc2420已经帮我们做了CRC。我的问题是：
 * 如果cc2420 CRC校验失败，它会怎么做？直接丢掉吗？我们的程序怎么反应？
 */

typedef struct{
  uint16 * framectrl;
  uint8  * sequence;
  uint32 * address;
  uint16 * superframe_spec;
  uint8  * gts;
  uint8  * address_padding;
  uint8  * payload;
  uint16 * fcs;
}TBeaconFrame;

// #prgama align 1
typedef struct{
  uint16 * framectrl;
  uint8  * sequence;
  uint32 * address;
  uint8  * payload;
  uint16 * fcs;
}TDataFrame;

// #prgama align 1
typedef struct{
  uint16 * framectrl;
  uint8  * sequence;
  uint16 * fcs;
}TAckFrame;

// #prgama align 1
typedef struct{
  uint16 * framectrl;
  uint8  * sequence;
  uint32 * address;
  uint8  * cmdtype;
  uint8  * payload;
  uint16 * fcs;
}TCommandFrame;

/* The TWirelessCommunication object/service, as a whole, has three state:
 * 		IDLE, SENDING, RECVING, SLEEP
 * 
 * while, the object implementation also uses an internal state machine to manage
 * the object's behavior. this state machine also has its own state value. These 
 * two are different. 
 * 		IDLE, SENDING_DATA, WAIT_FOR_ACK, RECVING_ACK, RECVING_DATA, SENDING_ACK,
 * 		SLEEP
 */

enum {WLS_MODE_RECVING=0, WLS_MODE_SENDING, WLS_MODE_SLEEP };
enum {WLS_STATE_IDLE=0, WLS_STATE_RECVING, WLS_STATE_SENDING, WLS_STATE_WAITACK, 
	WLS_STATE_SLEEP, WLS_STATE_SENDIMMEDIATE };
	
/* variable description
 * 	state		state of the wireless object
 * 	address		local network layer address
 * 	rmaddress	remote network layer address
 * 	uniqueid	unique identifier of this node. this is generally a part of the 
 * 				the MAC layer's address. For the current reference design, this
 *				value is got from a Dallas 1-wire unique id component.
 * 	rfdriver	the lower wireless communication driver.
 *  rxlen		RX payload length (less than the frame length)
 *  txlen		TX payload length (less than the frame length)
 * 	timer		the timer object used by TWirelessCommunication object. It is used
 * 				in ARQ.
 *  txqueue
 *  rxqueue		each queue is only an byte array to hold at most 2 frames. 
 * 				they are not full-function queues. they are only ping-pang 
 * 				buffers.
 */ 				
typedef struct{
  uint8 state;
  uint8 nextstate;
  uint16 panid;
  uint16 localaddress;
  uint16 rmtaddress;
  TRfDriver * rfdriver;
  TActionScheduler * actsche;
  uint8 waitack_duration;
  uint8 waitack_action;
  uint8 txlen;  
  uint8 rxlen;
  uint8 sequence;
  uint8 retry;
  //uint8 txindex;
  char * txframe;
  char * rxframe;
  char * txbuf;
  char * rxbuf;
  //char * txqueue;
  //char * rxqueue;
  char rxframebak[7];
  char txmem[ WLS_FRAMEBUFFER_SIZE ];
  char rxmem[ WLS_FRAMEBUFFER_SIZE ];
}TWirelessCommunication;

TWirelessComm * wls_construct( char * buf, uint16 size, TRfDriver * rf, TActionScheduler * actsche );
void  wls_destroy( TWirelessComm * wls );
void wls_configure( TWirelessComm * wls, uint16 panid, uint16 localaddress, uint16 rmtaddress );
uint8 wls_accept( TWirelessComm * wls, uint16 * panid, uint16 * addr );
uint8 wls_read( TWirelessComm * wls, char * payload, uint8 size, uint8 opt );
uint8 wls_rawread( TWirelessComm * wls, char * frame, uint8 size, uint8 opt );
uint8 wls_write( TWirelessComm * wls, char * payload, uint8 len, uint8 opt );
uint8 wls_rawwrite( TWirelessComm * wls, char * frame, uint8 len, uint8 opt );
int8  wls_evolve( TWirelessComm * wls );
uint8 wls_mode( TWirelessComm * wls );
uint8 wls_state( TWirelessComm * wls );
uint8 wls_ioresult( TWirelessComm * wls );
uint8 wls_sleep( TWirelessComm * wls );
uint8 wls_wakeup( TWirelessComm * wls );
uint16 wls_getlocaladdress( TWirelessComm * wls );
void wls_setlocaladdress( TWirelessComm * wls, uint16 addr );
uint16 wls_getrmtaddress( TWirelessComm * wls );
void wls_setrmtaddress( TWirelessComm * wls, uint16 addr );

#endif

