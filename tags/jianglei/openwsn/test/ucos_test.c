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
 * @modified by zhangwei on 20061013
 * developed it as an uCOS-II test application.
 * 
 *******************************************************************************************************/

#include <stdlib.h>
#include "..\foundation.h"
#include "..\hal\hal.h"
#include "..\src\config.h"
#include "..\global.h"
#include "..\start.h"

/* Define the Task0 stack length 定义用户任务0的堆栈长度 
 * Define the Task0 stack 定义用户任务0的堆栈
 */
#define	TaskStkLengh	128		
OS_STK	TaskStk[TaskStkLengh];		

static void Task0( void * pdata );

/*********************************************************************************************************
**                            Main
********************************************************************************************************/
int ucos_main (void)
{
	OSInit ();	
	OSTaskCreate (Task0,(void *)0, &TaskStk[TaskStkLengh - 1], 2);		
	OSStart ();
	
	return 0;															
}

/*********************************************************************************************************
**                            Task0 任务0
********************************************************************************************************/

// @modified by zhangwei on 20070627
// add the call to app_start() in Task0.
//
void Task0( void * pdata )
{
	pdata = pdata;
	//TargetInit();
		
	OSTimeDly( 10 );

	OSTimeDly( 10 );
	// call app_start() in your own application;
	app_start();
}
   
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
