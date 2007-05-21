#ifndef _SVC_NETNODE_H_7388_
#define _SVC_NETNODE_H_7388_
//----------------------------------------------------------------------------

#include "..\configall.h"
#include <assert.h>
#include "..\foundation.h"

//----------------------------------------------------------------------------
// TOpenNode
//----------------------------------------------------------------------------

#define NODE_STATE_FREE 0
#define NODE_STATE_ACTIVE 1

#define NODE_TXBUFFER_CAPACITY 128
#define NODE_RXBUFFER_CAPACITY 128

// 模拟一个网络结点
// id			a unique identifier of the network node
// state		node state
// x,y,z		该结点的位置
// timestamp	当前时刻
// sen_temprature	当前温度值
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