//----------------------------------------------------------------------------
// 全局变量模块
//----------------------------------------------------------------------------

#ifndef _GLOBAL_H_6748_
#define _GLOBAL_H_6748_

#include "configall.h"
#include "foundation.h"
#include "hal/hal_uart.h"
#include "hal/hal_timer.h"
#include "service/svc_siocomm.h"
#include "service/svc_queryengine.h"
#include "service/svc_network.h"
#include "service/svc_siocomm.h"

extern int             g_mode;
extern char *          g_owner;
extern TUart *         g_uart;
extern TSioComm *      g_sio;
extern TTimer *        g_timer;
extern TQueryEngine *  g_queryengine;
extern TOpenNetwork *  g_network;
extern TBinaryXml *    g_binaryxml;

int global_create( int mode );
int global_free();

#endif
