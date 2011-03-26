/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
#ifndef _HAL_SHT_HUMIDITY_TEMP_H_
#define _HAL_SHT_HUMIDITY_TEMP_H_

/******************************************************************************
                    ��ʪ�ȴ�����ͷ�ļ� 
  MCU:m128   
  ���ƿڣ�Ϊ DATA-PC0;SCK-PC1�� 
  �û�����: temp_humi_test(float *tp,float *hi)������floatֵ            
          �÷�:���������ж���IO��:�� 
                    #define  IO_PF   
                 #define  IO_DATA  0                                  
                 #define  IO_SCK   1 
                                 
                                 ..... 
                                 ��ʾʹ��PF0��DATA��,PF1��SCK�� 
                                     ע��ͷ�ļ������˳�� 

 * portable
 *	Atmega 128 MCU (ICT GAINZ Platform)
 *  compiled by WinAVR or AVR Studio (based on WinAVR)
 *  ANSI C
 *
 * state
 *	finished. not tested yet
 *
 * @author Ф���� on 200905xx
 * @modified by zhangwei on 20090715
 *	- revision new interface
 * 
 *****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_uart.h"

// todo:
// ���������include
// C����ŵ�hal_shtxx.c��
#ifdef __cplusplus
extern "C"{
#endif

typedef struct{
	uint8 id;
}TiShtSensor;

TiShtSensor * sht_construct( void * mem, uint16 size );
void sht_destroy( TiShtSensor * sht );
TiShtSensor * sht_open( TiShtSensor * sht, uint8 id );
void sht_close( TiShtSensor * sht );

char sht_write_byte( TiShtSensor * sht, unsigned char value );
char sht_read_byte( TiShtSensor * sht, unsigned char ack );
void sht_reset( TiShtSensor * sht );
void sht_value( TiShtSensor * sht, float *tp, float *hi );

#ifdef __cplusplus
}
#endif

#endif
