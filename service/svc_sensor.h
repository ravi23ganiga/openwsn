#ifndef _SENSOR_H_6278_
#define _SENSOR_H_6278_

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
 ***************************************************************************** 
 * @description sensing service
 * this is the higher level encapsulation of all kinds of low level sensors.
 * different to hal_sensor_xxx, this service include timer-driven sampling. 
 * while, the hal_sensor_xxx only encapsulate the basic read/write operations 
 * for a sensor.
 * 
 ****************************************************************************/ 		

#include "hal_foundation.h" 
#include "hal_sensor_temp.h"
#include "hal_sensor_vibration.h"
#include "hal_sensor_strain.h"
#include "hal_timer.h"
#include "svc_openpacket.h"

#define SEN_TYPE_STRAIN ODA_TYPE_STRAINSENSOR
#define SEN_TYPE_VIBRATION ODA_TYPE_VIBSENSOR
#define SEN_TYPE_TEMPRATURE ODA_TYPE_TEMPSENSOR
#define SEN_TYPE_LIGHT ODA_TYPE_LIGHTSENSOR

typedef struct{
  uint8 opt;
  TTempSensor * temp;
  TVibrationSensor * vib;
  TStrainSensor * strain;
  TTimer * timer;
}TSensorService;

TSensorService * sen_construct( char * buf, uint16 size );
void sen_destroy( TSensorService * sen );
void sen_configure( TSensorService * sen, TTempSensor * temp, TVibrationSensor * vib,
	TStrainSensor * strain, TTimer * timer );
int8 sen_read( TSensorService * sen, char * buf, uint8 size, uint8 opt );
int8 sen_fillpacket( TSensorService * sen, uint8 type, TOpenPacket * pkt, uint8 size );  
int8 sen_fillframe( TSensorService * sen, uint8 type, TOpenFrame * frame, uint8 size );  
int8 sen_start( TSensorService * sen, uint32 interval, uint32 delay );
int8 sen_stop( TSensorService * sen );

#endif
