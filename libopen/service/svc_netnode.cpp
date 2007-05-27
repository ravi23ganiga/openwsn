//----------------------------------------------------------------------------
// copyright
//----------------------------------------------------------------------------

#include "svc_configall.h"
#include "svc_netnode.h"
#include <stdlib.h>
#include <memory.h>

//----------------------------------------------------------------------------
// @author zhangwei on 20070521
// @modified by zhangwei on 20070522
//
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// TOpenNode
//----------------------------------------------------------------------------

DLLAPI TOpenNode * __stdcall netnode_create()
{
	TOpenNode * node;
	node = (TOpenNode *)malloc(sizeof(TOpenNode));
	if (node)
	{
		netnode_construct(node);
	}
	return node;
}

DLLAPI void __stdcall netnode_free( TOpenNode * node )
{
	if (node)
	{
		netnode_destroy( node );
		free( node );
	}
}

DLLAPI TOpenNode * __stdcall netnode_construct( TOpenNode * node )
{
	assert( node != NULL );
	memset( node, 0x00, sizeof(TOpenNode) );
	node->state = NODE_STATE_ACTIVE;
	return node;
}

DLLAPI void __stdcall netnode_destroy( TOpenNode * node )
{
	node->state = 0;
}

DLLAPI int _stdcall netnode_write( TOpenNode * node, char * buf, uint8 len, uint8 opt )
{
	// @TODO
	return 0;
}

DLLAPI int _stdcall netnode_read( TOpenNode * node, char * buf, uint8 capacity, uint8 opt )
{
	// @TODO
	return 0;
}

