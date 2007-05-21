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
