#ifndef _FOUNDATION_H_4278_
#define _FOUNDATION_H_4278_

/***************************************************************************** 
 * @author zhangwei on 2006-08-20
 * this is the foundation file of service layer.
 * every service modules in this directory should include this one as their first file.
 * 
 *****************************************************************************/

#include "../foundation.h"
#include "svc_configall.h"

#ifdef STANDARD_APPLICATION
#define DLLAPI extern "C" _declspec(dllimport)
#else
#define DLLAPI extern "C" _declspec(dllexport)
#endif

// macro _AFXDLL will tell the compiler and linker to compile the project as DLL and using ATL/MFC
// attention that MFC application can NOT include "windows.h".
//
// macro _CRT_SECURE_NO_DEPRECATE will tell the compile do NOT report warning messages will unsafe string 
// functions.
//
#define _AFXDLL
#define _CRT_SECURE_NO_DEPRECATE

#ifdef _AFXDLL
//#include <afx.h>
#endif

#ifndef _AFXDLL
//#define WIN32_LEAN_AND_MEAN		
//#include <windows.h>
#endif


#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

#define addrof(arr) (&(arr[0]))
#define msgbox(text,flag) MessageBox(NULL,(text),_T("libfsys"),(flag))


#endif
