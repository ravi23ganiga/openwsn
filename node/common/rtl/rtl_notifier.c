
/* rtl_notifier
 * this is an C language version of notifier object. notifier is used to connect
 * multiple objects and transmit events between sender and receiver.
 *
 * @state
 *	finished. not fully tested	
 *
 * @author zhangwei on 200812
 */ 

typedef struct { 
	TiFunNotifyHandler handler; 
	void * object; 
}_TiNotifierItem;


TiNotifier * notif_construct( void * buf, uint16 size )
{
	TiNotifier * notif = (TiNotifier *)buf;
	notif->memsize = size;
	notif->owner = NULL;
	vec_construct( &notif->handlers, (size - sizeof(TiNotifier) + sizeof(TiVector)), 
		sizeof(_TiNotifierItem) );
	return notif;
}

void notif_destroy( TiNotifier * notif )
{
	return;
}

void notif_addlistener( TiNotifier * notif, TxFunNotifyHandler handler, void * object )
{
	_TiNotifierItem item;
	item.handler = handler;
	item.object = object;
	vec_append( &notif->handlers, &item );
}

void notif_removelistener( TiNotifier * notif, TxFunNotifyHandler handler, void * object )
{
	/* TODO
	 * find the listener in the vector and delete it from the vector.
	 */
}

void notif_send( TiNotifier * notif, TxEvent * e )
{
	_TiNotifierItem * item;
	uint8 i;

	for (i=0; i<vec_count(notif->handlers); i++)
	{
		item = vec_items(notif->handlers, i);
		item->handler( item->object, e );
	}
}
