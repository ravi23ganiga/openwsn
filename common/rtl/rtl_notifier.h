#ifndef _RTL_NOTIFIER_H_8496_
#define _RTL_NOTIFIER_H_8496_

/* rtl_notifier
 * this is an C language version of notifier object. notifier is used to connect
 * multiple objects and transmit events between sender and receiver.
 *
 * @state
 *	finished. not fully tested	
 *
 * @author zhangwei on 200812
 * @modified by zhangwei on 200903
 *	- revision 
 */ 

#include "rtl_configall.h"
#include "rtl_foundation.h"

/* control whether the notif_post() function can be used in the project 
 * you should include "os_scheduler.h" in your project because this module
 * will use some functions in that module, but you cannot include it directly
 * in this one. 
 */
#define CONFIG_NOTIF_POST_ENABLE

#define TiFunNotifyHandler TiFunEventHandler
#define NOTIFIER_HOPESIZE(capacity) (sizeof(TiNotifier) - sizeof(TiVector) + VECTOR_HOPESIZE(capacity))

typedef struct{
	uint16       memsize;
	void        owner;
	TiVector    handlers;
}TiNotifier;

TiNotifier *    notif_construct( void * buf, uint16 size, uint8 capacity );
void            notif_destrit( TiNotifier * notif );
void            notif_addlistener( TiNotifier * notif, TxFunNotifyHandler handler, void * object );
void            notif_removelistener( TiNotifier * notif, TxFunNotifyHandler handler, void * object );
void            notif_send( TiNotifier * notif, TiEvent * e );

/* void notif_post( TiNotifier * notif, TxEvent e );
 * notif_post() is similar to notif_send(), but it's an asynchrous call. it will
 * place the event into the scheduler, and let the scheduler to make the function
 * calls.
 */
#ifdef CONFIG_NOTIF_POST_ENABLE
	#define notif_post(notif,e,sche) sche_put(sche,notif_send,notif,e)
#endif

#endif
