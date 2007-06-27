#ifndef _HAL_INTERRUPT_H_4788_
#define _HAL_INTERRUPT_H_4788_

#include "hal_foundation.h"
#include "..\src\target.h"

#if (CONFIG_OS == OPENWSN_OS_UCOSII)
#include "..\arm\os_cpu.h"
#endif

/* enable/disable the global interrupt control flag in CPU's flag register
 * for ARM CPU. it support fast interrupt FIQ. in openwsn, we don't use it. so
 * in most cases, FIQ are diabled.
 */
#define hal_irq_enable() SwiHandle1(1)
#define hal_irq_disable() SwiHandle1(0)
#define hal_fiq_disable() SwiHandle1(2)
#define hal_fiq_enable() SwiHandle1(3)

/* @author zhangwei on 2006-07-20
 * @attention
 * you SHOULD always perfer to use enter_critical() instead of enable_interrupts()
 * and better leave_critical() than disable_interrupts(). the reason is that xxx_critical()
 * will leave the global interrupt flag unchanged, while the disable() enable() pair will
 * leave the flag enabled!!! this is sometimes unwanted!
 */
void hal_enable_interrupts( void );
void hal_disable_interrupts( void );
void hal_enter_critical( void );
void hal_leave_critical( void );

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
void hal_vic_init()
void hal_vic_attach( uint8 num, TInterruptHandler isr, uint8 prior, uint8 opt );
void hal_vic_detach uint8 num );
void hal_vic_trigger( uint8 num );
void hal_vic_enable( uint8 num );
void hal_vic_disable( uint8 num );
uint8 hal_vic_state( uint8 num );  // has request? in servicing? masked?
void hal_vic_clearrequest( uint8 num );
*/

#endif
