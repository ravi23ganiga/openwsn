//----------------------------------------------------------------------------
// copyright
//----------------------------------------------------------------------------

#include "../service/svc_configall.h"
#include <stdlib.h>
#include "../service/svc_netnode.h"
#include "svc_nodesimu.h"

//----------------------------------------------------------------------------
// @author zhangwei on 20070521
// @modified by zhangwei on 20070522
//	correct the error in simunode_write
//	and simunode_write() implementation also uses txbuf, so you can read the data
// that you have just wrotte. this may be appropriate for simulation.
//
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// TSimuNode
//----------------------------------------------------------------------------

DLLAPI TSimuNode * _stdcall simunode_create()  
{
	TSimuNode * node;
	node = (TSimuNode *)malloc(sizeof(TSimuNode));
	if (node)
	{
		simunode_construct(node);
	}
	return node;
}

DLLAPI void _stdcall simunode_free( TSimuNode * node )
{
	if (node)
	{
		simunode_destroy( node );
		free( node );
	}
}

DLLAPI TSimuNode * _stdcall simunode_construct( TSimuNode * node )
{
	assert( node != NULL );
	memset( node, 0x00, sizeof(TSimuNode) );
	node->state = 0;
	return node;
}

DLLAPI void _stdcall simunode_destroy( TSimuNode * node )
{
	node->state = 0;
}

DLLAPI int _stdcall simunode_write( TSimuNode * node, char * buf, uint8 len, uint8 opt )
{
	int copied = 0;
	
	if (node->txlen == 0)
	{
		copied = min( len, NODE_RXBUFFER_CAPACITY );
		memmove( node->txbuf, buf, copied );
		node->txlen = copied;
	}
	else 
		copied = 0;

	return copied;
}

DLLAPI int _stdcall simunode_read( TSimuNode * node, char * buf, uint8 capacity, uint8 opt )
{
	int copied = 0;

	if (node->txlen > 0)
	{
		copied = min( capacity, NODE_RXBUFFER_CAPACITY );
		memmove( buf, node->txbuf, copied );
		node->txlen = 0;
	}

	return copied;
}

