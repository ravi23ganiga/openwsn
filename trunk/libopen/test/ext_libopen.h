#ifndef _EXT_LIBOPEN_H_7388_
#define _EXT_LIBOPEN_H_7388_
//----------------------------------------------------------------------------

#define CONFIG_APPLICATION
#include "configall.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//----------------------------------------------------------------------------
// TTimer
//----------------------------------------------------------------------------

// @TODO
typedef struct{
  uint8 id;
}TTimer;


//----------------------------------------------------------------------------
// TUart
//----------------------------------------------------------------------------

#define TUart TUartDriver

typedef struct{
  uint8 id;
  HANDLE handle;
  char name[5];
  COMMTIMEOUTS  new_timeout;
  COMMTIMEOUTS  old_timeout;
  uint8 databits;
  uint8 stopbits;
  uint8 parity;
  uint32 baudrate;
  bool isOpened;
}TUartDriver;


//----------------------------------------------------------------------------
// TSioComm
//----------------------------------------------------------------------------

//#define CONFIG_MAX_UART_FRAME_LENGTH 127

// configure TSioComm's internal buffer size
// MAX_FRAAME_LENGTH should be less than 0xFF. the maximum value of it is 0xFE
// the default settings in our application is 0x7E (127). not 0xFF(128)
//
#define SIO_FRAMEBUFFER_LENGTH CONFIG_MAX_UART_FRAME_LENGTH-3
#define SIO_RXBUFFER_CAPACITY SIO_FRAMEBUFFER_LENGTH
#define SIO_TXBUFFER_CAPACITY SIO_FRAMEBUFFER_LENGTH

#define SIOCOMM_RXBUFFER_CAPACITY 100

typedef struct{
  TUartDriver * uart;
  uint8 rxlen;
  uint8 txlen;
  char rxbuf[SIO_RXBUFFER_CAPACITY];
  char txbuf[SIO_TXBUFFER_CAPACITY];  
  uint8 option;
  // the following are to support version 3
  uint8 sof; 
  uint8 fmstate;
  uint8 fmstate_sofcount;
  uint8 fmstart;
  uint8 fmlength;
}TSioComm;


//----------------------------------------------------------------------------
// TBinaryXml
//----------------------------------------------------------------------------

#define BXML_PROPERTY_TOTAL_LENGTH 0
#define BXML_PROPERTY_

typedef struct{
  char * buf;
  uint8 capacity;
  uint8 length;
  uint8 current;
}TBinaryXml;

//#define xml_update  xml_write

TBinaryXml * xml_construct( TBinaryXml * xml, char * xmldata, uint8 size );
void  xml_destroy( TBinaryXml * xml );
void  xml_attach( TBinaryXml * xml, char * xmldata, uint8 len ); 
uint8 xml_newnode( TBinaryXml * xml, uint8 request_size );
uint8 xml_append( TBinaryXml * xml, uint8 parid, uint8 property, char * data, uint8 datalen );
uint8 xml_write( TBinaryXml * xml, uint8 id, uint8 parid, uint8 property, char * data, uint8 datalen );
uint8 xml_read( TBinaryXml * xml, uint8 id, uint8 * parid, uint8 * property, char * data, uint8 size );
void  xml_remove( TBinaryXml * xml, uint8 id );
uint8 xml_findparent( TBinaryXml * xml, uint8 id );
uint8 xml_findchild( TBinaryXml * xml, uint8 id );
uint8 xml_findprev( TBinaryXml * xml, uint8 id );
uint8 xml_findnext( TBinaryXml * xml, uint8 id );


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
  void * data;
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
// TOpenRouter 
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


//----------------------------------------------------------------------------
// TOpenNetwork
//----------------------------------------------------------------------------
//
// TUart ==> TSioComm ===> TOpenNetwork
//
// TOpenNetwork represents a WSN network and provide necessary functions for the 
// GUI application to interact with the WSN network. 
//
// TOpenNetwork object also used in simulation to hold a collection of network nodes.

#define CONFIG_NETSIMU_MAX_NODE 100
#define CONFIG_DATAPACKET_DATASIZE 100

#define OPENNET_MODE_SIMULATION 0x00
#define OPENNET_MODE_REALNETWORK 0x01

// mode             control the network work in simulation or with a real network
//                  default to simulated network now.
// root             the root node of network. usually it is the sink node.
// comm_radius	    the maximum distance that two node can communicate.
// nodes            the collection of network nodes. used in simulation
// random_sequence  used for random visiting of network nodes. useful in simulation.
// 
typedef struct{
  uint8             mode;
  uint16            root;
  TSioComm *        sio;
  TTimer *          timer;
  TOpenRouter *     router;
  uint32            sensing_radius;
  uint32            comm_radius;
  TOpenNode	        nodes[CONFIG_NETSIMU_MAX_NODE];
  uint16            random_sequence[CONFIG_NETSIMU_MAX_NODE];
  uint16            read_cursor;
  uint32            last_probetime;
}TOpenNetwork;

// @attention
// this structure is used to exchange information between GUI application and 
// TOpenNetwork object. it's different to the TOpenPacket structure. TOpenPacket
// is used to transmit information between two real network node devices. 
//
// however, most of they two are identical. TOpenDataPacket may contains more.

// this structure may merged with TOpenPacket 
typedef struct{
  uint16 id;
  char data[CONFIG_DATAPACKET_DATASIZE];
}TOpenDataPacket;

DLLAPI TOpenNetwork * _stdcall net_create( uint8 mode, TSioComm * sio, TTimer * timer );
DLLAPI void _stdcall net_free( TOpenNetwork * net );
DLLAPI int _stdcall  net_open( TOpenNetwork * net );
DLLAPI void _stdcall net_close( TOpenNetwork * net );
DLLAPI int _stdcall  net_write( TOpenNetwork * net, TOpenDataPacket * datapacket, uint8 opt );
DLLAPI int _stdcall  net_read(  TOpenNetwork * net, TOpenDataPacket * datapacket, uint8 opt );
DLLAPI int _stdcall  net_rawwrite( TOpenNetwork * net, char * buf, uint8 len, uint8 opt );
DLLAPI int _stdcall  net_rawread( TOpenNetwork * net, char * buf, uint8 capacity, uint8 opt );
DLLAPI void _stdcall net_evolve( TOpenNetwork * net );
DLLAPI void _stdcall net_probe( TOpenNetwork * net );
DLLAPI void _stdcall net_probe_node( TOpenNetwork * net, uint16 nodeid );
DLLAPI uint16 _stdcall net_get_node_count( TOpenNetwork * net );
DLLAPI TOpenNode * _stdcall net_node( TOpenNetwork * net, uint16 idx );
DLLAPI int _stdcall  net_get_neighbor_nodes( TOpenNetwork * net, uint16 id, uint32 radius, uint16 * buf, uint16 capacity );
DLLAPI uint32 _stdcall net_distance_between( TOpenNetwork * net, uint16 id1, uint16 id2 );
DLLAPI int _stdcall  net_generate( TOpenNetwork * net );
DLLAPI int _stdcall  net_load( TOpenNetwork * net, char * filename );
DLLAPI int _stdcall  net_save( TOpenNetwork * net, char * filename );
DLLAPI int _stdcall  net_sleep( TOpenNetwork * net );
DLLAPI int _stdcall  net_wakeup( TOpenNetwork * net );


//----------------------------------------------------------------------------
// TQueryEngine 
//----------------------------------------------------------------------------

// TQueryEngine
// interprete and execute the query received from GUI application
//
typedef struct{
  uint8 state;
  void * net;
  void * cond;
  void * dataset;
  void * cache;
  char * filename;
}TQueryEngine;


//----------------------------------------------------------------------------
// TOpenWSN Service
//----------------------------------------------------------------------------

DLLAPI int32 _stdcall svc_create( int mode );
DLLAPI void _stdcall svc_free();
DLLAPI void _stdcall svc_startup();
DLLAPI void _stdcall svc_shutdown();
DLLAPI TOpenNetwork * _stdcall svc_get_network();
DLLAPI TQueryEngine * _stdcall svc_get_queryengine();
DLLAPI TSioComm * _stdcall svc_get_siocomm();
DLLAPI TUart * _stdcall svc_get_uart();
DLLAPI void * _stdcall svc_query();

//----------------------------------------------------------------------------
#endif
