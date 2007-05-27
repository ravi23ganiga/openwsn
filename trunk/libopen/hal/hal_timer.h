#ifndef _TIMER_H_6828_
#define _TIMER_H_6828_

/******************************************************************************
 * @author zhangwei on 2006-08-11
 * Timer 
 * This object is the software mapping of MCU's hardware timer object.
 * attention that TTimer is not the 1:1 mapping of hardware timer. actually, one 
 * TTimer object corrspond to 1 separate channel of hardware timer.
 * 
 * @history
 * @modified by zhangwei 2006-09-05
 * @modified by tangwen on 2006-10-21
 * 	add channel parameter to construct function.
 *  and other modifications to support the channel(sub-timer).
 * 
 *****************************************************************************/
 
#include "hal_foundation.h"


typedef struct{
  uint8 id;
}TTimer;

TTimer * timer_construct( TTimer * timer, uint8 size );
void timer_destroy( TTimer * timer );

#endif /* _TIMER_H_6828_ */

