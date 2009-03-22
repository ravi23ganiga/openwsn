#ifndef _ARCH_CONFIGALL_H_4328_
#define _ARCH_CONFIGALL_H_4328_

/******************************************************************************
 * @author zhangwei on 2006-08-30
 * hal is the summary header file of all HAL header files to faciliate using
 * in other modules. attention that all the "*.h" file in the "hal" directory  
 * should NOT include "hal.h".
 * 
 * @attention
 * HAL中的对象及其方法要保持最大程度的简明．每一个接口变量和函数，能不增加就不增加．这样做
 * 是为了最大限度简化HAL在不同硬件平台上的移植．
 * 
 * @modified by zhangwei on 20061013
 * made great revision today. 
 * this is just the style of future hal.h. you should only include other hal header
 * files here! so other modules in the application can include "hal.h" only. 
 *****************************************************************************/

#include "../hal_configall.h"

#endif

