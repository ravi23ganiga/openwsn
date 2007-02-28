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
#ifndef _TK_FOUNDATION_H_
#define _TK_FOUNDATION_H_
/*******************************************************************
  @copyright(c)		2004-2010, XXX Tech. Co. Ltd
  @filename			sysdefn.h
  @author			Zhang Wei
  @version			1.0
  @Date				2004-09-08
  @status			developing

  @description 
    The most fundamental include file for Generic Programming Library. 
    Every file should include this basic .h file to get necessary defintions.

  @interface
  @implementation
		short description of some important algorithms here

  @history
  @modified by Zhang Wei on 2004-09-09
	  first created 
  @modified by Zhang Wei on 2004-09-10
	  revise the whole file and change its status from "developing" 
	  to "released"
*******************************************************************/

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <windows.h>

#include "vos/vos.h"
#include "tk_configall.h"

/*******************************************************************
*/

#define TK_MAX_ID8      0x7F
#define TK_MAX_UID8     0xFF
#define TK_MAX_ID16     0x7FFF
#define TK_MAX_UID16    0xFFFF
#define TK_MAX_ID32     0x7FFFFFFF
#define TK_MAX_UID32    0xFFFFFFFF

typedef int8    id8;
typedef uint8   uid8;
typedef int16   id16;
typedef uint16  uid16;
typedef int32   id32;
typedef uint32  uid32;

#define STEP_CHECK(cond,retvar,retvalue) \
	if ((retvar >= 0) && (cond)) retvar = retvalue;

/*******************************************************************
  DEBUG macros
  These functions can be used in debug mode of the application.
*/
#define TOOLKIT_DEBUG

#ifdef USER_DEBUG
	#define DEBUG_RUN(x) x
	#define NDEBUG_RUN(x)
    #define trace(x) printf("%s",(x))
    #define tracen(x,n) printf((x),(n))
    #define tracen2(x,n1,n2) printf((x),(n1),(n2))
    #define tracen3(x,n1,n2,n3) printf((x),(n1),(n2),(n3))
#else
	#define DEBUG_RUN(x) NULL
	#define NDENUG_RUN(x) x
    #define trace(x) NULL
    #define tracen(x,n) NULL
    #define tracen2(x,n1,n2) NULL
    #define tracen3(x,n1,n2,n3) NULL
    #define tracen4(x,n1,n2,n3,n4) NULL
    #define tracen5(x,n1,n2,n3,n4,n5) NULL
    #define tracen6(x,n1,n2,n3,n4,n5,n6) NULL
#endif

/*******************************************************************
  Utility Functions
  They can be used anywhere in the whole application.
*/

void* MemAlloc( size_t vsize );
void  MemFree( void * vpMem );
uint8 MemCompare( const byte * s1, const byte * s2, uint32 nLen );
void  MemFillRandom( void * vpMem, const uint32 vLen );

//void  SetLastError( int16 vErrorNo );
//int16 GetLastError();

/*
// @note: these functions assume IO port actually exists. remember that:
// although IO address is defined as int32, it must NOT exceed 16 bits in
// length as 16 is the maximun io address the intel X96 architecture can
// support.

uint8 IoRead8( uint32 vAddress );
uint16 IoRead16( uint32 vAddress );
uint32 IoRead32( uint32 vAddress );
void IoWrite8( uint32 vAddress, uint8 vData );
void IoWrite16( uint32 vAddress, uint16 vData );
void IoWrite32( uint32 vAddress, uint32 vData );

uint8 MemRead8( uint32 vAddress );
uint16 MemRead16( uint32 vAddress );
uint32 MemRead32( uint32 vAddress );
void MemWrite8( uint32 vAddress, uint8 vData );
void MemWrite16( uint32 vAddress, uint16 vData );
void MemWrite32( uint32 vAddress, uint32 vData );

#ifdef DEBUG_RAID
void DumpIo8( uint32 vAddr, char * vFormat );
void DumpIo16( uint32 vAddr, char * vFormat );
void DumpIo32( uint32 vAddr, char * vFormat );
void DumpMem8( uint32 vAddr, char * vFormat );
void DumpMem16( uint32 vAddr, char * vFormat );
void DumpMem32( uint32 vAddr, char * vFormat );
void DumpRam( uint8 * vpMem, uint32 vCount );
#else
#define DumpIo8 NULL
#define DumpIo16 NULL
#define DumpIo32 NULL
#define DumpMem8 NULL
#define DumpMem16 NULL
#define DumpMem32 NULL
#define DumpRam(v1,v2) NULL
#endif

#ifdef OS_NONE
#define min(a, b)	( ((a) <= (b)) ? (a): (b) )
#define max(a, b)	( ((a) >= (b)) ? (a): (b) )
#endif

//void TaskDelay( uint16 vMs );
//#define Equal(x,y) ((x)==(y))
*/

/*
 * define the BOOL data type. 
 * C language doesn't support bool data type, while the C++ does. The following
 * sections will define the bool data type if the current language is not C++.
 */
#ifndef __cplusplus
  #define BOOL int32 
  #define TRUE 1
  #define FALSE 0
  #define bool int8
  #define true 1
  #define false 0
#endif


uint16 GetRandBetween( uint16 vMin, uint16 vMax );
uint16 GetRandNum( uint16 vSeed );
void DivideEx( uint32 v1, uint32 v2, uint32 * vpResult, uint32 * vpLeft );

#define InRange(start1,len1,start2,len2) (((start2<=start1) && (start1+len1<start2+len2))?TRUE:FALSE)

/* A structure to hold version information
 * A version can be changed to its string representations such as: "Version 2004, 4.3"
 */

typedef struct{
  char ver_year[4];
  uint8  ver_major;
  uint8  ver_minor;
} TVersion;

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))

#define bit_set(token,index) (token |= (1<<index))
#define bit_get(token,index) ((token >> index) & 1)
#define bit_check(token,index) ((token >> index) & 1)
#define bit_clear(token,index) (token &= (~(1<<index)))

typedef uint32 version_t;
#define MAKE_VERSION(major,minor,micro) (((major&0x0FF)<<16) | ((minor&0xFF)<<8) | (micro&0xFF))
#define MAJOR_VERSION(ver) ((ver>>16) & 0xFF)
#define MINOR_VERSION(ver) ((ver>>8) & 0xFF)
#define MICRO_VERSION(ver) (ver&0xFF)


/******************************************************************/
#endif
