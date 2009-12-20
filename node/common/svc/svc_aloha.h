/****************************************************************************** 
 * svc_aloha.c
 * ALOHA medium access control (MAC) protocol 
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
 *****************************************************************************/  

#ifndef _SVC_ALOHA_H_4829_
#define _SVC_ALOHA_H_4829_

#define CONFIG_ALOHA_BASED_ON_CC2420

/* The "aloha" MAC protocol implementation requires the following interface functions 
 * of the wireless transceiver:
 *	- setlocaladdress;
 *	- setremoteaddress;
 *	- setpanid;
 *  - setchannel;
 *  - ischannelclear;
 *  - send;
 *  - write;
 *  - sleep;
 *  - wakeup;
 */

#define CONFIG_ALOHA_DEFAULT_PANID				0x0001
#define CONFIG_ALOHA_DEFAULT_CHANNEL            11
#define CONFIG_ALOHA_BROADCAST_ADDRESS          0xFFFF

#define CONFIG_ALOHA_MAX_RETRY                  3
#define CONFIG_ALOHA_ACK_RESPONSE_TIME          10

/* initial backoff time if encountered confliction */

#define CONFIG_ALOHA_BACKOFF                    10

#include "svc_configall.h"
#include "../hal/hal_foundation.h"
#include "../hal/hal_timer.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_debugio.h"
#include "../rtl/rtl_openframe.h"

#define ALOHA_OPTION_ACK 0x00
#define ALOHA_DEF_OPTION 0x00

/* TiAloha:
 *	- state
 *    There're 3 different states of TiAloha object: SENDING, RECVING, SLEEP
 */
typedef struct{
	uint8 state;
	TiCc2420Adapter * transceiver;
	TiTimerAdapter * timer;
	uint8 rxlen;
	char rxbuf[CC2420_RXBUFFER_SIZE];
    uint8 retry;
	uint16 backoff;
    TiFunEventHandler listener;
    void * lisowner;
	uint8 option;
}TiAloha;


TiAloha *	aloha_construct( char * buf, uintx size );
void        aloha_destroy( TiAloha * mac );
TiAloha *	aloha_open( TiAloha * mac, TiCc2420Adapter * transceiver, uint8 chn, uint16 panid, 
			uint16 address, TiTimerAdapter * timer, TiFunEventHandler listener, void * lisowner, uint8 option );
void        aloha_close( TiAloha * mac );
uintx       aloha_send( TiAloha * mac, TiOpenFrame * opf, uint8 option );
uintx       aloha_broadcast( TiAloha * mac, TiOpenFrame * opf, uint8 option );
uintx       aloha_recv( TiAloha * mac, TiOpenFrame * opf, uint8 option );
void        aloha_evolve( void * macptr, TiEvent * e );

inline void aloha_setlocaladdress( TiAloha * mac, uint16 addr )
{
	cc2420_setshortaddress(mac->transceiver, addr);
}
/*
inline void aloha_setremoteaddr( TiAloha * mac, uint16 addr )
{
	cc2420_setremoteaddr(mac->transceiver, addr);
}
*/
inline void aloha_setpanid( TiAloha * mac, uint16 pan )
{
	cc2420_setpanid(mac->transceiver, pan);
}

inline void aloha_setchannel( TiAloha * mac, uint8 chn )
{
	cc2420_setchannel(mac->transceiver, chn);
}

inline void aloha_ischannelclear( TiAloha * mac )
{
	cc2420_ischannelclear(mac->transceiver );
}

#endif
