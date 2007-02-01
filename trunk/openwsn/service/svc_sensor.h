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
#ifndef _SENSOR_H_6278_
#define _SENSOR_H_6278_

/*****************************************************************************
 * @description sensing service
 * this is the higher level encapsulation of all kinds of low level sensors.
 * 
 ****************************************************************************/ 		
// @TODO: to be implemented 

#include "hal_foundation.h" 

#define SEN_TYPE_STRAIN 0x01
#define SEN_TYPE_VIBRATION 0x02
#define SEN_TYPE_TEMPRATURE 0x03
#define SEN_TYPE_LIGHT 0x04

typedef struct{
  uint8 opt;
}TSensorService;

TSensorService * sen_construct( TSensorService * sen );
void sen_destroy( TSensorService * sen );

/* read data from all kinds of sensors.
 * @param
 * 	opt		an option to decide which type sensor to use.
 */
int8 sen_read( TSensorService * sen, char * buf, uint8 size, uint8 opt );

#endif
