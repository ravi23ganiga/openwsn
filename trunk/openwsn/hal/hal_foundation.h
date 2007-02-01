#ifndef _HAL_FOUNDATION_H_5342_
#define _HAL_FOUNDATION_H_5342_

/*******************************************************************************
 * @author zhangwei on 2006-07-24
 * 	this is the foundation header of HAL. every file in "hal" directory should 
 * include this one or "hal_configall.h" as their first header file in order 
 * to make the definition and declarations in this file effective.
 * 
 * @history
 * @modified by zhangwei on 2006-07-24
 * 		this file replace the old "include.h".
 * @modified by zhangwei on 2006-09-01
 * 		add type TInterruptHandler and TEventHandler. these two are function 
 * 		pointer types. they are often used when setting callback functions.
 ******************************************************************************/

#include <string.h>
#include "..\foundation.h" 
#include "hal_configall.h"

typedef unsigned char		BOOL;

//#define BOOL unsigned char;

#define BYTE uint8
#define WORD uint16
#define DWORD uint32
#define QWORD uint64

#define UINT8 uint8
#define UINT16 uint16
#define UINT32 uint32
#define UINT64 uint64

#define INT8 int8
#define INT16 int16
#define INT32 int32
#define INT64 int64

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE 1
#endif
#ifndef NULL
	#define NULL 0
#endif

// Useful stuff
#define BM(n) (1 << (n))
#define BF(x,b,s) (((x) & (b)) >> (s))
#define MINI(n,m) (((n) < (m)) ? (n) : (m))
#define MAX(n,m) (((n) < (m)) ? (m) : (n))
#define ABS(n) ((n < 0) ? -(n) : (n))

// Dynamic function pointer
typedef void (*VFPTR)(void);

/* TFunInterrupt
 * represent the hardware's interrupt service routine (ISR)
 *
 * TFunCallback
 * a standard callback function pointer type
 */
typedef void (* TInterruptHandler)( void ); 
typedef int8 (* TEventHandler)( void * data ); 

#ifndef assert
#define assert(v)  while (v) {NULL;}
#endif

#endif
