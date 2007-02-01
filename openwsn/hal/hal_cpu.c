/*******************************************************************************
 * @author zhangwei on 20060901
 * 
 * @history
 * @modified by zhangwei on 20060906
 * update compiling macros of hal_delay()
 ******************************************************************************/

#include "hal_foundation.h"
#include "hal_cpu.h"

static uint8 IRQ_FLAG_STORE;

/*******************************************************************************
 * Runs an idle loop for [timeout] microseconds.
 * 		Note 1: The time spent processing interrupts will be added to the timeout!
 * 		Note 2: When porting to another MCU or compiler take care and count generated instructions!
 * 
 * @attention
 * 	pay attention to the input parameter 
 * 	the time really delayed rely on the MCU and frequency.
 * 
 * 	the author recommended NOT to use function! the Timer and ActionScheduler 
 * 	object is more suitable for precise delays. you can start a timer and check
 * 	when it expired to wait for precise periods.
 * 
 * @param
 * 	uint16 timeout
 *  The timeout in microseconds ??? QQQ
 * 
 ******************************************************************************/

#ifdef __GNUC__
void hal_delay( uint16 timeout ) 
{
  	/* usually the sequence uses exactly 8 clock cycle for each round */
  	do {
    	NOP();
    	NOP();
    	NOP();
    	NOP();
  	} while (--timeout);
}

#elif defined __IAR_SYSTEMS_ICC__
#pragma optimize=none
void hal_delay( uint16 timeout ) 
  	do {
    	NOP();
  	} while (--timeout);
}

/* for unknown compiles */
#else
void hal_delay( uint16 delay )
{
	uint32 count, n;
	
	count = delay * 10000;
	for (n=0; n<count; n++)
	{
		delay = 0;
		delay ++;
	}
}
#endif


// reboot the whole system
// TODO: hal_reboot
void hal_reboot()
{
	
}

// obsolete functions
void  halWait(uint32  dly)
{  uint32  i;

   for(; dly>0; dly--) 
      for(i=0; i<8000; i++);
}

void NO_OS_ENTER_CRITICAL(void)
{
   // @TODO 20061025
   // zhangwei comment the following to make it compile pass
  //IRQ_FLAG_STORE = IRQ_FLAG;
  IRQDisable();	
}


void NO_OS_EXIT_CRITICAL(void)
{
  if(IRQ_FLAG_STORE) IRQDisable();	
  else               IRQEnable();		
}