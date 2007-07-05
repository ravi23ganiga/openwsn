#include "libopen_test.h"

void main(void)
{
	TOpenNetwork * net;

	// 0x00 for simulation and 0x01 for real network
	svc_create( 0x00 );
	net = svc_get_network();

	// do net_read, net_write, etc
	// printf net_node(...)

	svc_free();
}
