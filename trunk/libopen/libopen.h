//----------------------------------------------------------------------------
// libopen.h
//----------------------------------------------------------------------------

#ifndef _LIBOPEN_H_
#define _LIBOPEN_H_
//----------------------------------------------------------------------------
// @author zhousongli
// @state developing
//----------------------------------------------------------------------------

#include "foundation.h"
#include "hal\hal_uart.h"
#include "hal\hal_usb.h"
#include "service\svc_siocomm.h"

//----------------------------------------------------------------------------
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



high level data service such as binaryxml analyzing
  svc_binaryxml_getfirst
  svc_binaryxml_getnext
  svc_binaryxml_getprev
  svc_binaryxml_getlast


    svc_relation
  svd_dm_create
  svd_dm_free
  svd_dm_read
  svd_dm_rawread
  svd_dm_write
  svd_dm_rawwrite
  
  
  xxx_create
  xxx_free
  xxx_read
  xxx_write
 xxxÎªdriverËõÐ´£¬°üÀ¨rs232, usb, socket, webserv

rs232_create
rs232_free
rs232_read
rs232_write

usb_create
usb_free
usb_read
usb_write

socket_create
socket_free
socket_read
socket_write

webserv_create
webserv_free
webserv_read
webserv_write
*/

DLLAPI void * libopen_create( uint16 id, uint16 opt );
DLLAPI void libopen_free( void * svc );
DLLAPI void libopen_configure( void * svc );
DLLAPI void libopen_relation( void * svc, void * driver );
DLLAPI uint8 libopen_write( void * svc, char * buf, uint8 len, uint16 opt );
DLLAPI uint8 libopen_read( void * svc, char * buf, uint8 capacity, uint16 opt );
DLLAPI uint8 libopen_rawwrite( void * svc, char * buf, uint8 len, uint16 opt );
DLLAPI uint8 libopen_rawread( void * svc, char * buf, uint8 capacity, uint16 opt );
DLLAPI void * libopen_getdriver( void * svc );

//----------------------------------------------------------------------------
#endif
