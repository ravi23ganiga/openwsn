/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 
 
#ifndef _HAL_INTERRUPT_H_4788_
#define _HAL_INTERRUPT_H_4788_

#include "hal_foundation.h"
#include "..\src\target.h"

#ifdef CONFIG_OS_UCOSII
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
