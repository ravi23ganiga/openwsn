//---------------------------------------------------------------------------
// libsvc 接口说明文件
// 供主程序开发使用
//---------------------------------------------------------------------------

#ifndef _SVCAPI_H_
#define  _SVCAPI_H_
#include "foundation.h"
#include "hal/hal_uart.h"

#define MAX_DATAITEM_NUMBER 100

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

DWORD __stdcall  svc_thread(LPVOID pParam);
DLLAPI void *   svc_create( uint16 id, uint16 opt );
DLLAPI void svc_start(void * svc);
DLLAPI void svc_stop(void *svc);

uint8  svc_write_rxDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt);
uint8  svc_read_txDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt);
uint8  svc_write_txDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt);
uint8  svc_read_rxDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt);

DLLAPI uint8 svc_write( void * svc, char * buf, uint8 len, uint16 opt );
DLLAPI uint8 svc_read( void * svc, char * buf, uint8 capacity, uint16 opt );

#endif
