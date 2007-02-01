#ifndef _GLOBAL_H_6382_
#define _GLOBAL_H_6382_

//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// Global Variables with initialization and finalization
//
// 	This module is very important in the whole application. It does not only 
// declares and initializes all the global objects, but also create the 
// relationships between different objects.
// 	In TinyOS and nesC systems, these relationships are constructed by a separate
// Configurator object. As you have seem here, this is not necessary. You should
// understand and adopt the OOP programming model and use it in such conditions.
//  
//----------------------------------------------------------------------------

#include "foundation.h"
#include ".\hal\hal.h"
#include ".\service\svc_actsche.h"
#include ".\service\svc_siocomm.h"
#include ".\service\svc_wlscomm.h"
#include ".\service\svc_console.h"
#include ".\service\svc_debugio.h"
#include ".\service\svc_configure.h"

extern TActionScheduler * 	g_actsche;
extern TSioComm *			g_sio;
extern TWirelessComm *		g_wls;
extern TConsole *			g_console;
extern TConfigure *			g_config;
extern TDebugIo * 			g_debugio;


// construct all the necessary global object instances. The relationship between
// all these objects are also created.
// 
// @return 
//	0			success
//	negative	failed
//
int8 global_construct( void );

// destroy all the global object instances and their relationships.
// 
// @return 
//	0			success
//	negative	failed
//
int8 global_destroy( void );

#endif