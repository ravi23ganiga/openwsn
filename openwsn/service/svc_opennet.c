
#include "svc_foundation.h"
#include "svc_opennet.h"

typedef struct{
  uint16 dest;
  uint16 next;
  uint8 metric;
  uint8 quality;
}TRouteTableItem;

TRouteTableItem g_routetable[ NET_ROUTE_TABLE_SIZE ];
TOpenNET g_opennet;

int8 net_construct( TOpenMAC * net, TActionScheduler * actsche )
{
	return 0;
}

void net_destroy()
{
	return;
}

void net_configure( uint8 ctrlcode, uint8 value )
{
	return;
}

int8 net_setaddress( char * addr, uint8 len )
{
	return 0;
}

int8 net_getaddress( char * addr, uint8 capacity )
{
	return 0;
}

int8 net_read( TOpenPacket * pkt, uint8 opt )
{
	return 0;
}

int8 net_rawread( char * frame, uint8 size, uint8 opt )
{
	return 0;
}

int8 net_write( TOpenPacket * pkt, uint8 opt )
{
	return 0;
}

int8 net_rawwrite( char * frame, uint8 size, uint8 opt )
{
	return 0;
}

int8 net_forward( TOpenPacket * pkt, uint8 opt )
{
	return 0;
}

int8 net_rawforward( char * frame, uint8 size, uint8 opt )
{
	return 0;
}

int8 net_evolve( TOpenNET * net )
{
	return 0;
}

int8 net_sleep( TOpenNET * net )
{
	return 0;
}

int8 net_wakeup( TOpenNET * net )
{
	return 0;
}

int8 net_installnotify( TOpenNET * net, TEventHandler * callback, void * data )
{
	return 0;
}

