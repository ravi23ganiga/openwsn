
#include "svc_netnode.h"
#include <stdlib.h>

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
	node->state = 0;
	return node;
}

DLLAPI void __stdcall netnode_destroy( TOpenNode * node )
{
	node->state = 0;
}

DLLAPI int _stdcall netnode_write( TOpenNode * node, char * buf, uint8 len, uint8 opt )
{
	int copied = 0;
	
	if (node->rxlen == 0)
	{
		copied = min( len, NODE_RXBUFFER_CAPACITY );
		memmove( node->rxbuf, buf, copied );
		node->rxlen = copied;
	}
	return copied;
}

DLLAPI int _stdcall netnode_read( TOpenNode * node, char * buf, uint8 capacity, uint8 opt )
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

