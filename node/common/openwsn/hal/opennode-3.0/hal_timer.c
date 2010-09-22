/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However t#include "hal_openframe.h"he source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#include "hal_foundation.h"
#include "hal_cpu.h"
#include "hal_timer.h"
//for test

#include "hal_led.h"

/*******************************************************************************
 * @author zhangwei, tangwen on 2006-08-02
 * Timer hardware encapsulation
 * 
 * you can reference "test\timer.*" to see how to use the TiTimerAdapter object.
 * 
 * @history
 * @modified by zhangwei on 20060802
 * integrate Tangwen's new Timer code today. but not test.
 * 
 * @modified by zhangwei on 20061021
 * add channel support. 
 * testing needed.
 *
 * @modified by shanlei on 200610xx
 * corrected by shanlei and test passed.
 * 
 * @modified by zhangwei 2006-09-05
 * @modified by tangwen on 2006-10-21
 * 	- add channel parameter to construct function.
 *  - and other modifications to support the channel(sub-timer).
 *
 * @modified by Shanlei on 200611
 *  timer����ͨ�������汾˵�����£�
 *  timer_setvalue �� timer_setcaptureΪ�պ�����
 *  timer_configure��priority������ʾ��timer�����Ƿ�ʹ���жϷ�ʽ��priority
 * Ϊ0-15��ʾʹ���жϷ�ʽ��>15��ʾ��ʹ���жϡ�
 *  timer_VICinitֻ�Ǹ���timer�����id �� priority ������VIC��ַ��priority
 * ��ֵֻ��Ϊ0-15������Χ�������ַ���˺�����timer_configure�����е��ã���
 * timer_configure�Ĳ���priority��0-15֮�䡣ԭ������timer��VIC��ַΪͬһ��
 *��timer_interrupt���������Է��֣�������timerͬʱ�����жϵ�ʱ����Ϊʹ��ͬһ
 * ��ַ���������ͻ������Ϊÿһ��timer����һ��VIC��ַ��Timer0_Int for timer0 
 * and Timer1_Int for timer1����������ֳ�ͻ��
 *  ��timer_VICinit�з����timer_VICenable��timer_VICdisable��������timer��
 * ʹ�ܻ��ֹ��������������timer_start��timer_stop�и���timer->state�����á�
 *  timer_setinterval�ļ�ʱ��С�����Ϊ1ms������interval��ֵ����ms����
 * repeat���ָܻ���repeatΪ0��ֻƥ��һ�Σ�repeatΪ1������ƥ�䡣
 *  timer_enable��timer_disable�Ƕ�timer��ÿ���Ƚ�ͨ���жϵ�ʹ�ܻ��ֹ����
 * ʹ��timer_expired�Ĳ�ѯ��ʽ�£�Ҳ��Ҫʹ�ܱȽ�ͨ�����жϣ��Ա�����жϱ�־��
 * ��ˣ�����������������Ϊtimer����ʹ���жϷ�ʽ�ı��֣����ǽ����ı�timer-
 * >state��
 *   timer_VICinit��timer_VICenable��timer_VICdisable��timer_enable��
 * timer_disable���ϲ㶼�ǲ��ɼ��ġ�
 *
 * modified by zhangwei on 200612
 * timer��������ֹͣ����ʹ��start/stop����Ҫʹ��disable��enable
 ******************************************************************************/


static void __irq timer_interrupt( void );


// construct a TiTimerAdapter object in the memory
//
// @param
//	id 			timer id. now it can be 0 or 1
//	channel
//	buf
//	size
//	callback	the callback function when the timer expired/triggered.
//				attention the callback is essentially a part of the interrupt
//				service routine. 
//				The value of "callback" can be NULL
//	data		the parameter to be passed to the callback function.
//	priority	the interrupt priority level. 0 is the highest and 15 is the lowest.
//
TiTimerAdapter * timer_construct( char * buf, uint8 size )
{
	TiTimerAdapter * timer;
	
	if (sizeof(TiTimerAdapter) > size)
	{
		timer = NULL;
	}
	else{
		timer = (TiTimerAdapter *)buf;
	}	
	if (timer != NULL)
	{
		
		memset( buf, 0x00, sizeof(TiTimerAdapter) );
	}
	
	return timer;
}

// to be finished:
// should disconnect the interrupt here
// modified by shanlei
void timer_destroy( TiTimerAdapter * timer )
{
	timer_disable( timer );
	if (timer->state & TIMER_STATE_INTERRUPT)
	{
	    timer_VICdisable( timer );	
	}
	timer = NULL;
}

void timer_init( TiTimerAdapter * timer, uint8 id, uint8 channel )
{
	if (timer != NULL)
	{
		memset( timer, 0x00, sizeof(TiTimerAdapter) );
		timer->id =  ((id & 0x0F) << 4 | (channel & 0x0F));
		timer->state = 0;
		timer->interval = 0;
		timer->data = NULL;
		timer->callback = NULL;
		timer->priority = 0;
		timer->lasttime = 0;
	}
}

// @param
// 	callback	callback function of the timer. 
//				if it is NULL, the timer's interrupt will automatically disabled
// 	priority	�ж����ȼ���0��ߣ�15��͡���������˷�Χ��Ĭ��Ϊ��߼���
//  modified by shanlei 
//  the use of priority : �ж����ȼ���0��ߣ�15��͡��������15����ʾ��ʹ���жϡ�Ĭ��Ϊ��߼�
void timer_configure( TiTimerAdapter * timer, TiFunEventHandler callback, void * data, uint8 priority )
{
	timer->priority = priority;
	timer->data = data;
	timer->callback = callback;
	
	if( timer->priority > 15)
	{
	    timer->state &= (~TIMER_STATE_INTERRUPT);
	}    
	else if(timer->callback != NULL)
	{
		timer_VICinit( timer );
		timer->state |= TIMER_STATE_INTERRUPT;
	}
	else
	{
		timer->state &= (~TIMER_STATE_INTERRUPT);
	}
}

// set the timestamp of the timer 
// this will affect the hardware
void timer_setvalue( TiTimerAdapter * timer, uint32 value )
{	
    NULL;

}

// get the current timestamp of the timer
// this value is read from the timer hardware
// complete by shanlei 06-11-14 just get the current TC value
uint32 timer_getvalue( TiTimerAdapter * timer )
{
	uint8 checkid;
	uint32 timervalue;
	
	checkid = timer->id & 0xF0;
	if(checkid == 0x00)
	{
	    timervalue = T0TC;
	}
	else if(checkid == 0x10)
	{
	    timervalue = T1TC;
	}            
	return timervalue;
}



// @attention: 
// ����interval��ֵ�ǳ��н�������Ӧ��̫��Ҳ��Ӧ��̫С
// the value of "interval" is very importatnt! it should NOT be too small
// to enable the application process the timer interrupt successfully. and it
// should NOT be too large so that no calculation overflow occured! 
//
// time_interval����ʱ������1Ϊ0.125�룻8Ϊ1��
//
// This is the old "timer_init()" function of Tang Wen
//
// @param 
//	timer		the timer object
//	interval	next interval 
//	repeat		decide whether the timer should work periodically
//				TRUE/1   period triggering
//				FALSE/0  trigger only once
//
void timer_setinterval( TiTimerAdapter * timer, uint32 interval, uint8 repeat )
{
	//uint8 checkid;
	
	switch (timer->id)
	{
	    case 0x00:                           /* ��ʱ��0ͨ��0��ʼ�� */	
	        T0TC   = 0;			             /* ��ʱ����ʼֵ����Ϊ0*/
	        T0PR   = 0;			             /* ʱ�Ӳ���Ƶ*/
	        T0MCR |= 0x002;		             /* ����T0MR0ƥ���λT0TC*/
	        T0MR0  =(Fpclk / 1000)* interval;	 /* 1ms��ʱ*/
	        break;
		case 0x01:                           /* ��ʱ��0ͨ��1��ʼ�� */	
	    	T0TC   = 0;			             /* ��ʱ����ʼֵ����Ϊ0*/
	    	T0PR   = 0;			             /* ʱ�Ӳ���Ƶ*/
	    	T0MCR |= 0x010;		             /* ����T0MR1ƥ���λT0TC*/
	    	T0MR1  =(Fpclk / 1000)* interval;	 /*1ms��ʱ*/
	    	break;
		case 0x02:                           /* ��ʱ��0ͨ��2��ʼ�� */	
	    	T0TC   = 0;			             /* ��ʱ����ʼֵ����Ϊ0*/
	    	T0PR   = 0;			             /* ʱ�Ӳ���Ƶ*/
	    	T0MCR |= 0x080;		             /* ����T0MR2ƥ���λT0TC*/
	    	T0MR2  =(Fpclk / 1000)* interval;	 /*1ms��ʱ*/
	    	break;
		case 0x03:                           /* ��ʱ��0ͨ��3��ʼ�� */	
	    	T0TC   = 0;			             /* ��ʱ����ʼֵ����Ϊ0*/
	    	T0PR   = 0;			             /* ʱ�Ӳ���Ƶ*/
	    	T0MCR |= 0x400;		             /* ����T0MR3ƥ���λT0TC*/
	    	T0MR3  =(Fpclk / 1000)* interval;	 /* 1ms��ʱʱ*/
	    	break;
		case 0x10:
	    	T1TC   = 0;			             /* ��ʱ����ʼֵ����Ϊ0*/
	    	T1PR   = 0;			             /* ʱ�Ӳ���Ƶ*/
	    	T1MCR |= 0x003;		             /* ����T1MR0ƥ���λT1TC*/
	    	T1MR0  =(Fpclk / 1000)* interval;	 /*1ms��ʱ*/
	    	break;
		case 0x11:
	    	T1TC   = 0;			             /* ��ʱ����ʼֵ����Ϊ0*/
	    	T1PR   = 0;			             /* ʱ�Ӳ���Ƶ*/
	    	T1MCR |= 0x010;		             /* ����T1MR1ƥ���λT1TC*/
	    	T1MR1  =(Fpclk / 1000)* interval;	 /*1ms��ʱ*/
	    	break;
		case 0x12:
	    	T1TC   = 0;			             /* ��ʱ����ʼֵ����Ϊ0*/
	    	T1PR   = 0;			             /* ʱ�Ӳ���Ƶ*/
	    	T1MCR |= 0x080;		             /* ����T1MR2ƥ���λT1TC*/
	    	T1MR2  =(Fpclk / 1000)* interval;	 /* 1ms��ʱ*/
	    	break;
		case 0x13:
	    	T1TC   = 0;			             /* ��ʱ����ʼֵ����Ϊ0*/
	    	T1PR   = 0;			             /* ʱ�Ӳ���Ƶ*/
	    	T1MCR |= 0x400;		             /* ����T1MR3ƥ���λT1TC*/
	    	T1MR3  =(Fpclk / 1000)* interval;    /* 1ms��ʱ*/
	    	break;
		default:
	      	break;
	}      	
	timer->interval = interval;

    if( repeat == 0)
        timer->state &= ~TIMER_STATE_PERIOD;  
    else
        timer->state |= TIMER_STATE_PERIOD;
        
    if( timer->priority > 15 )
        timer->state &= ~TIMER_STATE_INTERRUPT;
    else
        timer->state |= TIMER_STATE_INTERRUPT;        

	return;
}

/* set timer capture settings
 * 	- capture interrupt enable/disable
 * 	- capture condition 
 */
void timer_setcapture( TiTimerAdapter * timer, uint8 opt )
{
	// @TODO: time capture features
	NULL;
}

// @warning 
//	start timer 0 or timer 1. now this function only support two timer hardware.
//  modified by shanlei 06-11-14 
//  it will enable the VIC according to the timer->state
void timer_start( TiTimerAdapter * timer )
{
	uint8 checkid;
	checkid = timer->id & 0xF0;
	
	if(checkid == 0x00)
	{ 
	    T0TCR  = 0x01;
	}    	
	else if(checkid == 0x10)	 
	{
	    T1TCR  = 0x01;
	}    		
	
	timer_enable( timer );
	if (timer->state & TIMER_STATE_INTERRUPT)
	{
		timer_VICenable( timer );
	}
	timer->state |= TIMER_STATE_ENABLED;
}

//  modified by shanlei 06-11-14
//  it will disable the VIC according to the timer->state
void timer_stop( TiTimerAdapter * timer )
{
	uint8 checkid;
	checkid = timer->id & 0xF0;
	if(checkid == 0x00)
	{
		T0TCR = 0x00;
	}	
	else if(checkid == 0x10)
	{
	    T1TCR = 0x00;
	}
	
	timer_disable( timer );	 
	if (timer->state & TIMER_STATE_INTERRUPT)
	{
	    timer_VICdisable( timer );
	}
	timer->state &= (~TIMER_STATE_ENABLED);
}

// restart the timer again. all the old settings are used, for example, the 
// interval and interrupt settings.
void timer_restart( TiTimerAdapter * timer, uint32 interval, uint8 repeat )
{
	timer_stop( timer );
	// timer_setinterval( timer, timer->interval, timer->state & TIMER_STATE_INTERRUPT );
	timer_setinterval( timer, interval, repeat );
	timer_start( timer );
}

/* to judge whether the timer object is expired or not. if it is expired, 
 * then you should reset the expired flag. often the hardware itself will reset
 * the "expired" flag in the register when you read it out.
 * 
 * @attention
 * 	after you check the expired flag of timer, the flag will be automatically
 * cleared. 
 */
 // complete by shanlei 06-11-15
 // �����ʹ���жϣ�����T0IR(TIMER0)��T1IR(timer1)�Ƿ�Ϊ1
boolean timer_expired( TiTimerAdapter * timer )
{
	boolean expired;
	uint8 checkperiod;
	
	expired = 0;
	checkperiod = (timer->state & TIMER_STATE_PERIOD);
	
	switch (timer->id)
	{
	// timer 0 channel 0
	case 0x00:
	    if (T0IR & 0x01) 
	    {
	        expired = 1;
	        T0IR = 0x01;
	        if( checkperiod == 0)
	        {
	            T0MCR &= ~0x007;
	           
	        }    
	    }
	    break;
	// timer 0 channel 1
	case 0x01:
	    if (T0IR & 0x02) 
	    {
	        expired = 1;
	        T0IR = 0x02;
	        if( checkperiod == 0)
	            T0MCR &= ~0x038;
	    }    
	    break;
	case 0x02:
	    if (T0IR & 0x04)
	    {
	        expired = 1;
	        T0IR = 0x04;
	        if( checkperiod == 0)
	            T0MCR &= ~0x1C0;
	    }    
	    break;
	case 0x03:
	    if (T0IR & 0x08)
	    {
	        expired = 1;
	        T0IR = 0x08;
	        if( checkperiod == 0)
	            T0MCR &= ~0xE00;
	    }    
	    break;
	case 0x10:
	    if (T1IR & 0x01) 
	    {
	        expired = 1;
	        T1IR = 0x01;
	        if( checkperiod == 0)
	            T1MCR &= ~0x003;
	    }    
	    break;
	case 0x11:
	    if (T1IR & 0x02) 
	    {
	        expired = 1;
	        T1IR = 0x02;
	        if( checkperiod == 0)
	            T1MCR &= ~0x038;
	    }    
	    break;
	case 0x12:
	    if (T1IR & 0x04) 
	    {
	        expired = 1;
	        T1IR = 0x04;
	        if( checkperiod == 0)
	            T1MCR &= ~0x1C0;
	    }    
	    break;
	case 0x13:
	    if (T1IR & 0x08) 
	    {
	        expired = 1;
	        T1IR = 0x08;
	        if( checkperiod == 0)
	            T1MCR &= ~0xE00;
	    }    
	    break;
	default:    
	    break;
	}    
	
	return expired;
}

/* @return
 * 	return's the time elapsed between two this function calls.
 * 	the returned value maybe incorrect if the time duration is too long when it
 * is exceed the maximum limit of an uint32 integer.
 */
uint32 timer_elapsed( TiTimerAdapter * timer )
{
	uint32 curtime, ret;
	
	curtime = timer_getvalue( timer );
	if (curtime > timer->lasttime)
		ret = curtime - timer->lasttime;
	else
		ret = ((uint32)(~0x00)) - timer->lasttime + curtime;
	
	timer->lasttime = curtime;
	return ret;
}


/* 
 * �ò�����Ӧ��Ӱ��Timer�����С������ж��Ƿ�����Timer��Ӧ�����ߵ�
 * @TODO 2006-09
 * install�жϺ�uninstall�ж�Ӧ�÷���configure�����ȽϺ�
 * enable��disableֻ�ǿ���ʹ�����source codeӦ�ø�һ��
 */
 
//add by shanlei 
void timer_VICinit( TiTimerAdapter * timer )
{
	uint8 checkid;
	VICIntSelect = 0x00;				/* �����ж�ͨ������ΪIRQ�ж�			*/
	
	checkid = timer->id & 0xF0;
	if(checkid == 0x00)      /* ���ö�ʱ��0�ж�ͨ���������ȼ�	*/
	{
		
		switch (timer->priority)	   
		{       
		    case 0:	 
		        VICVectCntl0 = 0x20 | 0x04; 
		    //	VICVectAddr0 = (uint32)timer_interrupt;
		    	VICVectAddr0 = (uint32)Timer0_Int;
		    		
		   	 	break;		/* ���ȼ����*/
			case 1:  
		    	VICVectCntl1 = 0x20 | 0x04; 
		   // 	VICVectAddr1 = (uint32)timer_interrupt;
		    	VICVectAddr1 = (uint32)Timer0_Int;
		    	break;	
			case 2:  
		    	VICVectCntl2 = 0x20 | 0x04; 
		   // 	VICVectAddr2 = (uint32)timer_interrupt;
		    	VICVectAddr2 = (uint32)Timer0_Int;
		    	break;		
			case 3:	 
		    	VICVectCntl3 = 0x20 | 0x04; 
		   // 	VICVectAddr3 = (uint32)timer_interrupt;
		    	VICVectAddr3 = (uint32)Timer0_Int;
		    	break;	
  			case 4:  
  		    	VICVectCntl4 = 0x20 | 0x04; 
  		   // 	VICVectAddr4 = (uint32)timer_interrupt;
  		    	VICVectAddr4 = (uint32)Timer0_Int;
  		    	break;		
			case 5:	 
		    	VICVectCntl5 = 0x20 | 0x04; 
		   // 	VICVectAddr5 = (uint32)timer_interrupt;
		    	VICVectAddr5 = (uint32)Timer0_Int;
		    	break;	
			case 6:  
		    	VICVectCntl6 = 0x20 | 0x04; 
		   // 	VICVectAddr6 = (uint32)timer_interrupt;
		    	VICVectAddr6 = (uint32)Timer0_Int;
		    	break;		
  			case 7:  
  		    	VICVectCntl7 = 0x20 | 0x04; 
  		   // 	VICVectAddr7 = (uint32)timer_interrupt;
  		    	VICVectAddr7 = (uint32)Timer0_Int;
  		    	break;	
			case 8:	 
		    	VICVectCntl8 = 0x20 | 0x04; 
		   // 	VICVectAddr8 = (uint32)timer_interrupt;
		    	VICVectAddr8 = (uint32)Timer0_Int;
		    	break;		
			case 9:	 
		    	VICVectCntl9 = 0x20 | 0x04; 
		   // 	VICVectAddr9 = (uint32)timer_interrupt;
		    	VICVectAddr9 = (uint32)Timer0_Int;
		    	break;	
			case 10:  
		    	VICVectCntl10 = 0x20 | 0x04; 
		   // 	VICVectAddr10 = (uint32)timer_interrupt;
		    	VICVectAddr10 = (uint32)Timer0_Int;
		    	break;	
  			case 11:  
  		    	VICVectCntl11 = 0x20 | 0x04; 
  		   // 	VICVectAddr11 = (uint32)timer_interrupt;
  		    	VICVectAddr11 = (uint32)Timer0_Int;
  		    	break;	
			case 12:  
		    	VICVectCntl12 = 0x20 | 0x04; 
		   // 	VICVectAddr12 = (uint32)timer_interrupt;
		    	VICVectAddr12 = (uint32)Timer0_Int;
		    	break; 		
			case 13:  
		    	VICVectCntl13 = 0x20 | 0x04; 
		   // 	VICVectAddr13 = (uint32)timer_interrupt;
		    	VICVectAddr13 = (uint32)Timer0_Int;
		    	break;	
			case 14:  
		    	VICVectCntl14 = 0x20 | 0x04; 
		   // 	VICVectAddr14 = (uint32)timer_interrupt;
		    	VICVectAddr14 = (uint32)Timer0_Int;
		    	break;
  			case 15:  
  		    	VICVectCntl15 = 0x20 | 0x04; 
  		   // 	VICVectAddr15 = (uint32)timer_interrupt;
  		    	VICVectAddr15 = (uint32)Timer0_Int;
  		    	break;	/* ���ȼ����*/
  			default:  
  		    	break;    
  		}    
	}
  	else if(checkid == 0x10)
  	{	 /* ���ö�ʱ��0�ж�ͨ���������ȼ�	*/
  		
  		switch (timer->priority)	
  		{ 
  			case 0:  	
				VICVectCntl0 = 0x20 | 0x05;		/* ���ȼ����*/
			//	VICVectAddr0 = (uint32)timer_interrupt;
				VICVectAddr0 = (uint32)Timer1_Int;
				break;
			case 1:  	
			    VICVectCntl1 = 0x20 | 0x05;
			//  VICVectAddr1 = (uint32)timer_interrupt;
			    VICVectAddr1 = (uint32)Timer1_Int;
			    break;	
  			case 2:  	
  			    VICVectCntl2 = 0x20 | 0x05;
  			//  VICVectAddr2 = (uint32)timer_interrupt;
  			    VICVectAddr2 = (uint32)Timer1_Int;
  			    break;		
			case 3:	  
			    VICVectCntl3 = 0x20 | 0x05;
			//  VICVectAddr3 = (uint32)timer_interrupt;
			    VICVectAddr3 = (uint32)Timer1_Int;
			    break;	
  			case 4:   
  			    VICVectCntl4 = 0x20 | 0x05;
  			//  VICVectAddr4 = (uint32)timer_interrupt;
  			    VICVectAddr4 = (uint32)Timer1_Int;
  			    break;		
			case 5:	  
				VICVectCntl5 = 0x20 | 0x05;
			//  VICVectAddr5 = (uint32)timer_interrupt;
				VICVectAddr5 = (uint32)Timer1_Int;	
				break;
			case 6:  	
			    VICVectCntl6 = 0x20 | 0x05;		
			//  VICVectAddr6 = (uint32)timer_interrupt;
			    VICVectAddr6 = (uint32)Timer1_Int;
			    break;
  			case 7:  	
  				VICVectCntl7 = 0x20 | 0x05;	
  			//	VICVectAddr7 = (uint32)timer_interrupt;
  				VICVectAddr7 = (uint32)Timer1_Int;
  				break;
			case 8:	  
				VICVectCntl8 = 0x20 | 0x05;		
			//	VICVectAddr8 = (uint32)timer_interrupt;
				VICVectAddr8 = (uint32)Timer1_Int;
				break;
			case 9:	  
				VICVectCntl9 = 0x20 | 0x05;	
			//	VICVectAddr9 = (uint32)timer_interrupt;
				VICVectAddr9 = (uint32)Timer1_Int;
				break;
			case 10:	
				VICVectCntl10 = 0x20 | 0x05;		
			//	VICVectAddr10 = (uint32)timer_interrupt;
				VICVectAddr10 = (uint32)Timer1_Int;
				break;
  			case 11:	
  				VICVectCntl11 = 0x20 | 0x05;	
  			//	VICVectAddr11 = (uint32)timer_interrupt;
  				VICVectAddr11 = (uint32)Timer1_Int;
  				break;
			case 12:	
				VICVectCntl12 = 0x20 | 0x05;		
			//	VICVectAddr12 = (uint32)timer_interrupt;
				VICVectAddr12 = (uint32)Timer1_Int;
				break;
			case 13:	
				VICVectCntl13= 0x20 | 0x05;	
			//	VICVectAddr13 = (uint32)timer_interrupt;
				VICVectAddr13 = (uint32)Timer1_Int;
				break;
			case 14:  
				VICVectCntl14= 0x20 | 0x05;
			//	VICVectAddr14 = (uint32)timer_interrupt;
				VICVectAddr14 = (uint32)Timer1_Int;
				break;
  			case 15:	
  				VICVectCntl15 = 0x20 | 0x05;		/* ���ȼ����*/
            //  VICVectAddr15 = (uint32)timer_interrupt;
                VICVectAddr15 = (uint32)Timer1_Int;
  				break;
  			default:	
  				break;
  		}		
	}	
}

//add by shanlei
void timer_VICenable( TiTimerAdapter * timer)
{
    uint8 checkid;
    checkid = timer->id & 0xF0;
    if( checkid == 0x00)
    {
        VICIntEnable = 1 << 4;			/* ʹ�ܶ�ʱ��0�ж�*/
    }
    else if(checkid == 0x10)
    {
        VICIntEnable = 1 << 5;		    /* ʹ�ܶ�ʱ��1�ж�*/    
    }
     timer->state |= TIMER_STATE_INTERRUPT; 
 }    

//add by shanlei    
void timer_VICdisable( TiTimerAdapter * timer)
{
    uint8 checkid;
    checkid = timer->id & 0xF0;
    if( checkid == 0x00)
    {
        VICIntEnClr = 1 << 4;			/* ��ֹ��ʱ��0�ж�*/
    }
    else if(checkid == 0x10)
    {
        VICIntEnClr = 1 << 5;		    /* ��ֹ��ʱ��1�ж�*/    
    }
    timer->state &= (~TIMER_STATE_INTERRUPT);
}    
    

//modified by shanlei 06-11-16 
//now the timer_enable function just enable the comparason interrupt of each channel
void   timer_enable( TiTimerAdapter * timer )
{
	switch (timer->id)
	{
	    case 0x00:   /* ��ʱ��0ͨ��0��ʼ�� */	
	        T0MCR |= 0x001;		/* ����T0MR0ƥ���λT0TC����ʹ���ж�*/
	        break;
		case 0x01:   /* ��ʱ��0ͨ��1��ʼ�� */	
	    	T0MCR |= 0x008;		/* ����T0MR1ƥ���λT0TC����ʹ���ж�*/
	    	break;
		case 0x02:   /* ��ʱ��0ͨ��2��ʼ�� */	
	    	T0MCR |= 0x040;		/* ����T0MR2ƥ���λT0TC����ʹ���ж�*/
	    	break;
		case 0x03:   /* ��ʱ��0ͨ��3��ʼ�� */	
	    	T0MCR |= 0x200;		/* ����T0MR3ƥ���λT0TC����ʹ���ж�*/
	    	break;
		case 0x10:
	    	T1MCR |= 0x001;		/* ����T1MR0ƥ���λT1TC����ʹ���ж�*/
	    	break;
		case 0x11:
	    	T1MCR |= 0x008;		/* ����T1MR1ƥ���λT1TC����ʹ���ж�*/
	    	break;
		case 0x12:
	    	T1MCR |= 0x040;		/* ����T1MR2ƥ���λT1TC����ʹ���ж�*/
	    	break;
		case 0x13:
	    	T1MCR |= 0x200;		/* ����T1MR3ƥ���λT1TC����ʹ���ж�*/
	    	break;
		default:
	      	break;
	}      	
}		

//modified by shanlei 06-11-16 
//now the timer_disable function just disable the comparason interrupt of each channel
void timer_disable( TiTimerAdapter * timer )
{	
	switch (timer->id)
	{
	    case 0x00:   /* ��ʱ��0ͨ��0��ʼ�� */	
	        T0MCR &= ~0x001;		/* ����T0MR0ƥ���λT0TC������ֹ�ж�*/
	        break;
		case 0x01:   /* ��ʱ��0ͨ��1��ʼ�� */	
	    	T0MCR &= ~0x008;		/* ����T0MR1ƥ���λT0TC������ֹ�ж�*/
	    	break;
		case 0x02:   /* ��ʱ��0ͨ��2��ʼ�� */	
	    	T0MCR &= ~0x040;		/* ����T0MR2ƥ���λT0TC������ֹ�ж�*/
	    	break;
		case 0x03:   /* ��ʱ��0ͨ��3��ʼ�� */	
	    	T0MCR &= ~0x200;		/* ����T0MR3ƥ���λT0TC������ֹ�ж�*/
	    	break;
		case 0x10:
	    	T1MCR &= ~0x001;		/* ����T1MR0ƥ���λT1TC������ֹ�ж�*/
	    	break;
		case 0x11:
	    	T1MCR &= ~0x008;		/* ����T1MR1ƥ���λT1TC������ֹ�ж�*/
	    	break;
		case 0x12:
	    	T1MCR &= ~0x040;		/* ����T1MR2ƥ���λT1TC������ֹ�ж�*/
	    	break;
		case 0x13:
	    	T1MCR &= ~0x200;		/* ����T1MR3ƥ���λT1TC������ֹ�ж�*/
	    	break;
		default:
	      	break;
	}      	
}

uint32 timer_clocksperms( TiTimerAdapter * timer )
{
	return TIMER_CLOCKS_PER_MILLISECOND;
}

// this is the interrupt service routine
// it will automatically called when the timer expired if the timer enabled
// interrupt.
// 

void __irq timer_interrupt( void )
{
	
	TiTimerAdapter * timer = NULL;
	uint8 VICcheck;
	uint8 checkperiod;
	
	// @TODO
	// check registers to see which timer hardware raised the interrupt
	// and call the active timer's callback function here
	// has been modified by shanlei 
	

	VICcheck = (1<<4) | (1<<5);
	VICcheck &= VICRawIntr;
	
	
	if (VICcheck == (1<<4)) // timer 0
	{
		//led_toggle( LED_GREEN );
		timer = g_timer0;
	}
	else if(VICcheck == (1<<5)) // timer 1
	{ 
		timer = g_timer1;	
	}
	
	
	//shanlei: clear the interrupt flag here, suppose the timer interrupt is
	//aroused by comparason. if other events like capture cause the interrupt,
	//you should clear the IF by yourself.
	checkperiod = timer->state;
	checkperiod &= TIMER_STATE_PERIOD;
	switch (timer->id)
	{
	// timer 0 channel 0
	case 0x00:
	    if (T0IR & 0x01) 
	    {
	        T0IR = 0x01;
	        if( checkperiod == 0)
	            T0MCR &= ~0x007;
	    }
	    break;
	// timer 0 channel 1
	case 0x01:
	    if (T0IR & 0x02) 
	    {
	        T0IR = 0x02;
	        if( checkperiod == 0)
	            T0MCR &= ~0x038;
	    }    
	    break;
	case 0x02:
	    if (T0IR & 0x04)
	    {
	        T0IR = 0x04;
	        if( checkperiod == 0)
	            T0MCR &= ~0x1C0;
	    }    
	    break;
	case 0x03:
	    if (T0IR & 0x08)
	    {
	        T0IR = 0x08;
	        if( checkperiod == 0)
	            T0MCR &= ~0xE00;
	    }    
	    break;
	case 0x10:
	    if (T1IR & 0x01) 
	    {
	        T1IR = 0x01;
	        if( checkperiod == 0)
	            T1MCR &= ~0x007;
	    }    
	    break;
	case 0x11:
	    if (T1IR & 0x02) 
	    {
	        T1IR = 0x02;
	        if( checkperiod == 0)
	            T1MCR &= ~0x038;
	    }    
	    break;
	case 0x12:
	    if (T1IR & 0x04) 
	    {
	        T1IR = 0x04;
	        if( checkperiod == 0)
	            T1MCR &= ~0x1C0;
	    }    
	    break;
	case 0x13:
	    if (T1IR & 0x08) 
	    {
	        T1IR = 0x08;
	        if( checkperiod == 0)
	            T1MCR &= ~0xE00;
	    }    
	    break;
	default:    
	    break;
	}    
	
	if (timer != NULL)
	{
		timer->callback( timer, timer->data );
	}
	
	VICVectAddr = 0x00;			// ֪ͨVIC�жϴ������
}

void __irq Timer0_Int (void)
{
    TiTimerAdapter * timer = g_timer0;

	uint8 checkperiod;
	


	//shanlei: clear the interrupt flag here, suppose the timer interrupt is
	//aroused by comparason. if other events like capture cause the interrupt,
	//you should clear the IF by yourself.
	checkperiod = timer->state;
	checkperiod &= TIMER_STATE_PERIOD;
	switch (timer->id)
	{
	// timer 0 channel 0
	case 0x00:
	    if (T0IR & 0x01) 
	    {
	        T0IR = 0x01;
	        if( checkperiod == 0)
	            T0MCR &= ~0x007;
	    }
	    break;
	// timer 0 channel 1
	case 0x01:
	    if (T0IR & 0x02) 
	    {
	        T0IR = 0x02;
	        if( checkperiod == 0)
	            T0MCR &= ~0x038;
	    }    
	    break;
	case 0x02:
	    if (T0IR & 0x04)
	    {
	        T0IR = 0x04;
	        if( checkperiod == 0)
	            T0MCR &= ~0x1C0;
	    }    
	    break;
	case 0x03:
	    if (T0IR & 0x08)
	    {
	        T0IR = 0x08;
	        if( checkperiod == 0)
	            T0MCR &= ~0xE00;
	    }    
	    break;
	default:    
	    break;
	}    
	
	if (timer != NULL)
	{
		timer->callback( timer, timer->data );
	}
	
    
    VICVectAddr = 0x00;			// ֪ͨVIC�жϴ������	
} 


void __irq Timer1_Int (void)
{
    TiTimerAdapter * timer = g_timer1;

	uint8 checkperiod;
	
	
	//shanlei: clear the interrupt flag here, suppose the timer interrupt is
	//aroused by comparason. if other events like capture cause the interrupt,
	//you should clear the IF by yourself.
	checkperiod = timer->state;
	checkperiod &= TIMER_STATE_PERIOD;
	switch (timer->id)
	{
	case 0x10:
	    if (T1IR & 0x01) 
	    {
	        T1IR = 0x01;
	        if( checkperiod == 0)
	            T1MCR &= ~0x007;
	    }    
	    break;
	case 0x11:
	    if (T1IR & 0x02) 
	    {
	        T1IR = 0x02;
	        if( checkperiod == 0)
	            T1MCR &= ~0x038;
	    }    
	    break;
	case 0x12:
	    if (T1IR & 0x04) 
	    {
	        T1IR = 0x04;
	        if( checkperiod == 0)
	            T1MCR &= ~0x1C0;
	    }    
	    break;
	case 0x13:
	    if (T1IR & 0x08) 
	    {
	        T1IR = 0x08;
	        if( checkperiod == 0)
	            T1MCR &= ~0xE00;
	    }    
	    break;
	default:    
	    break;
	}    
	
	if (timer != NULL)
	{
		timer->callback( timer, timer->data );
	}
	
    VICVectAddr = 0x00;			// ֪ͨVIC�жϴ������	    
} 

  	
/**********************************************************************************************************************/

// ���´���ƻ�����ͨ��

void LPC_TIMER0_INIT()
{
    T0PR = 0;  // Ԥ��Ƶ�� �д�����
    T0CCR |= BM(CAP2RE) | BM(CAP2I); //cap0.2 �����ز��񣬲������ж�       	
    T0MCR |= BM(MR0I) | BM(MR0R) | BM(MR1I); //BM(MR2I)| BM(MR2R);  //??
    T0TC = 0;
    VICIntSelect = 0x00000000;			// �������е�ͨ��ΪIRQ�ж�
    VICVectCntl0 = 0x20 | 4;			// Timer0���䵽IRQ slot0����������ȼ�
    VICVectAddr0 = (uint32)Timer0_Int;	// ����Timer1������ַ 
}

void INTERRUPT_LPC_TIMER0_CAPT()
{
	if (g_timer0 != NULL)
	{
		if (g_timer0->callback != NULL)
			g_timer0->callback( g_timer0, g_timer0->data );
	}
}

void INTERRUPT_LPC_TIMER0_T0MR0()
{
	if (g_timer0 != NULL)
	{
		if (g_timer1->callback != NULL)
			g_timer1->callback( g_timer1, g_timer1->data );
	}
}

void INTERRUPT_LPC_TIMER0_T0MR2()
{
	if (g_timer2 != NULL)
	{
		if (g_timer2->callback != NULL)
			g_timer2->callback( g_timer2, g_timer2->data );
	}
}


void ENABLE_LPC_TIMER0_INT(void)
{
    VICIntEnable = 1 << 4;			// ʹ��Timer0�ж�
}

void DISABLE_LPC_TIMER0_INT(void)
{
    VICIntEnClr = 1 << 4;
}    

void DISABLE_LPC_TIMER0_T0MR0_INT(void)
{
    T0MCR &= ~BM(MR0I);  
}

void ENABLE_LPC_TIMER0_T0MR0_INT(void)
{
    T0MCR |= BM(MR0I);  
}

void DISABLE_LPC_TIMER0_T0MR2_INT(void)
{
    T0MCR &= ~BM(MR2I);  
}

void ENABLE_LPC_TIMER0_T0MR2_INT(void)
{
    T0MCR |= BM(MR2I);  
}

void DISABLE_LPC_TIMER0_T0CAP2_INT(void)
{
    T0CCR &= ~BM(CAP2I);  
}

void ENABLE_LPC_TIMER0_T0CAP2_INT(void)
{
    T0CCR |= BM(CAP2I);  
}

void LPC_TIMER0_SET_T0MR0_VALUE(DWORD value)
{
    T0MR0 = value;
}

void LPC_TIMER0_SET_T0MR1_VALUE(DWORD value)
{
    T0MR1 = value;
}

void LPC_TIMER0_SET_T0MR2_VALUE(DWORD value)
{
    T0MR2 = value;
}

void LPC_TIMER0_CLEAR_T0MR0()
{
    T0IR  = BM(MR0IF);	
}

void LPC_TIMER0_CLEAR_T0MR1()
{
    T0IR  = BM(MR1IF);	
}

void LPC_TIMER0_CLEAR_T0MR2()
{
    T0IR  = BM(MR2IF);	
}