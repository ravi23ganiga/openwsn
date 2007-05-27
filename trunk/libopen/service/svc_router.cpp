//----------------------------------------------------------------------------
// @author zhousongli on 20061010
//
// @history
// @modified by zhangwei on 20070524
//	revise the module's interface
//   separate it from the main logic as an independent router module
//----------------------------------------------------------------------------

#include "svc_configall.h"
#include "svc_router.h"

static void router_test();

static void router_test()
{
}

//----------------------------------------------------------------------------
// create and destroy a router object
//----------------------------------------------------------------------------
TOpenRouter * router_create( TSioComm * sio, TTimer * timer )
{
	return NULL;
}

void router_free( TOpenRouter * rt )
{
}

void router_configure( TOpenRouter * rt )
{
}

//----------------------------------------------------------------------------
// the router_evolve() function is called by the timer or the master program. it will check 
// whether the time duration ended. if the time period expired and the router state is OK, then 
// the router will start a network maintainess process to referesh all the routing information 
// among the network. this process will keep the network aware network changings such as 
// a new node added or a old node leaved.
//
//----------------------------------------------------------------------------
void router_evolve( TOpenRouter * rt )
{
    
}

//----------------------------------------------------------------------------
// used for upper layer
// sio_read/sio_write will finally call uart_read/uart_write to receive/send data.
//
// for router_rawread() it will firstly put the received packet into local buffer. the router
// module will check whether the packet should be processed by itself or forward to upper layer.
//
// for router_rawwrite(), it will firstly consule the cache to retrieve a path. and insert the 
// path into the packet then call sio_write to send whole of them out.
//----------------------------------------------------------------------------
int16 router_rawread( TOpenRouter * rt,  char * buf, uint8 capacity, uint8 opt )
{
    return sio_rawread( rt->sio, buf, capacity, opt );		
}

int16 router_rawwrite( TOpenRouter * rt,  char * buf, uint8 len, uint8 opt )
{
    return sio_rawwrite( rt->sio, buf, len, opt );		
}


