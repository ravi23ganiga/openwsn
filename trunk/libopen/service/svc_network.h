//----------------------------------------------------------------------------
// @author zhangwei on 20070524
//----------------------------------------------------------------------------

#ifndef _SVC_NETWORK_H_4832_
#define _SVC_NETWORK_H_4832_

#include "svc_configall.h"
#include "svc_netnode.h"

#define CONFIG_NETSIMU_MAX_NODE 100
#define CONFIG_DATAPACKET_DATASIZE 100


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

DLLAPI TOpenNetwork * _stdcall opennet_create();
DLLAPI void _stdcall opennet_free( TOpenNetwork * net );
DLLAPI int _stdcall  opennet_open( TOpenNetwork * net );
DLLAPI void _stdcall opennet_close( TOpenNetwork * net );
DLLAPI int _stdcall  opennet_write( TOpenNetwork * net, TOpenDataPacket * datapacket, uint8 opt );
DLLAPI int _stdcall  opennet_read(  TOpenNetwork * net, TOpenDataPacket * datapacket, uint8 opt );
DLLAPI int _stdcall  opennet_rawwrite( TOpenNetwork * net, char * buf, uint8 len, uint8 opt );
DLLAPI int _stdcall  opennet_rawread( TOpenNetwork * net, char * buf, uint8 capacity, uint8 opt );
DLLAPI void _stdcall opennet_evolve( TOpenNetwork * net );
DLLAPI void _stdcall opennet_probe( TOpenNetwork * net );
DLLAPI void _stdcall opennet_probe_node( TOpenNetwork * net, uint16 nodeid );
DLLAPI uint16 _stdcall opennet_get_node_count( TOpenNetwork * net );
DLLAPI TOpenNode * _stdcall opennet_node( TOpenNetwork * net, uint16 idx );
DLLAPI int _stdcall  opennet_get_neighbor_nodes( TOpenNetwork * net, uint16 id, uint32 radius, uint16 * buf, uint16 capacity );
DLLAPI uint32 _stdcall opennet_distance_between( TOpenNetwork * net, uint16 id1, uint16 id2 );
DLLAPI int _stdcall  opennet_generate( TOpenNetwork * net );
DLLAPI int _stdcall  opennet_load( TOpenNetwork * net, char * filename );
DLLAPI int _stdcall  opennet_save( TOpenNetwork * net, char * filename );
DLLAPI int _stdcall  opennet_sleep( TOpenNetwork * net );
DLLAPI int _stdcall  opennet_wakeup( TOpenNetwork * net );


//----------------------------------------------------------------------------
#endif
