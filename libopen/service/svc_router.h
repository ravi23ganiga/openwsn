//----------------------------------------------------------------------------
// @author zhangwei on 20061010
//----------------------------------------------------------------------------

#ifndef _SVC_ROUTER_H_8952_
#define _SVC_ROUTER_H_8952_

#include "svc_configall.h"
#include "svc_siocomm.h"

//----------------------------------------------------------------------------
// TOpenRouter object
// this object is part of an node-driven query-based routing protocol. it's very small
// and simple to be implemented on most of hardware device, including from the 
// host computer, sink node to even an general sensor node. 
//
// sink router is running on the sink node.  for a general sensor node, it also 
// has routing functionality. that is handled by TOpenRouter object rather than 
// TOpenRouter object. TOpenRouter is more powerful and complex than TOpenRouter.
// further more, in an entire network, the TOpenRouter needs the cooperation 
// of TOpenRouter or else the whole network can not work.
//
// however, the SinkRouter is not an efficient one. it left a lot of space for
// you to improve it.
//----------------------------------------------------------------------------

#define ROUTER_MAX_HOP_COUNT 10
#define ROUTER_CACHE_SIZE 100

typedef struct{
  uint8 state;
  uint8 life;
  uint8 length;
  uint16 path[ROUTER_MAX_HOP_COUNT ];
}TRouterPath;

typedef struct{
  uint32 state;
  TTimer * timer;
  uint32 duration;
  uint32 next_probe_time;
  TSioComm * sio;
  uint16 path_cache_length;
  TRouterPath path_cache[ROUTER_CACHE_SIZE ];
}TOpenRouter;

TOpenRouter * router_create( TSioComm * sio, TTimer * timer );
void router_free( TOpenRouter * rt );
void router_configure( TOpenRouter * rt );
void router_evolve( TOpenRouter * rt );
int16 router_rawread( TOpenRouter * rt,  char * buf, uint8 capacity, uint8 opt );
int16 router_rawwrite( TOpenRouter * rt,  char * buf, uint8 len, uint8 opt );


#endif

