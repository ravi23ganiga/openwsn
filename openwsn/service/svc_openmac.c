#include "svc_foundation.h"
#include "svc_openmac.h"

#define WLS_ACK_LENGTH 7
static m_ack_template[7] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

TMediumAccess * mac_construct( char * buf, uint16 size, TCc2420Driver * phy,  
	TActionScheduler * actsche );
void  mac_destroy( TMediumAccess * mac );
void  mac_configure( TMediumAccess * mac, uint8 ctrlcode, uint8 value );

// before you call mac_read(), you can call mac_accept() to get the address
// information in the frame header. mac_read() will only return the payload.
//
uint8 mac_accept( TMediumAccess * mac, uint16 * panid, uint16 * addr )
{
	// @TODO 
	return 0;
}
/*
uint8 mac_read( TMediumAccess * mac, char * payload, uint8 size, uint8 opt )
{
	uint8 copied;

	mac_evolve( mac );
	if (mac->state == MAC_STATE_IDLE)
	{
		copied = min( size, mac->rxlen );
		if (copied > 0)
		{
			memmove( payload, mac->rxbuf, copied );
			mac->rxlen -= copied;
			memmove( mac->txbuf, (char*)(mac->txbuf + copied), mac->rxlen );
		}
	}
	
	return copied;
}
*/
uint8 mac_rawread( TMediumAccess * mac, char * frame, uint8 size, uint8 opt )
{
	uint8 copied = 0;
	
	mac_evolve( mac );
	if (mac->state == MAC_STATE_IDLE)
	{
		copied = min( size, mac->rxlen );
		if (copied > 0)
		{
			memmove( frame, mac->rxbuf, copied );
			memmove( &(mac->rxframebak[0]), mac->rxframe, 7 ); // backup the current 
														  // frame header for later visiting
			// if the external "frame" buffer is not large enough to hold the 
			// whole frame, then just drop the left data in the buffer.
			mac->rxlen = 0;
		}
	}
		
	return copied;
}
/*
uint8 mac_write( TMediumAccess * mac, char * payload, uint8 len, uint8 opt )
{
	uint8 copied;
	
	if (mac->state == MAC_STATE_IDLE)
	{
		copied = min( MAC_FRAMEBUFFER_SIZE - mac->txlen, len );
		if (copied > 0)
		{
			memmove( (char*)(mac->txbuf) + mac->txlen, payload, copied );
			mac->txlen += copied;
			mac->nextstate = MAC_STATE_SENDING;
		}
	}
	mac_evolve( mac );

	return copied;
}
*/
uint8 mac_rawwrite( TMediumAccess * mac, char * frame, uint8 len, uint8 opt )
{
	uint8 copied = 0;
	
	if ((mac->state == MAC_STATE_IDLE) && (mac->txlen == 0))
	{
		copied = min( MAC_FRAMEBUFFER_SIZE, len );
		if (copied > 0)
		{
			memmove( (char*)(mac->txbuf), frame, copied );
			mac	->txlen = copied;
			mac->nextstate = MAC_STATE_SENDING;
		}
	}
	mac_evolve( mac );

	return copied;
}

uint8 mac_setrmtaddress( TMediumAccess * mac, uint32 addr );
uint8 mac_setlocaladdress( TMediumAccess * mac, uint32 addr );
uint8 mac_getrmtaddress( TMediumAccess * mac, uint32 * addr );
uint8 mac_getlocaladdress( TMediumAccess * mac, uint32 * addr );


int8  mac_evolve( TMediumAccess * mac )
{
	boolean done = TRUE;
	uint8 count;
	int8 ret = 0;
	
	do{
		switch (mac->state)
		{
		// IDLE is the initial state of the state machine.
		// only in IDLE state, you can call rawread() and rawwrite(), or else
		// they simplely return 0 to indicate execution failure.
		//
		// @note 20061016
		// some one may argue IDLE state is no use. that's really true. however, 
		// it maybe more convenient when you dealing with SLEEP state. 
		//
		case MAC_STATE_IDLE:
			if (mac->nextstate == MAC_STATE_SENDING)
			{
				mac->state = MAC_STATE_SENDING;
				done = FALSE;
			}
			else if (mac->nextstate == MAC_STATE_SLEEP)
			{
				cc2420_sleep( mac->phy );
				mac->state = MAC_STATE_SLEEP;
			}
			else{
				mac->state = MAC_STATE_RECVING;
				done = FALSE;
			}
			break;
		
		// start try to receving data.
		// if there's no frame received, then go back to IDLE state. this feature
		// enable the user be able to call rawread()/rawwrite() successfully.
		//	
		case MAC_STATE_RECVING:
			count = cc2420_rawread( mac->phy, mac->rxframe, MAC_FRAMEBUFFER_SIZE, 0x00 );
			if (count > 0)
			{
				mac->rxlen = count;
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
				switch (mac->rxbuf[1] & 0x03)
				{
				case 0x00:
					// this is a data frame and do nothing now. just wait for the 
					// master program retrieve the data out.
					// @warning: the newest incoming frame may override the last 
					// frame. so the mater should retrieve the frame as fast as 
					// possible.
					//
					// @TODO
					// do check sum here
					// if checksum successfully, then return ACK or else NAK
					// do checksum for ACK frame
					
					m_ack_template[1] |= 0x01;
					m_ack_template[2] = mac->rxbuf[2]; // update the sequence number
					m_ack_template[3] = 0x00; // @TODO checksum
					m_ack_template[4] = 0x00;
					cc2420_rawwrite( mac->phy, (char*)(&(m_ack_template[0])),12, 0x00 );  //12 is len, please modify
					mac->state = MAC_STATE_IDLE;
					break;
					
				case 0x01:
					// ACK frame received. 
					// if the sequence id in the ACK packet equal to or larger than 
					// the sequence id of the frame in the sending buffer.
					if (mac->rxbuf[2] >= mac->txbuf[2])
					{
						mac->retry = 0;
						mac->txlen = 0;
						//acts_cancelaction( mac->actsche, mac->waitack_action );
						mac->state = MAC_STATE_IDLE;
					}
					break;
					
				case 0x03:
					// NAK frame received. then try to re-transmission the frame.
					if (mac->rxbuf[2] == mac->txbuf[2])
					{
						mac->retry ++;
						//acts_cancelaction( mac->actsche, mac->waitack_action );
						mac->nextstate = MAC_STATE_SENDING;
						done = FALSE;
					}
					break;
					
				default:
					NULL;
				}
			}
			else{
				mac->state = MAC_STATE_IDLE;
			}
			break;
		
		// start frame sending process. 
		case MAC_STATE_SENDING:
			if (mac->txlen == 0)
			{
				mac->state = MAC_STATE_IDLE;
			}
			else if (mac->retry > 3)
			{
				// this case should better not to happen! because this means a frame
				// relly lost during transmission. your master application should 
				// consider this problem!
				mac->txlen = 0;
				mac->nextstate = MAC_STATE_IDLE;
			}
			else{
				mac->state = MAC_STATE_WAIT_SENDRTS;
				// 指数退避算法
				mac->backoff = mac->backoff << 2;
				//actsche->inputaction ?;
				done = false;
			}
			break;
			
		case MAC_STATE_WAIT_SENDRTS:
			/*
			// @TODO
			//if actsche_expired() 
			{
				cc2420_rawwrite( mac->phy, RTS );
				mac->state = MAC_STATE_WAIT_CTS;
				start timer
			}
			*/
			break;
			
		case MAC_STATE_WAIT_CTS:
			/* memset(buf)
			count = cc2420_rawread()
			if (count == 0) && (actsche_expired)
			{
				mac->state = MAC_STATE_SENDING;
				计算重新发送RTS的时间
				done = FALSE;
			}
			if (count > 0) && (valid CTS)
			{
				mac->txindex = 0;
				count = cc2420_rawwrite( mac->phy, mac->txbuf,  0 );
				if (count > 0)
				{
					//mac->waitack_action = acts_inputaction( mac->actsche, NULL, NULL, mac->waitack_duration );
					mac->txlen -= count;
					mac->nextstate = MAC_STATE_WAITACK;
					启动等待WAITACK timer
					done = FALSE;
				}
			}
			*/
			break;
			
		case MAC_STATE_WAIT_ACK:
			// wait for ACK frame
			// when acts_expired() return TRUE, it will automatically cancel the 
			// pre-defined action. this feature will not lead to resource leakage.
			//
			/*
			memset(buf)
			count = cc2420_rawread()
			if (count == 0) && (actsche_expired)
			//if (acts_expired(mac->waitack_action))
			if (FALSE)
			{
				mac->retry ++;
				mac->txindex = 0;
				mac->state = MAC_STATE_SENDING;
				done = FALSE;
			}
			if (count > 0) && valid ACK/NAK
			{
				if ACK
					mac->state = MAC_STATE_IDLE;
				else
					mac->state = MAC_STATE_SENDING;
				done = FALSE;
			}*/
			break;

		case MAC_STATE_SLEEP:
			if (mac->nextstate != MAC_STATE_SLEEP)
			{ 
				cc2420_wakeup( mac->phy );
				mac->state = MAC_STATE_IDLE;
				done = FALSE;
			}
			break;			
		}
	}while (!done);				

	return ret;
}

/*

int8 mac_state_machine_evolve( TMediumAccess * mac )
{
	boolean done = TRUE;
	int8 ret = 0;
	
	do{
		switch (mac->transtate)
		{
		case MAC_IDLE_STATE:
			if (mac->nextstate == MAC_RECVING_STATE)
			{
				// @TODO
				// start the data receiving process
				mac->state = MAC_RECVING_STATE;
				done = FALSE;
			}
			else if (mac->nextstate == MAC_SENDING_STATE)
			{
				mac->state = MAC_SENDING_STATE;
				done = FALSE;
			}
			break;
			
		case MAC_RECVING_STATE:
			// @TODO
			// try to receive data from the wireless driver
			// and judge whether an entire frame received
			cc2420_read()
			
			if (entire frame received)
			{
				if DATA frame
				{
					// prepare ACK or NAK frame
					mac->state = MAC_SENDING_ACK
					done = FALSE;
				}
				else if CTS Frame
				{
					mac->state = MAC_SENDING_DATA
					done = FALSE;
				}
				else if ACK frame
				{
					//clear 有关变量
					mac->state = MAC_IDLE;
				}
				else if NAK frame
				{
					mac->state = MAC_SENDING_DATA
					done = FALSE;
				}
			}
			break;
			
		// 这个状态其实是启动发送事务，但是不能立刻发送数据，还要等待一段随机时候后才允许
		// 发送RTS
		case MAC_SENDING_STATE:
			if (mac->retry_count) >= 3
			{
				取消各计时action
				reset各有关变量
				mac->state = MAC_IDLE_STATE;
			}
			else{
				//启动RTS发送延迟，延迟时间根据backoff_rule计算
				// for random backoff
				backoff = random( 0, backoff_rule );
				// for square backoff
				backoff = backoff * 2;
				if (backoff == 0) 立即重传
				{
					设置各变量准备发送;
					mac->state = MAC_SENDING_RTS;
					done = FALSE;
				}
				else{
					mac->rts_delay_action = acts_inputactioin( mac->actsche, 发送RTS延迟 )
					设置各变量准备发送;
					mac->state = MAC_SENDING_RTS:
					done = FALSE;
				}
			}
			 
		case MAC_SENDING_RTS:
			if (RTS发送延迟时间到)
			{
				cc2420_write 发送RTS
				启动CTS等待计时
				mac->state = MAC_WAIT_CTS;
				done = FALSE;
			}
			break;
			
		case MAC_WAIT_CTS:
			cc2420_read 查是否收到CTS
			if yes
			{
				acts_cancel( CTS等待Action );
				mac->state = MAC_SENDING_DATA;
				done = FALSE;
			}
			else if acts_expired(CTS等待计时)
			{
				mac->state = MAC_SENDING_RTS;
				done = FALSE;
			}
			break;
		
		case MAC_SENDING_DATA:
			cc2420_write 发送数据
			if (发送完毕)
			{
				mac->state = MAC_RECVING;
				done = FALSE;
			}
			
		default
			mac->state = MAC_RECVING;
		}

		
	}
	while (!done)
}
*/


uint8 mac_mode( TMediumAccess * mac );
uint8 mac_state( TMediumAccess * mac );
uint8 mac_ioresult( TMediumAccess * mac );
uint8 mac_sleep( TMediumAccess * mac );
uint8 mac_wakeup( TMediumAccess * mac );
uint8 mac_installnotify( TMediumAccess * mac, TEventHandler * callback, void * data );


