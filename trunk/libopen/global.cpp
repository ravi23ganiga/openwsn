#include "configall.h"
#include "global.h"
#include "simu/svc_netsimu.h"

//----------------------------------------------------------------------------
// Global
//----------------------------------------------------------------------------

static TUart m_uart;
static TTimer m_timer;
static TSioComm m_sio;
static bool m_init = false;

int             g_mode = 0;
char *          g_owner = "OpenWSN Group @ TongJi University, 2005-2007";
TUart *         g_uart = NULL;
TSioComm *      g_sio = NULL;
TTimer *        g_timer = NULL;
TQueryEngine *  g_queryengine = NULL;
TOpenNetwork *  g_network = NULL;
TBinaryXml *    g_binaryxml = NULL;

// mode = 0(default)	simulation
// mode = 1				running with real network
//
int global_create( int mode )
{
    if (!m_init)
    {
		memset( &m_uart, 0x00, sizeof(TUart) );
		memset( &m_timer, 0x00, sizeof(TTimer) );
		
		g_mode = mode;
		g_timer = timer_construct( &m_timer, sizeof(TTimer) );
		g_uart = uart_construct( 0, (char *)(&m_uart), sizeof(TUart) );
		g_sio = sio_construct( (char*)(&m_sio), sizeof(TSioComm), g_uart, 0x00 );
		g_binaryxml = NULL;
		g_network = (mode ? opennet_create(g_sio,g_timer) : simunet_create(g_sio,g_timer));
		g_queryengine = query_engine_create( g_network );
		m_init = true;
    }

    return 0;   	
}

int global_free()
{
    if (m_init)
    {
		timer_destroy( g_timer );
		uart_destroy( g_uart );
		sio_destroy( g_sio );
		query_engine_free( g_queryengine );
		(g_mode ? opennet_free(g_network) : simunet_free(g_network));
        m_init = false;
    }	
	return 0;
}


