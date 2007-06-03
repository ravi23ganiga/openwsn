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
 *****************************************************************************/ 

#ifndef _LEDSYNC_H_4287_
#define _LEDSYNC_H_4287_

/*****************************************************************************
 * @author zhangwei on 20070421
 * @note
 * 	blink test and demostration
 * 	this test will synchronize the LED flash frequvency across serveral nodes.
 * so it can be used to test whether the wireless communication and main processing
 * service is working right or not. 
 *  different to "cc2420tx" and "cc2420rx", this demo will test both the sending
 * and receiving process.
 *
 *  it's similar to the TinyOS Blink Example.
 *
 * @status
 * 	un-tested yet
 * 
 * @history
 * @modified by xxx on 20070421
 *  
 *****************************************************************************/ 

#include "../configall.h"
#include "../foundation.h"

void blink_test();

#endif


 