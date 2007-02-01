//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// 本程序只测试ARM 7内部的两个UART，它会把每一个收到的字符原封不动的送回去。
// 可通过超级终端、串口调试助手等测试本程序效果
// 
// 本程序测试通过后，将保证两个UART可以同时工作，不发生冲突
//----------------------------------------------------------------------------


#ifndef _OPENMAC_TEST_H_
#define _OPENMAC_TEST_H_
//----------------------------------------------------------------------------

#include "..\foundation.h"
#include "..\global.h"

void openmac_run( void );

//----------------------------------------------------------------------------
#endif