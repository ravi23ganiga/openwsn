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

#ifndef _OS_INTERRUPT_H_3248_
#define _OS_INTERRUPT_H_3248_

/*****************************************************************************
 * provide an unified interrupt management interface for upper layers ("service" 
 * or "application" layer) 
 * 
 * since "openwsn" is assumed to be based on 16b/32b MCU such as MSP430 or ARM7, 
 * it's designed to supports multiple interrupts. for some simple MCU such as PIC16F, 
 * it only support one interrupt. but i think it doesn't matter. I mean the design 
 * should support multiple interrupts.
 *
 * for simplicity and efficiency, "openwsn" would support a simple interrupt 
 * priority mechanism rather than a complicated one. it support at most 4 interrupts 
 * by default. each has a unique priority number. the count can be configured 
 * by a macro at compiling time.
 ****************************************************************************/ 
 
 #include "os_foundation.h"

/* when there's a multiple interrupt managment module (such as VIC) in the system, 
 * this module can be simplified as a group of macros based on VIC interface. But
 * if there's no VIC such as NIOS MCU in FPGA, then this module support simulate
 * multiple interrupts and the whole system can continue running smoothly.
 *
 * since this module will keep a interrupt service handler table in the RAM, it
 * consume nearly 100 Bytes. you can choose to use VIC directly or give up this 
 * module if the RAM space is very valuable. 
 */

void os_attach( uint8 num, TOsInterruptHandler handler, void * owner, uint8 prior );
void os_detach( uint8 num );
void os_enable( uint8 num );
void os_disable( uint8 num ); 
void os_trigger( uint8 num );
void os_enter_critical(); 
void os_leave_critical(); 

#endif
