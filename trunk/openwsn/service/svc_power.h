/******************************************************************************
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
 *****************************************************************************/ 

#ifndef _SVC_POWER_H_
#define _SVC_POWER_H_

/******************************************************************************
 * @author zhangwei on 20070610
 *
 * PowerManagement Service
 * this module provides a high level abstraction and interface for low management
 * 
 *****************************************************************************/ 

#define POWER_ACTIVE
#define POWER_SLEEP
#define POWER_DEEPSLEEP
#define POWER_POWERDOWN
#define POWER_SHUTDOWN

#define POWER_LEVEL0
#define POWER_LEVEL1
#define POWER_LEVEL2
#define POWER_LEVEL3
#define POWER_LEVEL4


typedef struct{
  
}TiPowerManagement;

void pm_construct( char * buf, uint16 size );
void pm_free( TiPowerManagement * pm );
void pm_setlevel();
void pm_evolve();

#endif
