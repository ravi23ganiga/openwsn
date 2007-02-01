#ifndef _SPI_H_4289_
#define _SPI_H_4289_

/***************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TSPIDriver 
 *
 * @modified by zhangwei on 20061010
 * based on Huanghuan's mature code. 
 * support multiple SPI channel.
 * 
 ****************************************************************************/

#include "hal_foundation.h"
#include "hal_cpu.h"

typedef struct{
  uint8 id;
}TSpiDriver;

/* global variables defined in global.* */
extern TSpiDriver * g_spi0;
extern TSpiDriver * g_spi1;

TSpiDriver * spi_construct( uint8 id, char * buf, uint8 size );
void  spi_destroy( TSpiDriver * spi );
void  spi_configure( TSpiDriver * spi );

uint8 spi_read( TSpiDriver * spi, char * buf, uint8 capacity, uint8 opt );
uint8 spi_write( TSpiDriver * spi, char * buf, uint8 len, uint8 opt );
void  spi_put( TSpiDriver * spi, char ch );
int8  spi_get( TSpiDriver * spi, char * pc );
void  spi_wait( TSpiDriver * spi );

/* @attention
 * obsolete functions. pls use the above function instead
 */
void FASTSPI_TX_WORD_LE(TSpiDriver * spi, uint16 x) ;
void FASTSPI_TX_WORD(TSpiDriver * spi, uint16 x); 
void FASTSPI_RX_WORD_LE(TSpiDriver * spi, uint16 *x); 
void FASTSPI_RX_WORD(TSpiDriver * spi, uint16 *x); 


#endif /*_SPI_H_*/
