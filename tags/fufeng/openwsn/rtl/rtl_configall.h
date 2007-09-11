/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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

#ifndef _RTL_CONFIGALL_H_
#define _RTL_CONFIGALL_H_

/*****************************************************************************
 * @name rtl_configall.h
 * @author zhangwei on 20070331
 * 
 * configure file for runtime library(RTL)
 * 
 * @history
 * @modified by xxx on 200704
 * 
 ****************************************************************************/ 

#include "../configall.h"

#define RTL_DEBUG
#define USER_DEBUG

/* If you want to build a Dynamic Link Library(DLL). please define
 * compiler macro BUILD_DLL as the following before including this
 * interface file:
 *		#define BUILD_DLL
 *
 * By default, the whole source code will be compiled as DLL
 */
/*
#if (!defined(BUILD_STANDALONE) && !defined(BUILD_APPLICATION))
  #define BUILD_DLL
#endif

#ifdef DLLFUNC
  #undef DLLFUNC
#endif

#ifdef BUILD_DLL
  #define DLLFUNC __declspec(dllexport)
#else
  #define DLLFUNC __declspec(dllimport)
#endif

#if defined( BUILD_APPLICATION )
  #define MODULE_INTERFACE __declspec(dllimport)
#elif defined( BUILD_DLL )
  #define MODULE_INTERFACE __declspec(dllexport)
#elif defined( BUILD_STANDALONE )
  #define MODULE_INTERFACE extern
#endif

#define MODU_INTF MODULE_INTERFACE
*/
/******************************************************************/
#endif
