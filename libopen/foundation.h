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

#ifdef _AFXDLL
  #include <afx.h>
#else
  #define WIN32_LEAN_AND_MEAN		
  #include <windows.h>
#endif

#ifndef NULL
  #define NULL 0
#endif

#ifndef __cplusplus
  #define true  1
  #define false 0
#endif  

// Boolean
//typedef unsigned char BOOL;
//typedef bool       BOOL;

//typedef BOOL boolean;

// Data
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef unsigned long long  QWORD;
//typedef unsigned char    MAC_ENUM;

// Unsigned numbers
typedef unsigned char       UINT8;
typedef UINT8                   uint8;
typedef unsigned short      UINT16;
typedef UINT16                 uint16;
typedef unsigned long       UINT12;
typedef UINT12                uint12;
typedef unsigned long long  UINT64;
typedef UINT64                   uint64;

typedef signed int 		int32;
typedef unsigned long 	uint32;

typedef float          	fp32;             /* single precision floating point variable (32bits) 单精度浮点数（32位长度） */
typedef double         	fp64;             /* double precision floating point variable (64bits) 双精度浮点数（64位长度） */

// Signed numbers
typedef signed char         INT8;
typedef INT8                   int8;
typedef signed short        INT16;
typedef INT16                  int16;
typedef signed long         INT12;
typedef INT12                  int12;
typedef signed long long    INT64;
typedef INT64                   int64;

//#define min(x,y) ((x<y)?(x):(y))
//#define max(x,y) ((x>y)?(x):(y))
//-------------------------------------------------------------------------------------------------------
// Useful stuff
#define BM(n) (1 << (n))
#define BF(x,b,s) (((x) & (b)) >> (s))
#define MIN(n,m) (((n) < (m)) ? (n) : (m))
#define min(n,m)  (((n) < (m)) ? (n) : (m)) 
#define MAX(n,m) (((n) < (m)) ? (m) : (n))
#define ABS(n) ((n < 0) ? -(n) : (n))

#define addrof(arr) (&(arr[0]))
#define msgbox(text,flag) MessageBox(NULL,(text),_T("libfsys"),(flag))


#endif /*_FOUNDATION_H_*/
