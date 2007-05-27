#ifndef _SVC_NETNODE_H_7388_
#define _SVC_NETNODE_H_7388_
//----------------------------------------------------------------------------

#include "svc_configall.h"
#include <assert.h>

//----------------------------------------------------------------------------
// TOpenNode
//----------------------------------------------------------------------------

// TOpenNode object represent the node device in the networks. 
//
// @attention
//  in "libopen" implementation, both the real version and simulation version
// share the same data structure of TOpenNode and TOpenNetwork. this feature
// may waste some memory, but it enable the upper level GUI application can be
// switched between real network and simulated network without compiling the 
// program again!
//
//	txbuf, txlen, rxbuf, rxlen, sen_temprature, sen_strain are used in simulation only!	
//
//	id			a unique identifier of the network node
//	state		node state
//	x,y,z		该结点的位置
//	timestamp	当前时刻
//	sen_temprature	当前温度值

#define NODE_STATE_FREE 0
#define NODE_STATE_ACTIVE 1

#define NODE_TXBUFFER_CAPACITY 128
#define NODE_RXBUFFER_CAPACITY 128

//
typedef struct{
  uint16 id;				
  uint16 parent;
  uint8  state;
  uint32 x,y,z;
  uint32 timestamp;
  uint16 radius;
  uint8  txlen;
  uint8  rxlen;
  char   txbuf[NODE_TXBUFFER_CAPACITY];
  char   rxbuf[NODE_RXBUFFER_CAPACITY];
  uint32 sen_temprature;
  uint32 sen_strain; 
}TOpenNode;

DLLAPI TOpenNode * __stdcall netnode_create();
DLLAPI void __stdcall netnode_free( TOpenNode * node );
DLLAPI TOpenNode * __stdcall netnode_construct( TOpenNode * node );
DLLAPI void __stdcall netnode_destroy( TOpenNode * node );
DLLAPI int _stdcall netnode_write( TOpenNode * net, char * buf, uint8 len, uint8 opt );
DLLAPI int _stdcall netnode_read( TOpenNode * net, char * buf, uint8 capacity, uint8 opt );

//----------------------------------------------------------------------------
#endif