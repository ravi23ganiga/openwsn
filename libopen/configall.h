/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available

 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#ifndef _CONFIGALL_H_4237_
#define _CONFIGALL_H_4237_

/*****************************************************************************
 * @author zhangwei on 2006-07-20
 * this is the configure file of the whole project
 * you can #define or #undef the macros in this file to enable/disable some 
 * software modules.
 * 
 * every module including re-configure functions SHOULD include this file as 
 * their first one. this is implemented by including "foundation.h". the "foundation.h"
 * has already included the "configall.h"
 * 
 * there're three ways for the host to communicate with the wsn network through 
 * libopen. it is: RS232(UART), RS485(UART) and USB
 *
 * @modified by zhangwei on 2006-07-20
 * @modified by zhangwei on 20061015
 * add macro CONFIG_MAX_FRAME_LENGTH
 * 
 * @modified by zhangwei on 20061-26
 * add macros to support different hardware platforms
 *
 * @modified by zhangwei on 20070503
 * made revision on the "configall.h"
 *
 ****************************************************************************/
 

/*****************************************************************************
 * User Changable Macros
 ****************************************************************************/

#define CONFIG_MAJOR_VERSION 0
#define CONFIG_MINOR_VERSION 9
#define CONFIG_MICRO_VERSION 1

/* CONFIG_VERSION_10 
 * is developed by Zhou Songli. it is used with ICT's GAINZ and Zhou's routing
 * 
 * CONFIG_VERSION_20 (default)
 * is developed by zhangwei and xxx. it is used with OpenWSN Software on ARM nodes.
 */
#undef  CONFIG_VERSION_10
#define CONFIG_VERSION_20

// macro to enable the debug source code. for release version, you should undef this macro
#define CONFIG_DEBUG

// config the "libopen"'s host environment. it may be windows host, linux host
// and PDA host(ruuning WinCE or uCLinux)
#define CONFIG_OS_WINDOWS
#undef  CONFIG_OS_UCLINUX
#undef  CONFIG_OS_WINCE

// @modified by zhangwei on 20070205 
//  CONFIG_HARDWARE_ATMEL 
//  CONFIG_HARDWARE_ARM 
// these macros will help to choose the hardware platform it served for.
// actually, these two macros are not necessary. but in this transition period, i think it's 
// useful to keep they two. in the future, the Atmel GAINZ and ARM OpenWSN will share the 
// same libsink source code.
//
#define CONFIG_HARDWARE_ATMEL
#undef  CONFIG_HARDWARE_ARM

// control this application compiled as a standard application rather than a DLL
#define CONFIG_APPLICATION
#undef	CONFIG_APPLICATION


/*****************************************************************************
 * User Un-Changable Macros
 ****************************************************************************/

// define the application can be used from Win2000 OS.
#define _WIN32_WINNT 0x0500

#ifdef CONFIG_DEBUG
  #define GDEBUG
#else
  #undef GDEBUG 
#endif

#ifdef CONFIG_APPLICATION
#define DLLAPI extern "C" _declspec(dllimport)
#else
#define DLLAPI extern "C" _declspec(dllexport)
#endif

// macro _AFXDLL will tell the compiler and linker to compile the project as 
// DLL and using ATL/MFC. attention that MFC application can NOT include "windows.h".
// we needn't to be a MFC-enable DLL.
//
// macro _CRT_SECURE_NO_DEPRECATE will tell the compiler do NOT report warning 
// messages will unsafe string functions (in VS 2005 only).
// this macro is used to eliminate "deprecated function" warnings by VS2005
//
#undef _AFXDLL
#define _CRT_SECURE_NO_DEPRECATE

// this DLL needs to be a UNICODE DLL. 
#ifndef _UNICODE
  #define _UNICODE
#endif

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif


/* 串口SIO Frame的开始标志字节
 * 对流式Modem而言，不使用该标志
 */
#define CONFIG_START_OF_FRAME 27
#define CONFIG_MAX_UART_FRAME_LENGTH 127

/* UART匹配采用RS232还是485
 */
#undef CONFIG_UART_RS232
#define CONFIG_UART_RS232

#define CONFIG_UART_RS485
#undef CONFIG_UART_RS485

#ifdef CONFIG_UART_RS232
  #undef CONFIG_UART_RS485
#endif  


/* maximum frame length. no more than 127*/
//#define CONFIG_MAX_PHY_FRAME_LENGTH 0x7F
//#define CONFIG_MAX_MAC_FRAME_LENGTH (CONFIG_MAX_PHY_FRAME_LENGTH-9)
//#define CONFIG_MAX_UART_FRAME_LENGTH 0x7F

/* UART frame identification char */
//#define CONFIG_DEFAULT_ESCAPE_CHAR 27
//#define CONFIG_DEFAULT_FRAME_PREFIX CONFIG_DEFAULT_ESCAPE_CHAR

/*****************************************************************************
 * Type define for all the modules
 ****************************************************************************/

#ifndef NULL
  #define NULL 0
#endif

#ifndef __cplusplus
  #define true  1
  #define false 0
#endif  


#define int8 signed char		
#define int16 signed short        
#define int32 signed long         
#define int64 signed long long    

#define uint8 unsigned char       
#define uint16 unsigned short      
#define uint32 unsigned long       
#define uint64 unsigned long long  

#define fp32 float      
#define fp64 double     

#endif