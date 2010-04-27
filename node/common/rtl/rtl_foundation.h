#ifndef _RTL_FOUNDATION_H_7689_
#define _RTL_FOUNDATION_H_7689_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

/*****************************************************************************
 * @name rtl_foundation.h
 * @author zhangwei on 20070331
 * 
 * system wide definitions for runtime library (RTL)
 * 
 * @history
 * @modified by xxx on 200704
 * 
 ****************************************************************************/ 

#include "rtl_configall.h"
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * @name rtl_foundation.h
 * @author zhangwei on 20070331
 *
 * system wide definitions for runtime library (RTL)
 *
 * @history
 * @modified by xxx on 200704
 *
 ****************************************************************************/

#ifdef CONFIG_DEBUG
    //#include <assert.h>
    #include "../hal/hal_assert.h"
    #define rtl_assert(cond) hal_assert((cond))
#else
    #define rtl_assert(cond)
#endif

struct _TiEvent{
  uintx id;
  void (* handler)(void * object, struct _TiEvent * e);
  void * objectfrom;
  void * objectto;
};
typedef struct _TiEvent TiEvent;

#define TiEventId uintx

typedef void (* TiFunEventHandler)(void * object, TiEvent * e);

#define min(x,y) (((x)<(y))?(x):(y))


#define STEP_CHECK(cond,retvar,retvalue) \
	if ((retvar >= 0) && (cond)) retvar = retvalue;

/*******************************************************************
  DEBUG macros
  These functions can be used in debug mode of the application.
*/

#ifdef CONFIG_DEBUG
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

//void* MemAlloc( size_t vsize );
//void  MemFree( void * vpMem );
//uint8 MemCompare( const byte * s1, const byte * s2, uint32 nLen );
//void  MemFillRandom( void * vpMem, const uint32 vLen );

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


//uint16 GetRandBetween( uint16 vMin, uint16 vMax );
//uint16 GetRandNum( uint16 vSeed );
//void DivideEx( uint32 v1, uint32 v2, uint32 * vpResult, uint32 * vpLeft );

#define InRange(start1,len1,start2,len2) (((start2<=start1) && (start1+len1<start2+len2))?TRUE:FALSE)

/* A structure to hold version information
 * A version can be changed to its string representations such as: "Version 2004, 4.3"
 */



//#define min(a, b)  (((a) < (b)) ? (a) : (b))
//#define max(a, b)  (((a) > (b)) ? (a) : (b))


//typedef uint32 version_t;
//#define MAKE_VERSION(major,minor,micro) (((major&0x0FF)<<16) | ((minor&0xFF)<<8) | (micro&0xFF))
//#define MAJOR_VERSION(ver) ((ver>>16) & 0xFF)
//#define MINOR_VERSION(ver) ((ver>>8) & 0xFF)
//#define MICRO_VERSION(ver) (ver&0xFF)

/* The following functions are often implemented as macros in a lot of simple C 
 * developing systems. Consider the side effect of macros, I prefer to use inline 
 * functions here instead of macros.
 *
 * #define MAKE_WORD(highbyte,lowbyte) ((((uint16)(highbyte)) << 8) | ((uint8)(lowbyte)))
 */ 
inline uint16 MAKE_WORD( uint8 highbyte, uint8 lowbyte )
{
	return ((((uint16)(highbyte)) << 8) | ((uint8)(lowbyte)));
}

inline uint8 HIGH_BYTE( uint16 word )
{
	return (uint8)(word >> 8);
}

inline uint8 LOW_BYTE( uint16 word )
{
	return (uint8)(word & 0xFF);
}


/*******************************************************************************
 * Network Byte Order
 ******************************************************************************/

/* Network order problem is encountered when transmit word or qword variables. It's
 * a very fundmental problem in network programing or porting to other platforms. 
 * 
 * According to the network byte order, the highest byte should be sent first. So
 * it's essentially a "Big Endian" design. Attention some protocol or hardware design
 * may not comply with this regulations. 
 *
 * In order to avoid the Little Endian/Big Endian problem in communications, and 
 * also helping the developer to write platform-independent programs, we define the 
 * following macros to help coding. These macros can often be found in a lot of 
 * powerful C language developing environment. 
 *
 *      htons()--"Host to Network Short"
 *      htonl()--"Host to Network Long"
 *      ntohs()--"Network to Host Short"
 *      ntohl()--"Network to Host Long"
 */

#if defined(CONFIG_BIG_ENDIAN) && !defined(CONFIG_LITTLE_ENDIAN)
	#define htons(A) (A)
	#define htonl(A) (A)
	#define ntohs(A) (A)
	#define ntohl(A) (A) 
#elif defined(CONFIG_LITTLE_ENDIAN) && !defined(CONFIG_BIG_ENDIAN)
	#define htons(A) ((((uint16)(A) & 0xff00) >> 8) | \
					   (((uint16)(A) & 0x00ff) << 8))
	#define htonl(A) ((((uint32)(A) & 0xff000000) >> 24) | \
					   (((uint32)(A) & 0x00ff0000) >> 8) | \
					   (((uint32)(A) & 0x0000ff00) << 8) | \
					   (((uint32)(A) & 0x000000ff) << 24))

	#define ntohs htons
	#define ntohl htohl
#else
	//#error "Either BIG_ENDIAN or LITTLE_ENDIAN must be #defined, but not both." 
#endif

/******************************************************************/
#ifdef __cplusplus
}
#endif

#endif
