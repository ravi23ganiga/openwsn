#include "svc_foundation.h"
#include <string.h>
#include <stdarg.h>
#include "svc_log4c.h"

#define LOG_TBUF_SIZE 255
static TCHAR m_log_tbuf[LOG_TBUF_SIZE];

TLogger * log_open( TCHAR * fname, int opt )
{
    TLogger * log;

    log = (TLogger *)malloc( sizeof(TLogger) );
    if (log != NULL)
    {
        memset( log, 0x00, sizeof(TLogger) );
        //log->file = _tfopen( fname, _T("wt+") );
		log->file = _tfopen( fname, _T("wt+") );
    }

    return log;
}

void log_close( TLogger * log )
{
    if (log != NULL)
    {
        fclose( log->file );
        free( log );
    }
}

// @attention, "len" is the item count according to char type.
//
void log_write( TLogger * log, TCHAR * buf, int len, int opt )
{
    if ((log != NULL) && (log->file != NULL))
    {
        fwrite( buf, len, 1, log->file );
        fflush( log->file );
    }
}

// the last TCHAR of buffer will always be TCHAR(0). this is safe for
// string operations.
//
void log_printf( TLogger * log, const TCHAR * f, ... )
{
    if ((log != NULL) && (log->file != NULL))
    {
        memset( m_log_tbuf, 0x00, sizeof(m_log_tbuf) );
        va_list argptr;
        va_start(argptr, f);
        //_vsntprintf( m_log_tbuf, sizeof(m_log_tbuf) - sizeof(TCHAR), f, argptr );
        _vftprintf( log->file, f, argptr );
        va_end(argptr);
        //log_write( log, m_log_tbuf, sizeof(m_log_tbuf) - sizeof(TCHAR), 0x00 );
        fflush( log->file );
    }
}
