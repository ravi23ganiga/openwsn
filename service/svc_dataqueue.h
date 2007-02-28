
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

uint8  svc_write_rxDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt);
uint8  svc_read_txDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt);
uint8  svc_write_txDataQueue(TDataQueue * rxDataQueue, char *buf, uint8 len, uint16 opt);
uint8  svc_read_rxDataQueue(TDataQueue *txDataQueue, char *buf, uint8 capacity, uint16 opt);