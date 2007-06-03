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
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#ifndef _OPENMAC_TEST_H_
#define _OPENMAC_TEST_H_

/*****************************************************************************
 * @author zhangwei on 200612
 *	this demo is used to test whether the MAC layer work correct. 
 *	this test require two nodes work on at the same time.
 *
 * process flow:
 *	- boot as SLAVE node
 *  - wait for MASTER node broadcast
 *  - if cannot find a MASTER node, then itself become a MASTER node
 *        and waiting for SLAVE node.     
 *        master node should broadcast messages to neighbor nodes.
 *
 *  after the above process, you should know who is the MASTER node
 *  and who is the SLAVE node. you can get some reference on the above 
 *  process in "blink" demo.
 *
 *  - if working in MASTER mode then
 *        send a random frame to SLAVE node
 *        waiting for SLAVE response
 *        check whether the data received are the same as data send
 *        if they two are the same, then GREEN led on else RED led on.
 *    endif
 *  - if working in SLAVE mode then
 *        try read frame 
 *        if frame read, then send it back to MASTER
 *    endif
 *
 ****************************************************************************/ 

#include "../configall.h"
#include "../foundation.h"

void openmac_run( void );

//----------------------------------------------------------------------------
#endif