#ifndef _SVC_INTEREST_4768_
#define _SVC_INTEREST_4768_

#define DD_CTRL_REQUEST 0x01
#define DD_CTRL_RESPONSE 0x02

typedef struct{
  uint8 ctrl;
  uint32 address;
  uint8 iid;
  uint8 seq;
  uint32 timestamp;
  void * payload;
}TiDdInterest;

typedef struct{
  TDdInterest[20] interests;
  uint8[20] age; 
}TiDdCache;

dd_cache_construct
dd_cache_destroy
dd_cache_insert
dd_cache_remove
dd_cache_find

#endif
