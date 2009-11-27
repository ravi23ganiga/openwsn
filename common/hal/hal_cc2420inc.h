#ifndef _CC2420_BASE_H_7887_
#define _CC2420_BASE_H_7887_

/*******************************************************************************
 * hal_cc2420base
 * This module contains the most fundamental definitions for the cc2420 module. 
 * The motivation of this module is to organize those platform-dependent source
 * code into one file. So if you want to port the cc2420 module to a new hardware
 * architecture and platform, you can simply revise this file only instead of 
 * revising the whole hal_cc2420.c
 *
 * attention:
 *	Only the most fundamental source code should be placed in this file.
 *
 * @author zhangwei(TongJi University), HuangHuan(TongJi University) on 20061025
 *	- first created
 * @modified by zhangwei(TongJi University) on 20091104
 *	- revision. Simplified this file. Only those functions and macros which affect 
 * porting to other platforms are kept. 
 ******************************************************************************/

// todo: hal_cc2420中最基础的底层操作函数特别是spi操作和几根pin的读写将转移到该模块中，如果移植，则只要移植该模块即可

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_target.h"
#include "hal_cc2420const.h"

/*******************************************************************************
 * cc2420 PIN manipulations
 * SPI Pin(MISO, MOSI, SCK), CS, RSTN, VERN, FIFOP, FIFO, CCA, SFD
 ******************************************************************************/

#define CC_MAKE_MISO_INPUT()        HAL_MAKE_MISO_INPUT()
#define CC_MAKE_MOSI_OUTPUT()       HAL_MAKE_MOSI_OUTPUT()
#define CC_MAKE_SPI_SCK_OUTPUT()    HAL_MAKE_SPI_SCK_OUTPUT()

#define CC_MAKE_CS_OUTPUT()         HAL_MAKE_CC_CS_OUTPUT()
#define CC_MAKE_RSTN_OUTPUT()       HAL_MAKE_CC_RSTN_OUTPUT()
#define CC_MAKE_VREN_OUTPUT()       HAL_MAKE_CC_VREN_OUTPUT()
#define CC_MAKE_FIFOP_INPUT()       HAL_MAKE_CC_FIFOP_INPUT()
#define CC_MAKE_CCA_INPUT()         HAL_MAKE_CC_CCA_INPUT()
#define CC_MAKE_SFD_INPUT()         HAL_MAKE_CC_SFD_INPUT()
#define CC_MAKE_FIFO_INPUT()        HAL_MAKE_CC_FIFO_INPUT()

/* CLR(clear) will set the PIN to low voltage as 0. SET will set it to high voltage as 1 */

#define CC_CLR_CS_PIN()             HAL_CLR_CC_CS_PIN()
#define CC_SET_CS_PIN()             HAL_SET_CC_CS_PIN()

#define CC_READ_SFD_PIN()           HAL_READ_CC_SFD_PIN()
#define CC_READ_CCA_PIN()           HAL_READ_CC_CCA_PIN()
//#define CC_READ_FIFOP_PIN()         HAL_READ_CC_FIFOP_PIN()
#define CC_READ_FIFO_PIN()          HAL_READ_CC_FIFO_PIN()

#define CC_READ_FIFOP_PIN()         (PINE&(1<<6))? (true):(false)



/*******************************************************************************
 * cc2420 pin initialization
 ******************************************************************************/



/*******************************************************************************
 * cc2420 spi interface manipulations
 ******************************************************************************/

/* _cc2420_spi_init()
 * Initialize the MCU's SPI serial communication module. The software will use this
 * module to communicate with cc2420 transceiver.
 */

/* this function replace the obsolete _cc2420_spi_init */

inline void _cc2420_spi_open()
{
	cpu_atomic_t state;
	
	state = _cpu_atomic_begin();
	HAL_MAKE_SPI_SCK_OUTPUT();
	HAL_MAKE_MISO_INPUT();
	HAL_MAKE_MOSI_OUTPUT();
	SPSR |= 1 << 0;			
	SPCR |= 1 << 4;
	SPCR &= ~(1 << 3);
	SPCR &= ~(1 << 2);
	SPCR &= ~(1 << 1);
	SPCR &= ~(1 << 0);
	SPCR |= 1 << 6;
    _cpu_atomic_end(state); 
}

inline uint8 _cc2420_spi_put( uint8 value )
{
	SPDR = value;
    while (!(SPSR & 0x80)) {};
	return SPDR;
}

inline uint8 _cc2420_spi_get()
{
	SPDR = 0;
	while (!(SPSR & 0x80)) {};
	return SPDR;
}

#endif
