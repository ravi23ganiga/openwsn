#include "configall.h"
#include "ext_libopen.h"

//----------------------------------------------------------------------------
// @attention: for Microsoft Visual Studio Developers, please use this:
// #pragma comment( lib, "libopen" )
//
// for Borland C++ Builder developers, use this one:
// #pragma link "libopen_bcb.lib"
//----------------------------------------------------------------------------

#define CONFIG_COMPILER_MSVC

#ifdef CONFIG_COMPILER_MSVC
  #pragma comment( lib, "libopen" )
#else
  #pragma link "libopen_bcb.lib"
#endif

 