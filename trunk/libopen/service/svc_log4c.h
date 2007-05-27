
#ifndef _SVC_LOGGER_H_4672_
#define _SVC_LOGGER_H_4672_

#include "svc_configall.h"
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

typedef struct{
  FILE * file;
}TLogger;

TLogger * log_open( TCHAR * fname, int opt );
void log_close( TLogger * log );
void log_write( TLogger * log, TCHAR * buf, int len, int opt );
void log_printf( TLogger * log, const TCHAR * f, ... );

#endif

