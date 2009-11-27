#ifndef _HAL_ASSERT_H_5748_
#define _HAL_ASSERT_H_5748_

/* modified by zhangwei on 20090711
 * @state
 * ok
 */

#include "hal_configall.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_DEBUG
  #define hal_assert(v) _hal_assert(v, __FILE__, __LINE__)
#else
  #define hal_assert(v) 
#endif

void _hal_assert( bool cond, char * file, uint16 line );

#ifdef __cplusplus
}
#endif

#endif
