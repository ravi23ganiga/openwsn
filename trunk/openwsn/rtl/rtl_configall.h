#ifndef _TK_CONFIGALL_H_
#define _TK_CONFIGALL_H_
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

#define TOOLKIT_DEBUG
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
