#include "hal_interrupt.h"

/*****************************************************************************
 * @modified by zhangwei on 20070627
 * revision the interrupt related source code. collected them into this module. 
 ****************************************************************************/

void hal_enable_interrupts()
{
#if (CONFIG_OS == OPENWSN_OS_UCOSII)
	IRQEnable();
#else
	hal_irq_enable();
#endif
}

void hal_disable_interrupts()
{
#if (CONFIG_OS == OPENWSN_OS_UCOSII)
	IRQDisable();
#else
	hal_irq_disable();
#endif
}

   /* @modified by zhangwei on 20070425
 * enble irq and fiq of arm
 * 		MRS	r1, cpsr		
 * 		BIC r1, r1, #0xC0
 * 		MSR	cpsr_c, r1
 * 
 * disable irq and fiq of arm
 * 		MRS	r1, cpsr
 * 		ORR	r1, r1, #0xc0
 * 		MSR	cpsr_c, r1
 */

  /* TODO hal_enter_critical() and hal_leave_critical() are essentially wrong!
   */

void hal_enter_critical( void )
{
#if (CONFIG_OS == OPENWSN_OS_UCOSII)
   // @TODO 20061025
   // zhangwei comment the following to make it compile pass
  //IRQ_FLAG_STORE = IRQ_FLAG;
	IRQDisable();
#else
	hal_irq_disable();
	hal_fiq_disable();
#endif
}

void hal_leave_critical( void )
{
#if (CONFIG_OS == OPENWSN_OS_UCOSII)
  //if(IRQ_FLAG_STORE) IRQDisable();	
  //else               IRQEnable();		
	IRQEnable();
#else
	hal_irq_enable();
	hal_fiq_enable();
#endif
}

