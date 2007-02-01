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
