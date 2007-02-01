#ifndef _VIBSENSOR_H_7689_
#define _VIBSENSOR_H_7689_

/*****************************************************************************
 * @note
 * this module is based on hal_mcp6s26 and hal_ad
 * it provide a high level support to vibration sensor
 ****************************************************************************/
#include "hal_foundation.h"
#include "hal_ad.h"
#include "hal_mcp6s.h"

#define VIBRATION_SLEEP_PIN  0
#define VIBRATION_SLEEP_PORT 0

#define SLEEP  0
#define WAKE   1

typedef struct{
  uint8 id;
  TMcp6s26 * mcp;
  TAdConversion *ad;
}TVibrationSensor;

TVibrationSensor * vib_construct( uint8 id, char * buf, uint8 size, TMcp6s26 * mcp, TAdConversion * ad  );
void vib_destroy( TVibrationSensor * vib );  
uint8 vib_read( TVibrationSensor * vib, char * buf, uint8 size, uint8 opt );

void vib_config(uint8 ifsleep);   //sleep or wake


#endif