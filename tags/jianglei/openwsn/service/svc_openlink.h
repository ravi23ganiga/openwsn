#include "svc_foundation.h"
#include "svc_openmac.h"

#define CONFIG_MAX_NEIGHBOR_COUNT 8

typedef struct{
  uint16 id;
  uint8 state;
  uint8 addr[8];
  uint8 linkquality;
  uint8 signalstrength;
}TOpenNeighbor;

typedef struct{
  TOpenLLC * mac;
  TOpenNeighbor nodes[CONFIG_MAX_NEIGHBOR_COUNT];
}TOpenLLC;

TOpenLLC * llc_construct( void * buf, uint16 size );
void llc_destroy( TOpenLLC * llc ) 
int8 llc_probe( TOpenLLC * mac );
int8 llc_updatestatistics( TOpenLLC * mac );
int8 llc_getnode( TOpenLLC * mac, TOpenLLCNode * node );
int8 llc_getneighbors( TOpenLLC * mac, TOpenAddress * addr[] );
uint8 llc_getlinkquality( TOpenLLC * mac, TOpenAddress * addr );
uint8 llc_getsignalstrength( TOpenLLC * mac, TOpenAddress * addr );

