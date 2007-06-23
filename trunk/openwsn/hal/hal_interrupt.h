#ifndef _HAL_INTERRUPT_H_
#define _HAL_INTERRUPT_H_

#include "hal_foundation.h"
#include "..\src\target.h"

/* enable/disable the global interrupt control flag in CPU's flag register
 * for ARM CPU. it support fast interrupt FIQ. in openwsn, we don't use it. so
 * in most cases, FIQ are diabled.
 */
#define hal_irq_enable() SwiHandle1(1)
#define hal_irq_disable() SwiHandle1(0)
#define hal_fiq_disable() SwiHandle1(2)
#define hal_fiq_enable() SwiHandle1(3)

/*
enable global interrupt control bit flag and also the VIC
void hal_enable_with_vic();
void hal_disable_with_vic();
void hal_clear_with_vic();

// opt = 0x00 default
//   [b7,..,b0]
//  b0 = 0 general interrupt, 1 FIQ interrupt (ARM only)
//	b1 = 0 voltage trigger, 1 pulse trigger
//   
void hal_vic_install( uint8 num, TInterruptHandler isr, uint8 prior, uint8 opt );
void hal_vic_uninstall( uint8 num );
void hal_vic_trigger( uint8 num );
void hal_vic_enable( uint8 num );
void hal_vic_disable( uint8 num );
uint8 hal_vic_state( uint8 num );  // has request? in servicing? masked?
void hal_vic_clearrequest( uint8 num );

*/

#endif
