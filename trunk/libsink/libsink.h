//---------------------------------------------------------------------------
// libsvc 接口说明文件
// 供主程序开发使用
//---------------------------------------------------------------------------

#ifndef _SVCAPI_H_
#define  _SVCAPI_H_
#include "foundation.h"

#define maxDataRevNumber   0x0f
#define MAXLEAPNUMBER       0x05
#define MAX_ROUTE_LIST_NUMBER 0x04

#define MAX_DATAITEM_NUMBER 100
#define MAX_ROUTE_PATH_NUMBER 200
#define MAX_ROUTE_PATH_OPTIMAL_NUMBER 50

#define DATA_TYPE_BM             0x07
#define ADDRLIST_LENTH  0x38
#define ADDRLIST_BM        0x03

typedef struct{
//BYTE addrMode;
//ADDRESS addr;
WORD addr;
} RouteAddrItem;

typedef struct{
//BYTE   srcAddrMode;
WORD srcAddr;
//BYTE dstAddrMode;//destination node address mode;
WORD dstAddr;//destionateion node address;   
RouteAddrItem dstAddrItem[MAXLEAPNUMBER];
}RouteAddr;

/*
typedef struct{
RouteAddr RouteAddr[MAX_ROUTE_LIST_NUMBER];
uint8 inpos;
uint8 outpos;
uint8 count;
}RouteAddrList;
*/
// Data structures used with the MAC primitives
typedef union {
    WORD  Short;
    QWORD Extended;
} ADDRESS;

//Address list table used to store allocated address;
typedef struct {
WORD   panid;
WORD   macCoordShortAddr;
WORD   shortAssociateAddr;
ADDRESS aExtendedAddr;
}AddrItem;

typedef enum{
RouteRequest,
RouteFeedback,
QueryData,
QueryFeekback
} DataType;
	

// The structure which is used as payload in the mcpsDataRequest function
// DO NOT CHANGE THE ORDER INSIDE THIS STRUCTURE!!!
typedef struct {
    UINT8 FrameControl;//D0~D2表示数据类型；D3~D5表示地址个数；
    UINT8 seqNumber;
    RouteAddr routeAddr;
    BYTE pData[100-sizeof(RouteAddr)];//里面的常数不能修改；
} RF_PAYLOAD;


typedef struct{
  uint16 nodeid;  
  uint16 path[15]; // path. we don't consider those exceed 15 hop 
  uint8 length;   // path length
}TRoutePathCacheItem;

typedef struct{
  uint16 nodeid;  
  uint16 path[10];  
  uint8 length;
}TRouteOptimalPathItem;

typedef struct{
TRouteOptimalPathItem RouteOptimalPathItem[MAX_ROUTE_PATH_OPTIMAL_NUMBER];
uint8  totalCnt;
}TRouteOptimalPathCache;


typedef struct{
TRoutePathCacheItem RoutePathItem[MAX_ROUTE_PATH_NUMBER];
uint8 totalCnt;
}TRoutePathCache;

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

typedef struct{
  uint16 sinknode;
  uint8 maxhop;
  uint16 updateperiod;
  TRoutePathCache pathCache;
  TRouteOptimalPathCache optimalPathCache;
}TSinkService;

typedef struct{
TSinkService sinkServRoute;
TDataQueue rxDataQueue;
TDataQueue txDataQueue;
}TSvrData;

DLLAPI DWORD _stdcall svc_thread(LPVOID pParam);
DLLAPI void * __stdcall  svc_create( uint16 id, uint16 opt );
DLLAPI void svc_start(void * svc);
DLLAPI void svc_stop(void *svc);

DLLAPI uint8 _stdcall svc_write_rxDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt);
DLLAPI uint8 _stdcall svc_read_txDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt);
DLLAPI uint8 _stdcall svc_write_txDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt);
DLLAPI uint8 _stdcall svc_read_rxDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt);


/*It means optimal route table if opt equal 1,otherwise, general route path cache.*/
// @TODO
// please use int to replace bool when you want to export it as a DLL function
//

DLLAPI bool _stdcall svc_routetable_add_addrItem(TSinkService *pTSinkServ,RouteAddr routeAddr,uint8 addrLen,uint8 opt);
DLLAPI bool _stdcall svc_select_routeCache(TSinkService *pTSinkServ,WORD srcNodeId,WORD destNodeId,TRoutePathCache *pRouteCache,uint8 opt);
DLLAPI bool _stdcall svc_select_routeOptimalCache(TSinkService *pTSinkServ,WORD srcNodeId,WORD destNodeId,TRouteOptimalPathCache *pRouteCache,uint8 opt);

DLLAPI uint8 svc_write( void * svc, char * buf, uint8 len, uint16 opt );
DLLAPI uint8 svc_read( void * svc, char * buf, uint8 capacity, uint16 opt );

#endif
