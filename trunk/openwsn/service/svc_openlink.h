#include "svc_foundation.h"
#include "svc_openmac.h"

#define CONFIG_MAX_NEIGHBOR_COUNT 8

typedef struct{
  uint16 id;
  uint8 state;
  uint8 addr[8];
  uint8 linkquality;
  uint8 signalstrength;
}TiOpenNeighbor;

typedef struct{
  TiOpenLLC * mac;
  TiOpenNeighbor nodes[CONFIG_MAX_NEIGHBOR_COUNT];
}TiOpenLLC;

TiOpenLLC * llc_construct( void * buf, uint16 size );
void llc_destroy( TiOpenLLC * llc ) 
int8 llc_probe( TiOpenLLC * mac );
int8 llc_updatestatistics( TiOpenLLC * mac );
int8 llc_getnode( TiOpenLLC * mac, TiOpenLLCNode * node );
int8 llc_getneighbors( TiOpenLLC * mac, TiOpenAddress * addr[] );
uint8 llc_getlinkquality( TiOpenLLC * mac, TiOpenAddress * addr );
uint8 llc_getsignalstrength( TiOpenLLC * mac, TiOpenAddress * addr );

