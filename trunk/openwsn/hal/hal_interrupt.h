#ifndef _HAL_INTERRUPT_H_
#define _HAL_INTERRUPT_H_

#include "hal_foundation.h"

/*
enable the global interrupt control flag in CPU's flag register
void hal_enable();
void hal_disable();

enable global interrupt control bit flag and also the VIC
void hal_enable_with_vic();
void hal_disable_with_vic();
void hal_clear_with_vic();

// opt = 0x00 default
//   [b7,..,b0]
//  b0 = 0 general interrupt, 1 FIQ interrupt (ARM only)
//	b1 = 0 voltage trigger, 1 pulse trigger
//   
void hal_interrupt_install( uint8 num, TInterruptHandler isr, uint8 prior, uint8 opt );
void hal_interrupt_uninstall( uint8 num );
void hal_interrupt_trigger( uint8 num );
void hal_interrupt_enable( uint8 num );
void hal_interrupt_disable( uint8 num );
void hal_interrupt_state( uint8 num );  // has request? in servicing? masked?
void hal_interrupt_clearrequest( uint8 num );


how about the following names? is it more clearly to reflect the meaning?

hal_irq_enable
hal_irq_disable
hal_fiq_enable
hal_fiq_disable

hal_vic_install
hal_vic_uninstall
hal_vic_enable
hal_vic_disable
hal_vic_clearrequest
*/

#endif
