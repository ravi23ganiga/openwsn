/****************************************Copyright (c)**************************************************
 *                               Guangzou ZLG-MCU Development Co.,LTD.
 *                                      graduate school
 *                                 http://www.zlgmcu.com
 *
 *--------------File Info-------------------------------------------------------------------------------
 * File name:			main.c
 * Last modified Date:  2004-09-16
 * Last Version:		1.0
 * Descriptions:		The main() function example template
 *
 *------------------------------------------------------------------------------------------------------
 * Created by:			Chenmingji
 * Created date:		2004-09-16
 * Version:				1.0
 * Descriptions:		The original version
 *
 *------------------------------------------------------------------------------------------------------
 * Modified by:
 * Modified date:
 * Version:
 * Descriptions:
 *------------------------------------------------------------------------------------------------------
 * @modified by zhangwei on 2006-08-15
 * eliminate some unnecessary code. and re-arrange the source style.
 * 
 *******************************************************************************************************/

#include <stdlib.h>
#include "foundation.h"
#include ".\src\config.h"
#include ".\hal\hal.h"
#include "start.h"

#ifdef CONFIG_USE_RTOS
#include ".\test\ucos_test.h"
#endif

int main (void)
{
	/* If you don't introduce an RTOS such as uCOS-II, just call app_start() here, 
	 * or else you may need to create some  threads here. */
	#ifndef CONFIG_USE_RTOS
	app_start();
	#endif

	#ifdef CONFIG_USE_RTOS
	ucos_main();
	#endif

	return 0;															
}

