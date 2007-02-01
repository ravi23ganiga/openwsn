#ifndef _HAL_CC2420RF_H_3957_
#define _HAL_CC2420RF_H_3957_

/*****************************************************************************
 * @author zhangwei on 2006-07-20
 * TCc2420Driver
 * this file is part of the TCc2420Driver
 * 大部分TCc2420Driver代码都应该放在该文件中，hal_cc2420.* 中主要实现接口和配置逻辑。
 * 
 * @history
 * @modified by zhangwei on 2006-06-24
 *	this header file replaced the old "basic_rf.h" provided by HuangHuan.
 * 
 ****************************************************************************/

#include "hal_foundation.h"
#include "hal_spi.h"
#include "hal_cc2420.h"

//----------------------------------------------------------------------------
// Gerneal Constants
//----------------------------------------------------------------------------

// Constants concerned with the Basic RF packet format
//
// Packet overhead ((frame control field, sequence number, PAN ID, destination and source) + (footer))
// Note that the length byte itself is not included included in the packet length

#define BASIC_RF_PACKET_OVERHEAD_SIZE   ((2 + 1 + 2 + 2 + 2) + (2))
#define BASIC_RF_MAX_PAYLOAD_SIZE		(127 - BASIC_RF_PACKET_OVERHEAD_SIZE)
#define BASIC_RF_ACK_PACKET_SIZE		5

// The time it takes for the acknowledgment packet to be received after the data packet has been
// transmitted
#define BASIC_RF_ACK_DURATION			(0.5 * 32 * 2 * ((4 + 1) + (1) + (2 + 1) + (2)))
#define BASIC_RF_SYMBOL_DURATION	    (32 * 0.5)

// The length byte
#define BASIC_RF_LENGTH_MASK            0x7F

// Frame control field
#define BASIC_RF_FCF_NOACK              0x8841
#define BASIC_RF_FCF_ACK                0x8861
#define BASIC_RF_FCF_ACK_BM             0x0020
#define BASIC_RF_FCF_BM                 (~BASIC_RF_FCF_ACK_BM)
#define BASIC_RF_ACK_FCF		        0x0002

// Footer
#define BASIC_RF_CRC_OK_BM              0x80


/*******************************************************************************
 * some utility functions used in hal_cc2420rf.c
 ******************************************************************************/
 
void FAST2420_RX_GARBAGE(TSpiDriver * spi,char *pc)  ;
void FAST2420_TX_ADDR(TSpiDriver * spi,uint8 a); 
void FAST2420_RX_ADDR(TSpiDriver * spi,uint8 a); 
void FAST2420_STROBE(TSpiDriver * spi,uint8 s); 
void FAST2420_SETREG(TSpiDriver * spi,uint8 a,uint16 v); 
void FAST2420_GETREG(TSpiDriver * spi,uint8 a,uint16 *v); 
void FAST2420_UPD_STATUS(TSpiDriver * spi,uint8 *s); 
void FAST2420_WRITE_FIFO(TSpiDriver * spi,uint8 *p,uint8 c); 
void FAST2420_READ_FIFO(TSpiDriver * spi,uint8 *p,uint8 c); 
void FAST2420_READ_FIFO_BYTE(TSpiDriver * spi,uint8 *b); 
void FAST2420_READ_FIFO_NO_WAIT(TSpiDriver * spi,uint8 *p, uint8 c); 
void FAST2420_READ_FIFO_GARBAGE(TSpiDriver * spi,uint8 c); 
void FAST2420_WRITE_RAM_LE(TSpiDriver * spi,uint16 *p,uint16 a,uint8 c); 
void FAST2420_READ_RAM_LE(TSpiDriver * spi,uint8 *p,uint16 a,uint8 c); 
void FAST2420_WRITE_RAM(TSpiDriver * spi,uint16 *p,uint16 a,uint8 c); 
void FAST2420_READ_RAM(TSpiDriver * spi,uint8 *p, uint16 a, uint8 c); 
void FAST2420_RESET_CC2420( TSpiDriver * spi ); 

void CC2420_SPI_ENABLE( void );
void CC2420_SPI_DISABLE( void );
void SET_RESET_INACTIVE( void ); // // The CC2420 reset pin
void SET_RESET_ACTIVE( void );
void SET_VREG_ACTIVE( void ); // CC2420 voltage regulator enable pin      
void SET_VREG_INACTIVE( void );
BOOL VALUE_OF_FIFO( void );
BOOL VALUE_OF_CCA( void );
BOOL VALUE_OF_FIFOP( void );
BOOL VALUE_OF_SFD( void );
// BOOL VALUE_OF_RESET( void );    // not used in this file
// BOOL VALUE_OF_VREG(void);     // not used in this file


//----------------------------------------------------------------------------
#endif