#include <windows.h>
//#include <stdio.h>
#include "service/svc_siocomm.h"
#include "service/svc_log4c.h"
#include "libsink.h"

//extern CWinThread *pThread; 
//extern TSioComm  *g_pTSioComm;

TSioComm   m_TSioComm;
TUartDriver  m_TUartDriver;
TUartDriver *g_pTUartDriver;
TSioComm  *g_pTSioComm;
//TSvrData svrData;
//CCriticalSection Section; 


//bool bend = FALSE;//定义的全局变量，用于控制线程的运行 
DLLAPI void * svc_create( uint16 id, uint16 opt )
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

	uart_configure(g_pTUartDriver,57600,8,1,0,0);
	//uart_open(g_pTUartDriver,"COM1");
	

	pSvcPack->pSioComm = sio_construct((char *)&m_TSioComm, 
			sizeof(TSioComm), 
			g_pTUartDriver, 
			0);//SIO_OPTION_FRAME_SUPPORT);	
	
	return (void *)pSvcPack;
}
DLLAPI void svc_start(void * svc)
{
   TSvcPacket *pSvcPack = (TSvcPacket *) svc;
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
}

DLLAPI void svc_stop(void *svc)
{
	TSvcPacket *pSvcPack = (TSvcPacket *)svc;

	if ( pSvcPack != NULL && pSvcPack->hthread != NULL)
	{ 
	  pSvcPack->cancel = true;	
	//bend=TRUE;//改变变量，线程结束 	
	  Sleep(1000);
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
			//{
				TerminateThread( pSvcPack->hthread, -1 );
				CloseHandle( pSvcPack->hthread );
			//}
		pSvcPack->hthread = NULL;
		//Beep(100,100); 

	  }
	}
}

//The Thread Function 
DWORD __stdcall svc_thread(LPVOID pParam)
{
	uint8 cnt;
	TSvcPacket *pSvcPack = (TSvcPacket *)pParam;
	TSvrData *pSvrData = NULL;
	char tempData[128];
	memset(tempData,0,sizeof(tempData));
	pSvrData = &pSvcPack->svrData;
	while(!pSvcPack->cancel) 
	{
	   //Beep(100,100); 
	//read data from rs232;
		cnt = sio_read(pSvcPack->pSioComm,&tempData[0], 128, 0);
	   if (cnt >0){
		
		svc_write_rxDataQueue(&pSvrData->rxDataQueue,(char *)&tempData[0], cnt, 0);	
                        
	  } else{ /* write the data into sio_txbuf if there are txQueueData sent.*/ 
		  cnt = svc_read_txDataQueue(&pSvrData->txDataQueue,(char*)&tempData[0], 128,0);
	    if (cnt >0)	sio_write(pSvcPack->pSioComm, &tempData[0], cnt, 0);
	 
	  }
	  //Sleep(1000); 
	} 

	free(tempData);
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

/*
void svc_timer_handler()
{

}

void svc_optmize_path( TSinkService * svc )
{
}
*/

uint8  svc_write_rxDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt)
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

uint8  svc_read_rxDataQueue(TDataQueue *rxDataQueue, char *buf, uint8 capacity, uint16 opt)
{
	uint8 ptr;	
	TDataItem *pDataItem;
	uint8 count;
	uint8 i;
	if (rxDataQueue == NULL) return 0;
	if (buf == NULL) return 0;
	if (capacity==0) return 0;
  	if (rxDataQueue->totalCnt <1) return 0;//no data;
	

	//EnterCriticalSection(&Section); 
    ptr = rxDataQueue->ptrhead;
	pDataItem = &rxDataQueue->dataItem[ptr];
	count = MIN(pDataItem->totalen,capacity);
	
	for (i=0;i<count;i++)
		buf[i] = pDataItem->data[i];
	//memmove(buf,&pDataItem->data[0],count);
	pDataItem->totalen -= count ;
	if (pDataItem->totalen >0)
	{
		memmove(&pDataItem->data[0],&pDataItem->data[count],pDataItem->totalen);   		   		
	}else{
		rxDataQueue->totalCnt--;
		rxDataQueue->ptrhead++;
		if (rxDataQueue->ptrhead >= MAX_DATAITEM_NUMBER -1)	
		rxDataQueue->ptrhead = 0;
	}
        //LeaveCriticalSection(&Section);
	return count;

}

 uint8  svc_read_txDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt)
{
	uint8 ptr;	
	TDataItem *pDataItem;
	uint8 count;
	uint16 opt1 = opt;
	//FILE *stream_source;
	
	if (txDataQueue == NULL) return 0;
	if (buf == NULL) return 0;
	if (capacity==0) return 0;
	if (txDataQueue->totalCnt <1) return 0;//no data;
	    
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
		memmove(&pDataItem->data[0],&pDataItem->data[count],pDataItem->totalen);   		   
	}else{
		txDataQueue->totalCnt--;
		txDataQueue->ptrhead++;
		if (txDataQueue->ptrhead >= MAX_DATAITEM_NUMBER -1)	
		txDataQueue->ptrhead = 0;
	}
        //LeaveCriticalSection(&Section);
	return count;
}

uint8  svc_write_txDataQueue(TDataQueue * txDataQueue, char *buf, uint8 len, uint16 opt)
{
    uint8 ptr;	
	TDataItem *pDataItem;
	uint16 opt1 = opt;

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
    //LeaveCriticalSection(&Section);
	return len;
}

DLLAPI int8 svc_uart_open( void * svc, char * name)
{
  TSvcPacket *pSvcPacket = (TSvcPacket *) svc;
  TUartDriver * uart = pSvcPacket->pSioComm->uart;
  return uart_open(uart,name);
}



DLLAPI int8 svc_uart_close(void *svc)
{
  TSvcPacket *pSvcPacket = (TSvcPacket *) svc;
  TUartDriver * uart = pSvcPacket->pSioComm->uart;
  return uart_close(uart);
}

DLLAPI int8 svc_uart_configure (void *svc,uint32 baudrate, uint8 databits, uint8 stopbits, uint8 parity, uint8 optflag )
{
  TSvcPacket *pSvcPacket = (TSvcPacket *) svc;
  TUartDriver * uart = pSvcPacket->pSioComm->uart;
  return uart_configure(uart,baudrate,databits,stopbits,parity,optflag);
}
