#ifndef _HAL_ADC_H_3467_
#define _HAL_ADC_H_3467_

/******************************************************************************
 * @author MaKun on 200612
 * @note
 * 	for AD transformation
 * 
 * @history
 * @modified by zhangwei on 20070101
 *	- revise source code from MaKun
 * 
 * @modified by zhangwei on 20090715
 *	- revision. new interface now.
 *
 * @modified by xiao-yuezhang(TongJi University) on 20090716
 *	- ported to atmega128 and GAINZ platform
 *  - tested by xu-xizhou(TongJi University)
 *****************************************************************************/
 
#include "hal_configall.h"
#include "hal_foundation.h"

#define CONFIG_ADC_INTERRUPT_DRIVEN
#undef  CONFIG_ADC_INTERRUPT_DRIVEN

#define CONFIG_ADC_BUFSIZE 32

/* TiAdcAdapter
 *  option[bit7..bit0]: 
 *					bit0 indicate whether this adc is driven by interrupt or not. 
 *	listener:		will be called when A/D conversion complete if interrupt enabled.
 *	lisowner:       used when call listener
 */
typedef struct{
  uint8 id;
  uint8 option;
  TiFunEventHandler listener;
  void * lisowner;
  volatile uint8 len;
  volatile char data[ CONFIG_ADC_BUFSIZE ];
}TiAdcAdapter;

TiAdcAdapter * adc_construct( char * buf, uint8 size );
void adc_destroy( TiAdcAdapter * adc );
TiAdcAdapter * adc_open( TiAdcAdapter * adc, uint8 id, TiFunEventHandler listener, void * lisowner, uint8 option );
void adc_close( TiAdcAdapter * adc );
void adc_start( TiAdcAdapter * adc );
void adc_stop( TiAdcAdapter * adc );
uint16 adc_read(TiAdcAdapter * adc, char * buf, uint8 size, uint8 opt );

#endif
