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

#endif
