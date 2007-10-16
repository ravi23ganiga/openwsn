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


#define min(x,y) ((x<y)?(x):(y))
#define max(x,y) ((x>y)?(x):(y))

#include <string.h>

#endif /*_FOUNDATION_H_*/
