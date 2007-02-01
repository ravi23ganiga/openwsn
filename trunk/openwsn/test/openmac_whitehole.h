#ifndef _OPENMAC_WHITEHOLE_H_4788_
#define _OPENMAC_WHITEHOLE_H_4788_

#include "..\hal_foundation.h"

/* whitehole 
 * 是一个单纯发送openmac frame的测试工具
 * 目前主要是与openmac_test测试配合使用，在openmac_test工作的时候，让whitehole发送
 * 数据frame进行干扰。
 */
void openmac_whitehole( void );

#endif

