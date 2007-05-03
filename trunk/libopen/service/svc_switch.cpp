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

#include "svc_foundation.h"
//----------------------------------------------------------------------------
// svc_switch
// connect different channels
//----------------------------------------------------------------------------

// the "switch" has at least the following ports: UART0, UART1, USB, SOCKET, FILE,
// SIMULATE
//
#define SWITCH_PORT_COUNT 6

typedef int8 (*TSwitchRead)( void * inst, char * buf, uint8 size, uint8 opt );

typedef struct{
  int master;
  int slavelist;
  //TSwitchRead
}TSwitchPort;

typedef struct{
 TSwitchPort ports[SWITCH_PORT_COUNT]; 
}TSwitch;
/*
int switch_evolve( TSwitch * swi )
{
	int i, j;

	if swi->state == BUSY
		return 0

	for (master=0; master<SWITCH_PORT_COUNT; master++)
	{
		swi->datalen = read( i, swi->buf );
		if (swi->datalen > 0)
		{
			for (slave=0; slave<SWITCH_PORT_COUNT; slave++)
			{
				if (slave == master)
					continue;
				if (all write() success)
				{
					swi->datalen = 0;
				}
			}
		}
	}
}
				  

switch_create
switch_free
switch_configure
switch_attach
switch_evolve
switch_wait // wait for any frame arrived. similar to semaphore
*/
