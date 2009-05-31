#ifndef _OSX_H_7698_
#define _OSX_H_7698_
/*****************************************************************************
 * OS kernel and its extension
 ****************************************************************************/ 

#include "osx_configall.h"
#include "osx_foundation.h"
#include "osx_kernel.h"
#include "osx_fifosche.h"
#include "osx_dba.h"
#include "osx_rtsche.h"

/* @attention
 *	if you encounter link error when you use osx, please add the necessary .c files
 * into your project.
 */


/*****************************************************************************
 * an template function to demostrate how to using the osx kernel in an application.
 ****************************************************************************/ 

/*
void osx_demo()
{
	target_init();
	osx_init();
	global_construct();
    uart_configure( g_uart, CONFIG_UART_BAUDRATE, 0, 0, 0, 0 );
    spi_configure( g_spi );
	// you can initialize your object relationships here
	osx_execute();
	global_destroy();
}
*/

#endif