#ifndef _SVC_NODESIMU_H_7388_
#define _SVC_NODESIMU_H_7388_
//----------------------------------------------------------------------------

#include "../service/svc_configall.h"
#include "../service/svc_netnode.h"

//----------------------------------------------------------------------------
// TSimuNode 
//----------------------------------------------------------------------------

// this simulation network has the same interface with TSimuNode and TOpenNetwork 
// define in svc_netnode.h / svc_network.h
//
#define TSimuNode TOpenNode

DLLAPI TSimuNode * _stdcall simunode_create();
DLLAPI void _stdcall simunode_free( TSimuNode * node );
DLLAPI TSimuNode * _stdcall simunode_construct( TSimuNode * node );
DLLAPI void _stdcall simunode_destroy( TSimuNode * node );
DLLAPI int _stdcall simunode_write( TSimuNode * net, char * buf, uint8 len, uint8 opt );
DLLAPI int _stdcall simunode_read( TSimuNode * net, char * buf, uint8 capacity, uint8 opt );

//----------------------------------------------------------------------------
#endif