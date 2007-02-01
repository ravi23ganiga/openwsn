#ifndef _HAL_GLOBAL_H_6382_
#define _HAL_GLOBAL_H_6382_

//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// Global Variables used in HAL layer 
//
// 	This module is very important in the whole application. It does not only 
// declares and initializes all the global objects, but also create the 
// relationships between different objects.
// 	In TinyOS and nesC systems, these relationships are constructed by a separate
// Configurator object. As you have seem here, this is not necessary. You should
// understand and adopt the OOP programming model and use it in such conditions.
//  
//----------------------------------------------------------------------------

#include "hal_foundation.h"
#include "hal_spi.h"
#include "hal_timer.h"
#include "hal_watchdog.h"
#include "hal_uart.h"
#include "hal_cc2420.h"
#include "hal_mcp6s.h"
#include "hal_ad.h"
#include "hal_sensor_vibration.h"
#include "..\service\svc_location.h"

/* these 4 definitions should be moved from HAL to service or application layer 
 */
  
#ifdef TARGET_OPENNODE_10  
#define g_spi g_spi0
#endif

#ifdef TARGET_OPENNODE_20  
#define g_spi g_spi0
#endif

#ifdef TARGET_WLSMODEM_11
#define g_spi g_spi0
#endif

#define g_uart g_uart0
#define g_debuguart g_uart1
#define g_timer g_timer1 

extern TUartDriver *		g_uart0;
extern TUartDriver *		g_uart1;
extern TWatchdog * 			g_watchdog;
extern TSpiDriver *			g_spi0;
extern TSpiDriver *			g_spi1;
extern TCc2420Driver *		g_cc2420;
extern TTimer * 			g_timer0;
extern TTimer * 			g_timer1;
extern TTimer * 			g_timer2;
extern TMcp6s26 *           g_mcp6s26;
extern TVibrationSensor *   g_vibration;
extern TAdConversion *      g_ad; 

extern TLocation g_loinfo;
extern TLocationService * g_loservice;

// construct all the necessary global object instances. The relationship between
// all these objects are also created.
// 
// @return 
//	0			success
//	negative	failed
//
int8 hal_global_construct( void );

// destroy all the global object instances and their relationships.
// 
// @return 
//	0			success
//	negative	failed
//
int8 hal_global_destroy( void );

#endif