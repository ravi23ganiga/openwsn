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
#ifndef _RTL_CONFIGALL_H_
#define _RTL_CONFIGALL_H_

/*******************************************************************
  @copyright(c)		2004-2010, XXX Tech. Co. Ltd
  @filename			sysconfigall.h
  @author			Zhang Wei
  @version			1.0
  @Date				2004-09-08
  @status			developing

  @description 
	This file defines some macros to re-configure the Universal 
	Programming Library. Every other file should include this file as
	their first included file to make the configure macros in this 
	file effective!

  @interface
	VOS_OPERATING_SYSTEM
	VOS_HARDWARE

  @implementation
		short description of some important algorithms here

  @history
  @modified by Zhang Wei on 2004-09-09
	  first created 
  @modified by Zhang Wei on 2004-09-10
	  revise the whole file and change its status from "developing" 
	  to "released"
*******************************************************************/

/*******************************************************************
  @attention
  -> This file must be the first included file in your files in order
  to make the configurations effective.

  set the whole system running in debug mode. if you want a release
  version, you should comment this macro.
*/

#define RTL_DEBUG
#define USER_DEBUG

/* If you want to build a Dynamic Link Library(DLL). please define
 * compiler macro BUILD_DLL as the following before including this
 * interface file:
 *		#define BUILD_DLL
 *
 * By default, the whole source code will be compiled as DLL
 */

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

/******************************************************************/
#endif
