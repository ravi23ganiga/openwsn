#ifndef _EXT_LIBOPEN_H_7388_
#define _EXT_LIBOPEN_H_7388_
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
// TOpenNetwork
//----------------------------------------------------------------------------

#define CONFIG_NETSIMU_MAX_NODE 100
#define CONFIG_DATAPACKET_DATASIZE 100

// 模拟一个网络
//
typedef struct{
  uint16 root;
  uint32 sensing_radius;
  uint32 comm_radius;
  TOpenNode nodes[CONFIG_NETSIMU_MAX_NODE];
  uint16 random_sequence[CONFIG_NETSIMU_MAX_NODE];
  uint16 read_cursor;
  uint32 last_probetime;
}TOpenNetwork;

typedef struct{
  uint16 id;
  char data[CONFIG_DATAPACKET_DATASIZE];
}TOpenDataPacket;

DLLAPI TOpenNetwork * _stdcall net_create();
DLLAPI void _stdcall net_free( TOpenNetwork * net );
DLLAPI int _stdcall net_open( TOpenNetwork * net );
DLLAPI void _stdcall net_close( TOpenNetwork * net );
DLLAPI int _stdcall net_write( TOpenNetwork * net, TOpenDataPacket * datapacket, uint8 opt );
DLLAPI int _stdcall net_read(  TOpenNetwork * net, TOpenDataPacket * datapacket, uint8 opt );
DLLAPI int _stdcall net_rawwrite( TOpenNetwork * net, char * buf, uint8 len, uint8 opt );
DLLAPI int _stdcall net_rawread( TOpenNetwork * net, char * buf, uint8 capacity, uint8 opt );
DLLAPI void _stdcall net_evolve( TOpenNetwork * net );
DLLAPI void _stdcall net_probe( TOpenNetwork * net );
DLLAPI void _stdcall net_probe_node( TOpenNetwork * net, uint16 nodeid );
DLLAPI uint16 _stdcall net_get_node_count( TOpenNetwork * net );
DLLAPI TOpenNode * _stdcall net_node( TOpenNetwork * net, uint16 idx );
DLLAPI int _stdcall net_get_neighbor_nodes( TOpenNetwork * net, uint16 id, uint32 radius, uint16 * buf, uint16 capacity );
DLLAPI uint32 _stdcall net_distance_between( TOpenNetwork * net, uint16 id1, uint16 id2 );
DLLAPI int _stdcall net_generate( TOpenNetwork * net );
DLLAPI int _stdcall net_load( TOpenNetwork * net, char * filename );
DLLAPI int _stdcall net_save( TOpenNetwork * net, char * filename );
DLLAPI int _stdcall net_sleep( TOpenNetwork * net );
DLLAPI int _stdcall net_wakeup( TOpenNetwork * net );

//----------------------------------------------------------------------------
#endif
