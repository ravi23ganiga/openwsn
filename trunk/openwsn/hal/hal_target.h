
#ifndef _HAL_TARGET_H_3792_
#define _HAL_TARGET_H_3792_

/***************************************************************************** 
 * @author zhangwei on 2006-07-20
 * Target utilities
 * 通常，target.*模块中包含目标硬件系统的配置，这些配置通常是电路板系统级的连接，
 * 它们放入MCU模块中并不合适，因为这些信息并不与特定的MCU有关。target中的代码
 * 很类似于许多RTOS中提到的BSP(board support package)概念。
 *
 * @modified by zhangwei on 20061013
 * first created
 * move the old PORT_INIT() and UART0_INIT() here.
 * 
 ****************************************************************************/

#include "hal_foundation.h"

/* initialize the target hardware 
 * this function MUST run successfully or the later code will dead.
 */
void target_init( void );

#endif