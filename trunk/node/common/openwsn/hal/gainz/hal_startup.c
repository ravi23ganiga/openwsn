#include "../hal_startup.h"

/* 
 * @modified by zhangwei in 2009008
 * - replace the previous hal_boot module.
 */

/* @attention
 *  - This module contains only one function "target_startup()". This function is
 * generally called by the startup.s asssemble module. 
 * 
 * for AVR Studio and WinAVR developing environment, the WinAVR has already provides
 * an startup assemble module and it will call C language's main() function automatically,
 * the target_startup() can keep empty here. You needn't call this function in your
 * main().
 * 
 *  - During the startup process, all the interrupts should be disabled. Generally,
 * it's enabled after the whole application is startup successfully.
 */

void target_startup()
{
    return;
}

