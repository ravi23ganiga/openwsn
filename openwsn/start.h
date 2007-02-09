	//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// this module functions as the main application entry. 
// for uCOS-II, you should call app_start() in a separate task
// for Non-uCOS and Non OS-based application, you can simply call app_start()
// after you boot the system

#ifndef _START_H_5784_
#define _START_H_5784_
 
#include "foundation.h"
#include "global.h"
#include ".\test\simplerouter.h"
#include ".\test\uartecho.h"
#include ".\test\debuguart.h"
#include ".\test\sioecho.h"
#include ".\test\sniffer.h"
#include ".\test\wlsecho.h"
#include ".\test\cc2420echo.h"
#include ".\test\analyzer.h"
#include ".\test\wlsmodem.h"
#include ".\test\cc2420dev_test.h"
#include ".\test\uartanddebug.h"
#include ".\test\uartecho.h"
#include ".\test\location_test.h"
#include ".\test\uniqueid_test.h"
#include ".\test\sensor_test.h"
#include ".\test\openmac_test.h"
#include ".\test\openmac_whitehole.h"
#include ".\test\openmac_blackhole.h"

void app_start( void );

#endif
