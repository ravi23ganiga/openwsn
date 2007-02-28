
#include "svc_foundation.h"
#include ".\hal\hal_cpu.h"
#include "svc_debugio.h"

// there's only one TDebugIo objects exists in the application.
//
static uint8 m_instance_count = 0;

TDebugIo * debug_construct( char * buf, uint16 size, TUartDriver * uart )
{
	TDebugIo * db;
	
	if (m_instance_count == 0)
		db = (sizeof(TDebugIo) <= size) ? (TDebugIo *)buf : NULL;
	else
		db = NULL;

	if (db != NULL)
	{
		memset( buf, 0x00, sizeof(TDebugIo) );
		db->uart = uart;
		db->datalen = 0;
		m_instance_count ++;
	}
	
	return db;
}

void debug_destroy( TDebugIo * db )
{
	if (m_instance_count > 0)
		m_instance_count --;
}

/* this function should be called frequvently to send data to UART
 * or else all your debug operations only place data in the internal buffer
 * without sending them out.
 */
void debug_evolve( TDebugIo * db )
{
	uint16 count;
	char * buf;
	if (db->datalen > 0)
	{
		hal_enter_critical();
		count = uart_write( db->uart, db->buf, db->datalen, 0 );	
		db->datalen -= count;
		buf = (char *)(&(db->buf[0]));
		memmove( buf, buf + count, db->datalen );
		hal_leave_critical();
	}
}

uint16 debug_write( TDebugIo * db, char * buf, uint16 size )
{
	uint16 copied;
	
	hal_enter_critical();
	copied = min( db->datalen, size );
	if (copied > 0)
	{
		memmove( db->buf + db->datalen, buf, copied );
		db->datalen += copied;
	}
	hal_leave_critical();
	
	return copied;
}

