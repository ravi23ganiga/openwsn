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
#ifndef _HAL_CC2420RF_H_3957_
#define _HAL_CC2420RF_H_3957_

/*****************************************************************************
 * @author zhangwei on 2006-07-20
 * TCc2420Driver
 * this file is part of the TCc2420Driver
 * �󲿷�TCc2420Driver���붼Ӧ�÷��ڸ��ļ��У�hal_cc2420.* ����Ҫʵ�ֽӿں������߼���
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