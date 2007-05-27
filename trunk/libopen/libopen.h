//----------------------------------------------------------------------------
// libopen.h
//----------------------------------------------------------------------------

#ifndef _LIBOPEN_H_
#define _LIBOPEN_H_

//----------------------------------------------------------------------------
// @author zhousongli
// @state developing
//
// @modified by zhangwei on 20070524
// the architecture of libopen
//
//           TQueryEngine................................
//                 |                              load from Database or File
//            TOpenNetwork..............TSimuNetwork-----
//                 |                                    |
//                 |THostDrivenRouting.......Log to Database or File
//          ---------------
//          |             |
//      TSioComm      TSioComm
//        TUart        TUsb
// 
// there're some key objects in libopen:
// o TUart
//	exposed to application developer
//  the user can use this object to configure the Rs232 parameter or Usb device
//  parameter. generally, the user will not do this because we have already 
//  set their default configurations.
// o TUsb (not developed now)
//	similar to TUsb
// o TSioComm
//	on top of TUart
//	provide frame assemble and disassemble. 
//  the payload in each frame can be mapped to an TQueryDataItem object.
//
// o TOpenRouter
//	an libopen/host driven routing protocol. query based. 
//  this protocol enable you can communicate with remote nodes in the network 
//  by multi-hop forwarding.
//
// o TOpenNetwork
//  exposed to application developer.
//  provide an interface for the upper level developer to interact with the 
//  sensor network. it's an abstraction of the whole network.
//
// o TQueryEngine
//  exposed to application developer.
//  on top of TOpenNetwork interface
//  user query command interpretation and execution
//
// o TSimuNetwork
//  exposed to application developer.
//  has almost the same interface of TOpenNetwork
//  used for simulation only
//
// there're also some utility object
// o TBinaryXml 
//  ease the manipulation of payload data.
// o TQueryDataItem 
//  represents a data item. data item : packet = 1 : 1
// o TQueryDataSet
//	is an list of the TQueryDataItem
//
//----------------------------------------------------------------------------

#include "foundation.h"
#include "hal/hal_uart.h"
#include "hal/hal_usb.h"
#include "service/svc_siocomm.h"
#include "service/svc_network.h"
#include "service/svc_dataset.h"
#include "service/svc_queryengine.h"
#include "simu/svc_netsimu.h"

//----------------------------------------------------------------------------
// Interface of OpenWSN system
// generally, it's enough to use the following functions only for most of the 
// application.
//----------------------------------------------------------------------------

DLLAPI int _stdcall  wsn_open( int mode );
DLLAPI void _stdcall wsn_close();
DLLAPI TOpenNetwork* wsn_get_network();
DLLAPI int _stdcall  wsn_write( TOpenDataPacket * datapacket, uint8 opt );
DLLAPI int _stdcall  wsn_read(  TOpenDataPacket * datapacket, uint8 opt );
DLLAPI int _stdcall  wsn_rawwrite( char * buf, uint8 len, uint8 opt );
DLLAPI int _stdcall  wsn_rawread( char * buf, uint8 capacity, uint8 opt );
DLLAPI void _stdcall wsn_evolve();
DLLAPI void _stdcall wsn_probe();
DLLAPI void _stdcall wsn_probe_node( uint16 nodeid );
DLLAPI uint16 _stdcall wsn_get_node_count();
DLLAPI TOpenNode * _stdcall wsn_node( uint16 idx );
DLLAPI int _stdcall  wsn_get_neighbor_nodes( uint16 id, uint32 radius, uint16 * buf, uint16 capacity );
DLLAPI uint32 _stdcall wsn_distance_between( uint16 id1, uint16 id2 );
DLLAPI int _stdcall  wsn_generate();
DLLAPI int _stdcall  wsn_load( char * filename );
DLLAPI int _stdcall  wsn_save( char * filename );
DLLAPI int _stdcall  wsn_sleep();
DLLAPI int _stdcall  wsn_wakeup();

// @TODO
DLLAPI TSioComm * _stdcall wsn_get_siocomm();
DLLAPI TUart * _stdcall wsn_get_uart();
DLLAPI TQueryEngine * _stdcall wsn_get_queryengine();
DLLAPI int _stdcall  wsn_query();

//----------------------------------------------------------------------------
// Interface of TOpenNetwork 
//----------------------------------------------------------------------------

DLLAPI TOpenNetwork * _stdcall net_create( TSioComm * sio, TTimer * timer );
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
#ifdef CONFIG_VERSION_10
#define MAX_DATAITEM_NUMBER 100
/*
typedef struct{
char data[128];
uint8 totalen;
}TDataItem;

typedef struct{
TDataItem dataItem[MAX_DATAITEM_NUMBER];
uint8 ptrhead;
uint8 ptrtail;
uint8 totalCnt;
}TDataQueue;
*/

// obsolete
typedef struct{
//TSinkService sinkServRoute;
TDataQueue rxDataQueue;
TDataQueue txDataQueue;
}TSvrData;

typedef struct{
	uint16            id;
	char			type;
	int				state;
/*	stack<TProfileStackItem> stack;
	tstring			caption;
	tstring			dirfrom;
	tstring			dirto;
	volatile int    foldercount;
	volatile int	folderdone;
	volatile int    filecount;
	volatile int	filedone;
	volatile int	action;
    volatile bool   cancel;
	volatile int	option;

    HANDLE			thread;
	tstring			quefile;
	TFileQueue *	queue;
	TActionService * asv;
*/
	volatile bool   cancel;
	HANDLE			hthread;
    TSioComm *pSioComm;
	TSvrData  svrData;	
//	CCriticalSection Section;
}TSvcPacket;

DWORD __stdcall svc_thread(LPVOID pParam);
DLLAPI void * svc_create( uint16 id, uint16 opt );
DLLAPI void svc_start(void * svc);
DLLAPI void svc_stop(void *svc);
/*
uint8  svc_write_rxDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt);
uint8  svc_read_txDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt);
uint8  svc_write_txDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt);
uint8  svc_read_rxDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt);
*/
DLLAPI uint8 svc_write( void * svc, char * buf, uint8 len, uint16 opt );
DLLAPI uint8 svc_read( void * svc, char * buf, uint8 capacity, uint16 opt );


/* 
DLLAPI void * svc_create( uint16 id, uint16 opt );
DLLAPI void svc_free( void * svc );
DLLAPI void svc_configure( void * svc );
DLLAPI void svc_relation( void * svc, void * driver );
DLLAPI uint8 svc_write( void * svc, char * buf, uint8 len, uint16 opt );
DLLAPI uint8 svc_read( void * svc, char * buf, uint8 capacity, uint16 opt );
DLLAPI uint8 svc_rawwrite( void * svc, char * buf, uint8 len, uint16 opt );
DLLAPI uint8 svc_rawread( void * svc, char * buf, uint8 capacity, uint16 opt );

DLLAPI int8 svc_dbopen( void * svc, char * db );  // connect to a existed database file
DLLAPI int8 svc_dbclose( void * svc );
DLLAPI int8 svc_dbread()   // read one data item from the database it's usually a binaryxml format array
DLLAPI int8 svc_dbwrite()
DLLAPI int8 svc_dbload    // load a database file into the memory for further processing
DLLAPI int8 svc_dbsave
DLLAPI int8 svc_dbgen    // generate a random data logging database file

DLLAPI int8 svc_query( void * svc, TQueryCondition * cond, TRangeData * data )
DLLAPI int8 svc_querynode( void * svc, uint16 nodeid, uint8 sentype, uint8 * value, uint8 capacity );
DLLAPI int8 svc_querylocation( void * svc, Tlocation * loc, uint8 sentype, uint8 * value, uint8 capacity, uint8 * probability );
*/
#endif /* CONFIG_VERSION_10 */



//----------------------------------------------------------------------------
#endif
