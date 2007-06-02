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
#include "svc_configall.h"
#include "svc_foundation.h"
#include <../hal/hal_cc2420.h>
#include "svc_wlscomm.h"

/*
#define WLS_VERSION_TEST
#undef WLS_VERSION_TEST

#undef WLS_VERSION_ARQ
#define WLS_VERSION_ARQ
*/

#undef PHY_ACK_SUPPORT
#define PHY_ACK_SUPPORT 

/* Q: leaved problems
 * R: 如果需要CRC校验的话，应该在这个模块中完成，但cc2420已经帮我们做了CRC。我的问题是：
 * 如果cc2420 CRC校验失败，它会怎么做？直接丢掉吗？我们的程序怎么反应？
 */

#define WLS_MAC_HEADER_LENGTH 7

/* ACK/NAK frame template
 * they are pre-initialized here to improve performance. 
 * 
 * 802.15.4 ACK frame format:
 * [2B Frame Control] [1B Sequence Number][2 FCS]
 */	
#define WLS_ACK_LENGTH 7
static m_ack_template[7] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static void _parse_beacon_frame( TBeaconFrame * frame, char * buf );
static void _parse_data_frame( TDataFrame * frame, char * buf, uint8 len );
static void _parse_ack_frame( TAckFrame * frame, char * buf );
static void _parse_command_frame( TCommandFrame * frame, char * buf, uint8 len );

TWirelessComm * wls_construct( char * buf, uint16 size, TRfDriver * rf, TActionScheduler * actsche )
{
	uint16 len;
	TWirelessComm * wls;
	
	len = sizeof(TWirelessComm);
	if (len > size)
		wls = NULL;
	else
		wls = (TWirelessComm *)buf;
	
	if (wls != NULL)
	{
		memset( buf, 0x00, len );
		wls->state = WLS_STATE_IDLE;
		wls->nextstate = WLS_STATE_IDLE;
		wls->panid = 1;
		wls->localaddress = 1;
		wls->rmtaddress = 1;
		wls->rfdriver = rf;
		wls->txlen = 0;
		wls->rxlen = 0;
		//wls->txindex = 0;
		wls->txframe = &( wls->txmem[0] );
		wls->rxframe = &( wls->rxmem[0] );
		wls->txbuf = &( wls->txmem[WLS_MAC_HEADER_LENGTH] ); // the first 7 bytes are 2 frame control, 1 sequence, 4 address
		wls->rxbuf = &( wls->rxmem[WLS_MAC_HEADER_LENGTH] );
		memset( (char*)(wls->txmem[0]), 0x00, WLS_FRAMEBUFFER_SIZE );
		memset( (char*)(wls->rxmem[0]), 0x00, WLS_FRAMEBUFFER_SIZE );
		wls->retry = 0;
		wls->actsche = actsche;
		wls->waitack_action = ACTSCHE_INVALID_ACTION_ID;
	}
	
	return wls;
}

void wls_destroy( TWirelessComm * wls )
{
	if (wls != NULL)
	{
		if (wls->waitack_action != ACTSCHE_INVALID_ACTION_ID)
			acts_cancelaction( wls->actsche, wls->waitack_action );	
			
		//cc2420_shutdown( wls->rfdriver );
		wls->txlen = 0;
		//wls->txindex = 0;
		wls->rxlen = 0;
	}
}

/* 配置Wireless Communication模块的有关参数，例如无线通信的地址 
 */
void wls_configure( TWirelessComm * wls, uint16 panid, uint16 localaddress, uint16 rmtaddress )
{
	wls->panid = panid;
	wls->localaddress = localaddress;
	wls->rmtaddress = rmtaddress;
		
	cc2420_configure( wls->rfdriver, CC2420_CONFIG_PANID, wls->panid, 2 );
 	cc2420_configure( wls->rfdriver, CC2420_CONFIG_LOCALADDRESS, wls->localaddress, 2 );
	cc2420_configure( wls->rfdriver, CC2420_CONFIG_APPLY, NULL, 0 );
}

/* accept a frame in the buffer. 
 * generally, you should call accept() before read() to get the panid and address
 * information. thus you can do some processing acoording to their address.
 *  
 * @attention
 * 	for current version, you should call read() immediately after accept(). this
 * is because the interrupt service may override the frame in the buffer.
 *  for frame based communication only. for stream mode, it simply return 0
 * 
 * @return
 * 	0	if no frame received into the buffer
 * 	1	if frame arrived. that means you can call read() or rawread() to get it. 
 * @param
 * 	* panid		contains the PAN id of in the buffer
 *  * addr		contains the address of the frame in the buffer
 */
uint8 wls_accept( TWirelessComm * wls, uint16 * panid, uint16 * addr )
{
	uint8 ret;
	
	wls_evolve( wls );
	if ((wls->rxlen > 0) && (wls->state == WLS_STATE_IDLE))
	{
		memmove( panid, &(wls->rxframe[2]), 2 );
		memmove( addr, &(wls->rxframe[4]), 2 );
		ret = 1;
	}
	else
		ret = 0;
	
	return ret;
}

/* stream read interface
 * read received data from TWirelessComm object's internal buffer. these data
 * are placed by wls_evovle() function
 * 
 * @attention:
 * you should not mix using read()/write() and rawread()/rawwrite(). mix-using may 
 * return wrong data.
 */
uint8 wls_read( TWirelessComm * wls, char * payload, uint8 size, uint8 opt )
{
	uint8 copied;

	wls_evolve( wls );
	if (wls->state == WLS_STATE_IDLE)
	{
		copied = min( size, wls->rxlen );
		if (copied > 0)
		{
			memmove( payload, wls->rxbuf, copied );
			memmove( wls->txbuf, (char*)(wls->txbuf + copied), wls->rxlen - copied );
			wls->rxlen -= copied;
		}
	}
	
	return copied;
}

/* frame read interface.
 * read received data from TWirelessComm object's internal buffer. these data
 * are placed by wls_evovle() function. this function assume all the data in the 
 * buffer are the entire payload of the frame.
 */
uint8 wls_rawread( TWirelessComm * wls, char * frame, uint8 size, uint8 opt )
{
	uint8 copied = 0;
	
	wls_evolve( wls );
	if (wls->state == WLS_STATE_IDLE)
	{
		copied = min( size, wls->rxlen + WLS_MAC_HEADER_LENGTH );
		if (copied > 0)
		{
			memmove( frame, wls->rxframe, copied );
			memmove( &(wls->rxframebak[0]), wls->rxframe, WLS_MAC_HEADER_LENGTH ); // backup the current 
														  // frame header for later visiting
			// if the external "frame" buffer is not large enough to hold the 
			// whole frame, then just drop the additional data. this behavior is
			// different to read() function. in read() function:
			// memmove( wls->txbuf, (char*)(wls->txbuf + copied), wls->rxlen - copied );
			// wls->rxlen -= copied;
			wls->rxlen = 0;
		}
	}
		
	return copied;
}

/* stream write interface
 * this function will write data to "TWirelessComm"'s internal buffer, and then 
 * the wls_evolve() function will send them out.
 */
uint8 wls_write( TWirelessComm * wls, char * payload, uint8 len, uint8 opt )
{
	uint8 copied;
	
	if (wls->state == WLS_STATE_IDLE)
	{
		copied = min( WLS_FRAMEBUFFER_SIZE - wls->txlen - WLS_MAC_HEADER_LENGTH, len );
		if (copied > 0)
		{
			memmove( (char*)(wls->txbuf) + wls->txlen, payload, copied );
			wls->txlen += copied;
			wls->nextstate = WLS_STATE_SENDING;
		}
	}
	wls_evolve( wls );

	return copied;
}

/* frame write interface
 * this function will write the whole frame payload to "TWirelessComm"'s internal 
 * buffer, and then the wls_evolve() function will send them out.
 */
uint8 wls_rawwrite( TWirelessComm * wls, char * frame, uint8 len, uint8 opt )
{
	uint8 copied = 0;
	
	if ((wls->state == WLS_STATE_IDLE) && (wls->txlen == 0))
	{
		copied = min( WLS_FRAMEBUFFER_SIZE, len );
		if (copied > 0)
		{
			memmove( (char*)(wls->txframe), frame, copied );
			wls->txlen = copied - WLS_MAC_HEADER_LENGTH;
			wls->nextstate = WLS_STATE_SENDING;
		}
	}
	wls_evolve( wls );

	return copied;
}

/*******************************************************************************
 * IDLE is the first state of the state machine.  Then the state machine will 
 * execute state evolution cycling in SENDING, RECVING, SLEEP and other state.  
 * When you call read() or write() in SLEEP mode, the state machine will automatically 
 * wakeup.
 ******************************************************************************/
#ifndef PHY_ACK_SUPPORT 
int8 wls_evolve( TWirelessComm * wls )
{
	boolean done = TRUE;
	uint8 count;
	int8 ret = 0;
	
	do{
		switch (wls->state)
		{
		// IDLE is the initial state of the state machine.
		// only in IDLE state, you can call rawread() and rawwrite(), or else
		// they simplely return 0 to indicate execution failure.
		//
		case WLS_STATE_IDLE:
			if (wls->nextstate == WLS_STATE_SENDING)
			{
				wls->state = WLS_STATE_SENDING;
				done = FALSE;
			}
			else if (wls->nextstate == WLS_STATE_SLEEP)
			{
				cc2420_sleep( wls->rfdriver );
				wls->state = WLS_STATE_SLEEP;
			}
			else{
				wls->state = WLS_STATE_RECVING;
				done = FALSE;
			}
			break;
		
		// start try to receving data.
		// if there's no frame received, then go back to IDLE state. this feature
		// enable the user be able to call rawread()/rawwrite() successfully.
		//	
		// @attention	
		//  You should NOT split the RECVING state into RECVING_DATA, RECVING_ACK
		// and other RECVING_XXX state, because the receiver should NOT assume 
		// the strict order these packets. for example, you may receive another 
		// data packets before ACK packets. 
		// 
		// @attention
		//	generally, you should judge whether an entire packet received or not, 
		// and whether the received packet is corrupt or not. these work should be 
		// DONE in MAC layer(in this object). however, the TI/chipcon "cc2420"
		// can help you do this. the cc2420_rawread() will return non-zero only 
		// when an entire frame received!
		//
		// @TODO
		//	如何判断收到的是否是一个损坏的frame? 过去我们可以在MAC增加Checksum，但是现在
		// 这些工作已经在cc2420中完成。cc2420如何向上报告这种情况，因为我们需要这种报告以发送
		// NAK，而不应过度依赖timeout机制，那样response太慢。
		//
		case WLS_STATE_RECVING:
			count = cc2420_rawread( wls->rfdriver, wls->rxframe, WLS_FRAMEBUFFER_SIZE, 0x00 );
			if (count > 0)
			{
				wls->rxlen = count;
				// rxbuf[0-1] is the frame control word. so you can judge the frame 
				// type through this word. this application uses the last two bits
				// to indicate such information:
				//		0x0000 	00b data frame
				//		0x0001  01b ACK frame
				//		0x0003	11b NAK frame
				// 
				// the last bit indicates whether this is a data frame, and the 
				// second bit indicates whether this is a ACK/NAK frame.
				//
				switch (wls->rxbuf[1] & 0x03)
				{
				case 0x00:
					// this is a data frame and do nothing now. just wait for the 
					// master program to retrieve the data out.
					// @warning: the newest incoming frame may override the last 
					// frame. so the mater should retrieve the frame as fast as 
					// possible.
					//
					// @TODO
					// do check sum here
					// if checksum successfully, then return ACK or else NAK
					// do checksum for ACK frame
					
					// @attention: you ACK format must be identical with 802.15.4
					// specification, or the cc2420 receiveer may reject it.
					//
					m_ack_template[1] |= 0x01;
					m_ack_template[2] = wls->rxbuf[2]; // update the sequence number
					m_ack_template[3] = 0x00; // @TODO checksum
					m_ack_template[4] = 0x00;
					// @TODO: checksum calculation!
					// @modified by zhangwei on 20061108
					// 如下这句似乎还是有问题
					// rawwrite如果没有成功呢？
					//cc2420_rawwrite( wls->rfdriver, (char*)(&(m_ack_template[0])), 0x00 );
					cc2420_rawwrite( wls->rfdriver, (char*)(&(m_ack_template[0])),&ack, 0x00 );
					wls->state = WLS_STATE_IDLE;
					break;
					
				case 0x01:
					// ACK frame received. 
					// if the sequence id in the ACK packet equal to or larger than 
					// the sequence id of the frame in the sending buffer.
					// if the sequence id is smaller, then just drop the ACK frame
					// and try to continue read(). 
					if (wls->rxbuf[2] >= wls->txbuf[2])
					{
						wls->retry = 0;
						//wls->txindex = 0;
						wls->txlen = 0;
						acts_cancelaction( wls->actsche, wls->waitack_action );
						wls->waitack_action = ACTSCHE_INVALID_ACTION_ID;
						wls->state = WLS_STATE_IDLE;
					}
					else{
						done = FALSE;
					};
						
					break;
					
				case 0x03:
					// NAK frame received. then try to re-transmission the frame.
					if (wls->rxbuf[2] == wls->txbuf[2])
					{
						wls->retry ++;
						//wls->txindex = 0;
						acts_cancelaction( wls->actsche, wls->waitack_action );
						wls->waitack_action = ACTSCHE_INVALID_ACTION_ID;
						wls->nextstate = WLS_STATE_SENDING;
						done = FALSE;
					}
					else{
						done = FALSE;
					}
					break;
				}
			}
			else{
				wls->state = WLS_STATE_IDLE;
			}
			break;
		
		// try to send data only. 
		case WLS_STATE_SENDING:
			if (wls->txlen == 0)
			{
				wls->state = WLS_STATE_IDLE;
			}
			else if (wls->retry > 3)
			{
				// this case should better not to happen! because this means a frame
				// relly lost during transmission. your master application should 
				// consider this problem!
				if (wls->waitack_action != ACTSCHE_INVALID_ACTION_ID)
				{
					acts_cancelaction( wls->actsche, wls->waitack_action );
					wls->waitack_action = ACTSCHE_INVALID_ACTION_ID;
				}
				wls->txlen = 0;
				//wls->txindex = 0;
				wls->nextstate = WLS_STATE_IDLE;
			}
			else{
				//wls->txindex = 0;
				memset( &(wls->txframe[0]), 0x00, WLS_MAC_HEADER_LENGTH );
				wls->txframe[0] = 0x01; // 2 control byte 
				wls->txframe[1] = 0x00; 
				wls->txframe[2] = wls->sequence ++;
				memmove( &(wls->txframe[3]), &(wls->panid), 2 );
				memmove( &(wls->txframe[5]), &(wls->rmtaddress), 2 );
				count = cc2420_rawwrite( wls->rfdriver, wls->txframe ,&ack,0 );
				if (count > 0)
				{
					wls->waitack_action = acts_inputaction( wls->actsche, NULL, NULL, wls->waitack_duration );
					//wls->txlen -= count;
					wls->txlen = 0;
					wls->nextstate = WLS_STATE_WAITACK;
					done = FALSE;
				} 
			}
			break;
			
		case WLS_STATE_WAITACK:
			// wait for ACK frame
			// when acts_expired() return TRUE, it will automatically cancel the 
			// pre-defined action. this feature will not lead to resource leakage.
			// 
			if (acts_expired(wls->actsche, wls->waitack_action))
			{
				wls->waitack_action = ACTSCHE_INVALID_ACTION_ID;
				wls->retry ++;
				//wls->txindex = 0;
				wls->state = WLS_STATE_SENDING;
				done = FALSE;
			}
			else{ 
				wls->state = WLS_STATE_RECVING;
				done = FALSE;
			}
			break;

		case WLS_STATE_SLEEP:
			if (wls->nextstate != WLS_STATE_SLEEP)
			{ 
				cc2420_wakeup( wls->rfdriver );
				wls->state = WLS_STATE_IDLE;
				done = FALSE;
			}
			break;			
		}
	}while (!done);				

	return ret;
}
#endif

#ifdef PHY_ACK_SUPPORT
int8 wls_evolve( TWirelessComm * wls )
{
	boolean done = TRUE;
	uint8 count;
	int8 ret = 0;

	do{
		switch (wls->state)
		{
		// IDLE is the initial state of the state machine.
		// only in IDLE state, you can call rawread() and rawwrite(), or else
		// they simplely return 0 to indicate execution failure.
		//
		case WLS_STATE_IDLE:
			if (wls->nextstate == WLS_STATE_SENDING)
			{
				wls->state = WLS_STATE_SENDING;
				done = FALSE;
			}
			else if (wls->nextstate == WLS_STATE_SLEEP)
			{
				//cc2420_sleep( wls->rfdriver );
				wls->state = WLS_STATE_SLEEP;
			}
			else{
				wls->state = WLS_STATE_RECVING;
				done = FALSE;
			}
			break;
		
		// start try to receving data.
		// if there's no frame received, then go back to IDLE state. this feature
		// enable the user be able to call rawread()/rawwrite() successfully.
		//	
		// @attention
		//	generally, you should judge whether an entire packet received or not, 
		// and whether the received packet is corrupt or not. these work should be 
		// DONE in MAC layer(in this object). however, the TI/chipcon "cc2420"
		// can help you do this. the cc2420_rawread() will return non-zero only 
		// when an entire frame received!
		//
		case WLS_STATE_RECVING:
			count = cc2420_rawread( wls->rfdriver, wls->rxframe, WLS_FRAMEBUFFER_SIZE, 0x00 );
			if (count > 0)
			{
				wls->rxlen = count;
			}
			wls->state = WLS_STATE_IDLE;
			break;
		
		// try to send data only. 
		case WLS_STATE_SENDING:
			if (wls->txlen == 0)
			{
				wls->state = WLS_STATE_IDLE;
			}
			else if (wls->retry > 3)
			{
				// this case should better not to happen! because this means a frame
				// relly lost during transmission. your master application should 
				// consider this problem!
				if (wls->waitack_action != ACTSCHE_INVALID_ACTION_ID)
				{
					acts_cancelaction( wls->actsche, wls->waitack_action );
					wls->waitack_action = ACTSCHE_INVALID_ACTION_ID;
				}
				wls->txlen = 0;
				wls->nextstate = WLS_STATE_IDLE;
			}
			else if (wls->retry == 0)
			{
				memset( &(wls->txframe[0]), 0x00, WLS_MAC_HEADER_LENGTH );
				wls->txframe[0] = 0x01; // 2 control byte 
				wls->txframe[1] = 0x00; 
				wls->txframe[2] = wls->sequence ++;
				memmove( &(wls->txframe[3]), &(wls->panid), 2 );
				memmove( &(wls->txframe[5]), &(wls->rmtaddress), 2 );
				wls->nextstate = WLS_STATE_SENDING;
				done = FALSE;
			}
			else if (acts_expired(wls->actsche, wls->waitack_action))
			{
				wls->nextstate = WLS_STATE_SENDING;
				done = FALSE;
			}
			break;
			
		case WLS_STATE_SENDIMMEDIATE:
			count = cc2420_rawwrite( wls->rfdriver, wls->txframe,12, 0 );  //12 is len, please modify
			if (count > 0)
			{
				if (wls->waitack_action == ACTSCHE_INVALID_ACTION_ID)
				{
					acts_cancelaction( wls->actsche, wls->waitack_action );
					wls->waitack_action = ACTSCHE_INVALID_ACTION_ID;
				}
				wls->txlen = 0;
				wls->nextstate = WLS_STATE_IDLE;
			} 
			else if (count == 0)
			{
				wls->nextstate = WLS_STATE_SENDIMMEDIATE;
			}
			else{
				wls->waitack_duration =  wls->waitack_duration << 1;
				wls->waitack_action = acts_inputaction( wls->actsche, NULL, NULL, wls->waitack_duration );
				wls->nextstate = WLS_STATE_SENDING;
				done = FALSE;
			}
			break;

		case WLS_STATE_SLEEP:
			if (wls->nextstate != WLS_STATE_SLEEP)
			{ 
				cc2420_wakeup( wls->rfdriver );
				wls->state = WLS_STATE_IDLE;
				done = FALSE;
			}
			break;			
		}
	}while (!done);				

	return ret;
}
#endif

uint8 wls_mode( TWirelessComm * wls )  
{
	uint8 mode;
	
	switch (wls->state)
	{
	case WLS_STATE_SLEEP:
		mode = WLS_MODE_SLEEP;
		break;
	case WLS_STATE_IDLE:
	case WLS_STATE_RECVING:
		mode = WLS_MODE_RECVING;
		break;
	case WLS_STATE_SENDING:
	case WLS_STATE_WAITACK:
		mode = WLS_MODE_SENDING;
		break;
	default:
		mode = WLS_MODE_RECVING;
	}
	
	return mode;
}

uint8 wls_state( TWirelessComm * wls )  
{
	return wls->state;
}

uint8 wls_ioresult( TWirelessComm * wls )  
{
	uint8 ioresult = 0x00;
	
	if (wls->txlen > 0)
		ioresult |= 0x01;
	if (wls->rxlen > 0)
		ioresult |= 0x02;
		
	return ioresult;
}

uint8 wls_sleep( TWirelessComm * wls )  
{
	return 0;
}

uint8 wls_wakeup( TWirelessComm * wls )  
{
	if (wls->state != WLS_STATE_SLEEP)
	{
		cc2420_wakeup( wls->rfdriver ); 
	}
	return 0;
}

uint16 wls_getlocaladdress( TWirelessComm * wls )
{
	return wls->localaddress;
}

void wls_setlocaladdress( TWirelessComm * wls, uint16 addr )
{
	wls->localaddress = addr;
	cc2420_configure( wls->rfdriver, CC2420_CONFIG_LOCALADDRESS, wls->localaddress, 2 );
	cc2420_configure( wls->rfdriver, CC2420_CONFIG_APPLY, NULL, 0 );
}

// @attention
//	because the next frame may override the current frame, so the "wlscomm" object 
// save a copy of frame header in rawread(). the remote address is got from this
// backup rather than the original frame buffer. because the frame buffer's address
// may not the frame you have just read().
//
// @return 
//	the frame source node address. the frame is one you just call rawread() to get it out.
//
uint16 wls_getrmtaddress( TWirelessComm * wls )
{
	return wls->rxframebak[6];
}

void wls_setrmtaddress( TWirelessComm * wls, uint16 addr )
{
	wls->rmtaddress = addr;
	wls->txframe[6] = addr;
}

//------------------------------------------------------------------------------
// utility functions
//------------------------------------------------------------------------------

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

void _parse_beacon_frame( TBeaconFrame * frame, char * buf )
{
	frame->framectrl = (uint16 *)buf; buf += 2;
	frame->sequence = (uint8 *)buf; buf += 1;
	frame->address = (uint32 *)buf; buf += 4;
	frame->superframe_spec = (uint16 *)buf; buf += 2;
	frame->gts = NULL;
	frame->address_padding = NULL;
	frame->payload = NULL;
	frame->fcs = NULL;
}

void _parse_data_frame( TDataFrame * frame, char * buf, uint8 len )
{
	char * cursor = buf;
	frame->framectrl = (uint16 *)cursor; cursor += 2;
	frame->sequence = (uint8 *)cursor; cursor += 1;
	frame->address = (uint32 *)cursor; cursor += 4;
	frame->payload = (uint8 *)cursor;
	frame->fcs = (uint16 *)(buf + len - 2);
}

void _parse_ack_frame( TAckFrame * frame, char * buf )
{
	frame->framectrl = (uint16 *)buf; buf += 2;
	frame->sequence = (uint8 *)buf; buf += 1;
	frame->fcs = (uint16 *)buf;
}

void _parse_command_frame( TCommandFrame * frame, char * buf, uint8 len )
{
	char * cursor = buf;
	frame->framectrl = (uint16 *)cursor; cursor += 2;
	frame->sequence = (uint8 *)cursor; cursor += 1;
	frame->address = (uint32 *)cursor; cursor += 4;
	frame->cmdtype = (uint8 *)cursor; cursor += 1;
	frame->payload = (uint8 *)cursor;
	frame->fcs = (uint16 *)(buf + len - 2);
}

#undef WLS_MAC_HEADER_LENGTH
#undef PHY_ACK_SUPPORT

