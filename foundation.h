#ifndef _FOUNDATION_H_3721_
#define _FOUNDATION_H_3721_

/*******************************************************************************
 * @author zhangwei on 20060906
 * foundation.h
 * this is the base foundation file of all the files in the application.
 * 
 * @modified by zhangwei on 20060906
 * add "signed" in the typedef of int8
 * this is because some compile will assume char as unsigned type, while here 
 * we need a signed char.
 ******************************************************************************/
 
#include "configall.h"

#ifndef NULL
  #define NULL 0
#endif

#ifndef __cplusplus
  #define true  1
  #define false 0
#endif  

typedef signed char 	int8;
typedef signed short 	int16;
typedef signed int 		int32;
typedef long long 		int64;
typedef unsigned char 	uint8;
typedef unsigned short 	uint16;
typedef unsigned long 	uint32;
typedef unsigned long long uint64;
typedef unsigned char 	boolean;
typedef float          	fp32;             /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         	fp64;             /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */


#define min(x,y) ((x<y)?(x):(y))
#define max(x,y) ((x>y)?(x):(y))

#endif /*_FOUNDATION_H_*/
