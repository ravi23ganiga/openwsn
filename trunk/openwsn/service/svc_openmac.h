#ifndef _OPENMAC_H_7248_
#define _OPENMAC_H_7248_

/*******************************************************************************
 * @author zhangwei on 2006-07-20
 * OpenMAC
 * an distributed medium access layer. this layer is kind of ALOHA and use 
 * backoff to avoid collision.
 ******************************************************************************/
  
#include "..\hal\hal_foundation.h"
#include "..\hal\hal_cc2420.h"
#include "..\hal\hal_timer.h"
#include "svc_foundation.h"
#include "svc_actsche.h"

#define MAC_FRAMEBUFFER_SIZE CONFIG_MAX_MAC_FRAME_LENGTH
#define TOpenMAC TMediumAccess


enum {MAC_STATE_IDLE=0, MAC_STATE_SLEEP, MAC_STATE_RECVING, MAC_STATE_SENDING, MAC_STATE_WAIT_ACK,
	MAC_STATE_WAIT_SENDRTS, MAC_STATE_WAIT_CTS };
	
typedef struct{
  uint8 id;
}TMacFrame;

typedef struct{
  uint8 state;
  uint8 nextstate;
  TCc2420Driver * phy;
  uint8 retry; // 已经retry的次数
  uint8 retry_limit; // 最大允许的retry次数，通过configure配置，default = 3
  uint8 seqno; // sequence number, 按照15.4规定
  uint8 txlen;
  uint8 rxlen;
  char * txbuf;
  char * rxbuf;
  char * ackbuf;
  uint8 backoff;
  uint8 backoff_rule;
  char txframe[255];
  char rxframe[255];
  char rxframebak[7];
}TMediumAccess;  

TMediumAccess * mac_construct( char * buf, uint16 size, TCc2420Driver * phy,  
	TActionScheduler * actsche );
void  mac_destroy( TMediumAccess * mac );
void  mac_configure( TMediumAccess * mac, uint8 ctrlcode, uint8 value );

//uint8 mac_accept( TMediumAccess * mac, uint16 * panid, uint16 * addr );
//uint8 mac_read( TMediumAccess * mac, char * payload, uint8 size, uint8 opt );
uint8 mac_read( TMediumAccess * mac, TMacFrame * frame, uint8 opt );
uint8 mac_rawread( TMediumAccess * mac, char * frame, uint8 size, uint8 opt );
//uint8 mac_write( TMediumAccess * mac, char * payload, uint8 len, uint8 opt );
uint8 mac_write( TMediumAccess * mac, TMacFrame * frame, uint8 opt );
uint8 mac_rawwrite( TMediumAccess * mac, char * frame, uint8 len, uint8 opt );

//uint8 mac_setrmtaddress( TMediumAccess * mac, uint32 addr );
uint8 mac_setlocaladdress( TMediumAccess * mac, uint32 addr );
//uint8 mac_getrmtaddress( TMediumAccess * mac, uint32 * addr );
uint8 mac_getlocaladdress( TMediumAccess * mac, uint32 * addr );
int8  mac_evolve( TMediumAccess * mac );

//uint8 mac_mode( TMediumAccess * mac );
//uint8 mac_state( TMediumAccess * mac );
//uint8 mac_ioresult( TMediumAccess * mac );
uint8 mac_sleep( TMediumAccess * mac );
uint8 mac_wakeup( TMediumAccess * mac );
uint8 mac_installnotify( TMediumAccess * mac, TEventHandler * callback, void * data );

#endif