#include <stdlib.h>
#include <string.h>
#include "svc_foundation.h"
#include "svc_dataqueue.h"

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