
#include "foundation.h"
#include <windows.h>
#include "service/svc_siocomm.h"
#include "service/svc_log4c.h"
#include "libsink.h"

//extern CWinThread *pThread; 
//extern TSioComm  *g_pTSioComm;

TSioComm   m_TSioComm;
TSioComm  *g_pTSioComm;
TUartDriver  m_TUartDriver;
TUartDriver *g_pTUartDriver;

//TSvrData svrData;
//CCriticalSection Section; 

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
	// @modified by zhangwei on 200701
	// zhangwei decorated these two variable as volatile
	volatile bool   cancel;
	volatile HANDLE hthread;
	TSvrData  svrData;	
//	CCriticalSection Section;
}TSvcPacket;

//bool bend = FALSE;//定义的全局变量，用于控制线程的运行 
DLLAPI void * __stdcall  svc_create( uint16 id, uint16 opt )
{
	TSvcPacket * pSvcPack;
	uint16 opt1 = opt;
	pSvcPack = new TSvcPacket();
	pSvcPack->id = id;
	pSvcPack->type = 0;
	pSvcPack->cancel = false;
	pSvcPack->state = 0;
	memset((char *)&pSvcPack->svrData,0x00,sizeof(TSvrData));
	g_pTUartDriver = uart_construct(0, (char *)&m_TUartDriver, sizeof(TUartDriver));
    g_pTSioComm = sio_construct((char *)&m_TSioComm, 
			sizeof(TSioComm), 
			g_pTUartDriver, 
			SIO_OPTION_FRAME_SUPPORT);

	
	return (void *)pSvcPack;
}
DLLAPI void svc_start(void * svc)
{
   TSvcPacket *pSvcPack = (TSvcPacket *) svc;
  //start_thread( sink_service_thread )
	//memset((char *)&svrData,0,sizeof(TSvrData));
   if (pSvcPack->hthread == NULL)
	{
		//InitializeCriticalSection(&pSvcPack->Section); 

		pSvcPack->hthread = CreateThread(
        			NULL,							// default security attributes
					0,								// use default stack size
					svc_thread,           // thread function
					pSvcPack,						// argument to thread function
					0,								// use default creation flags
					NULL);							// returns the thread identifier

		
		pSvcPack->cancel = false;
	}
//	pThread=AfxBeginThread(svc_thread,&svrData);//启动线程 
//	pThread->m_bAutoDelete=FALSE;//线程为手动删除 	
}

DLLAPI void svc_stop(void *svc)
{
	
	TSvcPacket *pSvcPack = (TSvcPacket *)svc;

	if ( pSvcPack != NULL && pSvcPack->hthread != NULL)
	{
	  pSvcPack->cancel = true;	
	//bend=TRUE;//改变变量，线程结束 	
	  //Sleep(1000);  
	  if (pSvcPack->hthread != NULL)
	  {
	  //WaitForSingleObject(pSvcPack->hthread,INFINITE);//等待线程结束 

	  // @modified by zhangwei on 200701
	  // for zhou songli
	  // you use CreateThread to allocate resource for thread. you cannot call delete to free the 
	  // resource. i remember i had given you the template on how to canncel a thread.
	  //
	  //delete pSvcPack->hthread;//删除线程 
		
		//DWORD exitcode = 0;
		//if (GetExitCodeThread( pSvrPack->thread, &exitcode) != 0)
			{
				TerminateThread( pSvcPack->hthread, -1 );
				CloseHandle( pSvcPack->hthread );
			}
		pSvcPack->hthread = NULL;

	  }
	Beep(100,100); 
	}
}

//The Thread Function 
DLLAPI DWORD _stdcall svc_thread(LPVOID pParam)
{
	uint8 cnt;
	//uint8  dataType;
	//uint8  AddrCnt;
	TSvcPacket *pSvcPack = (TSvcPacket *)pParam;
	TSvrData *pSvrData ;
	char tempData[128];
	FILE * stream_source;
	
	RF_PAYLOAD *pRfPayload = (RF_PAYLOAD *) &tempData[0];       
	memset(tempData,0,sizeof(tempData));
	pSvrData = &pSvcPack->svrData;
	while(!pSvcPack->cancel) 
	{
	   //Beep(100,100); 
	//read data from rs232;
	   //cnt = sio_read(g_pTSioComm, &tempData[0], 128, 0);
	   cnt = 0;
	   if (cnt >0){

	/* phase the data read;push the route data into sinkServRoute and received data into 
	   rxDataQueue;
        */ 
  /*          dataType = BF(pRfPayload->FrameControl,DATA_TYPE_BM,0);
            AddrCnt = BF(pRfPayload->FrameControl,ADDRLIST_LENTH, ADDRLIST_BM);
            cnt = pRfPayload->pData[0];
	    switch(dataType)
	    {
	       case RouteFeedback:
			   svc_routetable_add_addrItem(&pSvrData->sinkServRoute,
		   pRfPayload->routeAddr,AddrCnt,0);
		   break;
	       case QueryFeekback:
		 				   svc_write_rxDataQueue(&pSvrData->rxDataQueue,
				   (char *)&pRfPayload->pData[1], cnt, 0);		   
		   break;
	    }		
		*/
		 svc_write_rxDataQueue(&pSvrData->rxDataQueue,(char *)&tempData[0], cnt, 0);	
		
		// @modified by zhangwei 
		// 最好在进入thread一开始就fopen，退出时close，不要频繁open/close

		stream_source=fopen("D:\\source\\sio_raw_revcived.txt","w+");
		fseek(stream_source,0L,SEEK_SET); 						
		fwrite(&tempData[0],cnt,1,stream_source);			
		fclose(stream_source);	

                        
	  } else{ /* write the data into sio_txbuf if there are txQueueData sent.*/ 
		  //cnt = svc_read_txDataQueue(&pSvrData->txDataQueue,&tempData[0], 128,0);
	    //if (cnt >0)	sio_write(g_pTSioComm, &tempData[0], cnt, 0);
	 
	  }
	Sleep(1000); 
	} 

	free(tempData);
	pRfPayload = NULL;

	// @modified by zhangwei on 200701
	pSvcPack->hthread = NULL;
	return 0; 

}

DLLAPI uint8 svc_read( void * svc, char * buf, uint8 capacity, uint16 opt )
{
	TSvcPacket * pSvcPack = (TSvcPacket *)svc;
	TSvrData * pTSvrData = &pSvcPack->svrData;
     if (svc == NULL)	 return 0;
     if (buf == NULL)  return 0;	 
	 return (svc_read_rxDataQueue(&pTSvrData->rxDataQueue, buf,capacity,opt));
}

DLLAPI uint8 svc_write( void * svc, char * buf, uint8 len, uint16 opt )
{
	TSvcPacket * pSvcPack = (TSvcPacket *)svc;
	TSvrData * pTSvrData = &pSvcPack->svrData;
     if (svc == NULL)	 return 0;
     if (buf == NULL)  return 0;	 
     return (svc_write_txDataQueue(&pTSvrData->txDataQueue, buf,len,opt));     
}

void svc_timer_handler()
{

}

void svc_optmize_path( TSinkService * svc )
{
}

DLLAPI uint8 _stdcall svc_write_rxDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt)
{
    uint8 ptr;	
	TDataItem *pDataItem;
	if (rxDataQueue == NULL) return 0;
	if (buf == NULL) return 0;
	if (len ==0) return 0;
	//EnterCriticalSection(&Section); 
        ptr = rxDataQueue->ptrtail++;
	if (ptr >= MAX_DATAITEM_NUMBER -1)
	{
		ptr = 0;
	}	
	pDataItem = &rxDataQueue->dataItem[ptr];
	pDataItem->totalen = len;
	memmove(&pDataItem->data[0],buf,len);
	
	rxDataQueue->totalCnt++;
        //LeaveCriticalSection(&Section);
	return len;
}

DLLAPI uint8 _stdcall svc_read_rxDataQueue(TDataQueue *rxDataQueue, char *buf, uint8 capacity, uint16 opt)
{
	uint8 ptr;	
	TDataItem *pDataItem;
	uint8 count;
	if (rxDataQueue == NULL) return 0;
	if (buf == NULL) return 0;
	if (capacity==0) return 0;
  	if (rxDataQueue->totalCnt <1) return 0;//no data;

	//EnterCriticalSection(&Section); 
        ptr = rxDataQueue->ptrhead;
	pDataItem = &rxDataQueue->dataItem[ptr];
	count = MIN(pDataItem->totalen,capacity);
	
	memmove(buf,&pDataItem->data[0],count);
	pDataItem->totalen -= count ;
	if (pDataItem->totalen >0)
	{
           memmove(&pDataItem->data[0],&pDataItem[count],pDataItem->totalen - count);   		   
	}else{
		rxDataQueue->totalCnt--;
		rxDataQueue->ptrhead++;
		if (rxDataQueue->ptrhead >= MAX_DATAITEM_NUMBER -1)	
		rxDataQueue->ptrhead = 0;
	}
        //LeaveCriticalSection(&Section);
	return count;

}

 uint8 _stdcall svc_read_txDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt)
{
	uint8 ptr;	
	TDataItem *pDataItem;
	uint8 count;
	uint16 opt1 = opt;
	FILE *stream_source;
	
	if (txDataQueue == NULL) return 0;
	if (buf == NULL) return 0;
	if (capacity==0) return 0;
	if (txDataQueue->totalCnt <1)
	{

		return 0;//no data;
	}

    
	//EnterCriticalSection(&Section); 
        ptr = txDataQueue->ptrhead;
	if (ptr >= MAX_DATAITEM_NUMBER -1)
	{
		ptr = 0;
	}	
	pDataItem = &txDataQueue->dataItem[ptr];
	count = MIN(pDataItem->totalen,capacity);
	
	memmove(buf,&pDataItem->data[0],count);
	pDataItem->totalen -= count ;
	if (pDataItem->totalen >0)
	{
           memmove(&pDataItem->data[0],&pDataItem[count],pDataItem->totalen - count);   		   
	}else{
		txDataQueue->totalCnt--;
		txDataQueue->ptrhead++;
		if (txDataQueue->ptrhead >= MAX_DATAITEM_NUMBER -1)	
		txDataQueue->ptrhead = 0;
	}
		stream_source=fopen("D:\\source\\sio_raw_to_be_sent(bak).txt","w+");
		fwrite(&txDataQueue->dataItem[0].data[0],txDataQueue->dataItem[0].totalen +count,1,stream_source);
		fclose(stream_source);
        //LeaveCriticalSection(&Section);
	return count;
}
DLLAPI uint8 _stdcall svc_write_txDataQueue(TDataQueue * txDataQueue, char *buf, uint8 len, uint16 opt)
{
        uint8 ptr;	
	TDataItem *pDataItem;
	uint16 opt1 = opt;
	FILE* stream_source;
	
	if (txDataQueue == NULL) return 0;
	if (buf == NULL) return 0;
	if (len ==0) return 0;
	//EnterCriticalSection(&Section); 
        ptr = txDataQueue->ptrtail++;
	if (ptr >= MAX_DATAITEM_NUMBER -1)
	{
		ptr = 0;
	}	
	pDataItem = &txDataQueue->dataItem[ptr];
	pDataItem->totalen = len;
	memmove(&pDataItem->data[0],buf,len);	
	txDataQueue->totalCnt++;
		

    stream_source=fopen("D:\\source\\sio_raw_to_be_sent(txDataQueue).txt","w+");
	fseek(stream_source,0L,SEEK_SET); 		
    fwrite(&pDataItem->data[0],len,1,stream_source);
	fclose(stream_source);
    //LeaveCriticalSection(&Section);
	return len;
}

DLLAPI bool  _stdcall svc_routetable_add_addrItem(TSinkService *pTSinkServ,RouteAddr routeAddr,uint8 addrLen,uint8 opt)
{
        WORD nodeid;	
	uint8 ptr;	
	uint8 i;
        TRoutePathCache *pTRoutePathCache;	
	TRoutePathCacheItem *pTRouteCacheItem;	
	
	if (&routeAddr == NULL)	return FALSE;
	if (pTSinkServ == NULL) return FALSE;
	if (addrLen == 0) return FALSE;

	nodeid = routeAddr.dstAddr;
	pTRoutePathCache = &pTSinkServ->pathCache;
	if (pTRoutePathCache->totalCnt >= MAX_ROUTE_PATH_NUMBER) return FALSE;

	/*寻找该路由地址是否已经存在于地址表中；如果已经存
	在，则返回true;否则，在列表后面加入该路由地址。*/
	for(i=0;i< pTRoutePathCache->totalCnt;i++)
	{
	        pTRouteCacheItem = &pTRoutePathCache->RoutePathItem[i];
		if (pTRouteCacheItem->nodeid != nodeid) continue;	
		if (pTRouteCacheItem->length != addrLen) continue;
		for (ptr =0;ptr <addrLen;ptr++)
		{
			if (pTRouteCacheItem->path[ptr] != routeAddr.dstAddrItem[ptr].addr) break;                     				
		}

		if (ptr == addrLen -1)
		{/*所要加入的路由地址已经存在于路由缓冲区内；*/
			return TRUE;
		}
	}

	/*缓冲区中不存在路由地址，需要把传入的路由地址加入到缓冲区内；*/
        pTRouteCacheItem = &pTRoutePathCache->RoutePathItem[pTRoutePathCache->totalCnt++];
	pTRouteCacheItem ->nodeid = nodeid;
	pTRouteCacheItem->length = addrLen;
	for (i=0;i<addrLen;i++)
	{
		pTRouteCacheItem->path[i] = routeAddr.dstAddrItem[i].addr;
	}
	return TRUE;
}
bool _stdcall svc_select_routeCache(TSinkService *pTSinkServ,WORD srcNodeId,WORD destNodeId,TRoutePathCache *pRouteCache,uint8 opt)
 {
	return TRUE;
}
bool _stdcall svc_select_routeOptimalCache(TSinkService *pTSinkServ,WORD srcNodeId,WORD destNodeId,TRouteOptimalPathCache *pRouteCache,uint8 opt)
{
	return TRUE;
}

/*
@author zhangwei on 20061105
==========================================================================

目标
==========================================================================
在整个OpenM2M/OpenWSN体系中，提供sensor network和computer network(Internet)之间的一个gateway。它包含两条数据通路，一条上行，负责接收来自sensor networks的packet，并且缓存在本地，等待SCADA程序来读取。一条下行，负责接受SCADA的命令packet，并且发给sensor network。


设计思想
==========================================================================

关键点：实现两个communication channel
一个channel是通过UART与sink node通信，包含上行packet和下行packet。
一个channel是与SCADA Admin程序通信

Sink Service主程序则轮询两个channel，看是否有数据到达，若scada channel有数据来，则通过sink uart channel发送出去，若uart channel有数据来，则缓存在本地RAM中，等待Scada Admin来读取。

char m_txpacket[]
char m_rxpacket[];
queue m_rxqueue;
table m_routetable[];

svc_main()
{
  start_thread( sink_service_thread )
}

svc_thread()
{
  if (uart_read() > 0)
  {
    是否完整的数据包，如果不是，则将数据copy到m_rxpacket[]
    如果m_rxpacket中已经是一个完整的packet，则执行入队操作，push到m_rxqueue中；
    如果收到的数据包是路由维护request的反馈response，则不必送入队列，直接放入routing table即可。
       放进去之后可以再做路由优化处理 svc_optimize_routing()
  }

  if (m_txpacket中有待发送数据)
  {
    通过UART发送给sink node再送给整个网络。
  }
}



队列m_rxqueue可以利用STL queue对象以简化实现。

svc需要提供两个函数给SCADA Admin调用

uint16 svc_read( void * svc, char * buf, uint16 capacity, uint16 opt )
该函数供admin调用，将执行出队操作，将最先收到的数据从queue中取出，并且copy到buf所指向的空间中，返回实际copy的字节数


uint16 svc_write( void * svc, char * buf, uint16 len, uint16 opt )
该函数供admin调用，将buf数据写入到m_txpacket中，注意要写就全部写入，否则就不写。不能写成功时返回0，admin会负责retry的。


路由信息的维护主要也是在svc中做的。这样设计的目标是为了简化sensor node开发，并尽量简化sensor node软件。
路由维护的基本思路是svc通过sink node周期性广播path request数据包，每个sensor收到该packet都要进一步广播出去，最终网络中所有node都能收到。该packet在传输过程中需要记录它所经过的path。每个收到该packet的sensor node都要按照其中记录的path反向传输一个packet给sink node。sink node收到后给sink svc。这样,sink svc就能获得到任意node如何走的path信息。以后svc/admin要发送数据时，只要简单的从表里查询一下path即可。sink svc也可以对收到的path进行优化，计算出到每个node的最佳path。

service中内含一个timer对象(VC有的)，周期性的调用svc_timer_handler，执行网络维护工作，如路由维护
void svc_timer_handler()
{
  调用svc_write()发送路由维护packet
  svc_optmize_path()
}


typedef TRoutePathCache{
  nodeid;
  length;   // path length
  path[15]; // path. we don't consider those exceed 15 hop 
};

typedef TRouteOptimalPath{
  nodeid;
  path[10];  
}

typedef TSinkService{
  sinknode;
  maxhop;
  updateperiod;
  TRouteOptimalPath path[50]
  TRoutePathCache pathcache[200];
};

svc_optmize_path( TSinkService * svc )
{
  利用TRoutePathCache中存储的path信息，归并出从sink到每个destination sensor node的最优path，
  并且将optimal path存入TRouteOptimalPath中。
}

注意：最优path的标准每个人各有不同看法，对我们而言，最好是综合考虑这条路上的energy和path length.
但是，我们的电量估计也是个问题，虽然目前第二版硬件有支持，但电量估算还是有点难度。
所以我想这里的最优目前先按照最短路去理解吧，这样也是为了降低难度。


最终成果考核
==========================================================================

一个可供SCADA Admin GUI调用的库(DLL形式)，或者是一个可以独立运行的windows service。
出于简便，我想第一步还是先用DLL形式去做吧。

这样，唐文和黄欢以及后人都可利用这个成果，不至于每个人都独立作一套。
至于硬件和传感器读写等问题，你也可要黄欢等多承担一些，这样大家配合一下，早日完工。


至于主界面，我强烈建议你们三人用C#去开发（唐文作图像的可以例外），只有简单不会复杂。主界面你们3人倒是可以自己干自己的，不必统一。

至于主界面的框架程序，最简单的就是个对话框。如果要漂亮的，可以download DevExpress控件 for C#，有非常好的Example可以拿来改。
*/
