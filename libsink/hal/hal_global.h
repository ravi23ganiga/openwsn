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
#include "hal_uart.h"

#define g_uart g_uart0
#define g_debuguart g_uart1

extern TUartDriver *		g_uart0;
extern TUartDriver *		g_uart1;


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