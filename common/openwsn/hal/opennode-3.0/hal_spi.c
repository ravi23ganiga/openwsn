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

#ifdef CONFIG_DEBUG
#define GDEBUG
#endif

#include "hal_foundation.h"
#include "hal_spi.h"
#include "hal_lpc213x.h"
#ifdef GDEBUG
#include "hal_led.h"
#include "hal_uart.h"
#include "hal_global.h"
#endif

/***************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TiSpiAdapter 
 *
 * @modified by zhangwei on 20061010
 * revision
 * add support to OpenNode-3.0 target hardware 
 * based on Huanghuan's mature code. 
 * support multiple SPI channel.
 * 
 * @modified by zhangwei on 20070701
 * modify spi_put() to support return value. this feature is useful for SPI communication. 
 * some program may use the returned character by the peer spi device, such as cc2420
 *
 ****************************************************************************/

#if ((!defined(CONFIG_TARGET_OPENNODE_10)) && (!defined(CONFIG_TARGET_OPENNODE_20)) \
  	&& (!defined(CONFIG_TARGET_OPENNODE_30)) && (!defined(CONFIG_TARGET_WLSMODEM_11)))
#define CONFIG_TARGET_DEFAULT
#endif

// P1.21 - Output: SPI Chip Select (CS_N)
#ifdef CONFIG_TARGET_OPENNODE_10 
#define CSN            21  
#endif

// P1.21 - Output: SPI Chip Select (CS_N)
#ifdef CONFIG_TARGET_OPENNODE_20 
#define CSN            21  
#endif

// P1.21 - Output: SPI Chip Select (CS_N)
#ifdef CONFIG_TARGET_OPENNODE_30 
#define CSN            21  
#endif

// P1.17 - Output: SPI Chip Select (CS_N)
#ifdef CONFIG_TARGET_WLSMODEM_11
#define CSN            17  
#endif

#ifdef CONFIG_TARGET_DEFAULT 
#define CSN            21   
#endif

TiSpiAdapter * spi_construct( uint8 id, char * buf, uint8 size )
{
	TiSpiAdapter * spi;
	
	if (sizeof(TiSpiAdapter) <= size)
		spi = (TiSpiAdapter *)buf;
	else
		spi = NULL;
		
	if (spi != NULL)
	{
		memset( buf, 0x00, size );
		spi->id = id;
	}
	
	return spi;
}

void spi_destroy( TiSpiAdapter * spi )
{
	NULL;
}

/* configure the SPI here. for example, the interrupt settings 
 */
void spi_configure( TiSpiAdapter * spi )
{
	if (spi->id == 0)
    {
    	PINSEL0 = (PINSEL0 & 0xffff00ff) | 0x00005500;
    	#if (defined(CONFIG_TARGET_OPENNODE_10) || defined(CONFIG_TARGET_OPENNODE_20))
        SPI_SPCCR = 0x52;		               // ����SPIʱ�ӷ�Ƶ for 11.0592 Khz 
        #elif defined(CONFIG_TARGET_OPENNODE_30)
        SPI_SPCCR = 0x08;
        #else
        SPI_SPCCR = 0x52;
        #endif
		
 	  	SPI_SPCR  = (0 << 3) |		       // CPHA = 0, ������SCK �ĵ�һ��ʱ���ز���
 			        (0 << 4) |	       // CPOL = 0, SCK Ϊ����Ч
 			        (1 << 5) |	       // MSTR = 1, SPI ������ģʽ
 			        (0 << 6) |	       // LSBF = 0, SPI ���ݴ���MSB (λ7)����
 			        (0 << 7);	       // SPIE = 0, SPI �жϱ���ֹ
	}

	else if (spi->id == 1)
    {
		PINSEL1 = (PINSEL1 & (~(0xFF << 2))) | (0xAA << 2);
        
        SSPCR0 = (0x01 << 8) |              // SCR  ����SPIʱ�ӷ�Ƶ
        	(0x00 << 7) |              // CPHA ʱ�������λ,��SPIģʽ��Ч 
            (0x00 << 6) |              // CPOL ʱ���������,��SPIģʽ��Ч
            (0x00 << 4) |              // FRF  ֡��ʽ 00=SPI,01=SSI,10=Microwire,11=����
            (0x07 << 0);               // DSS  ���ݳ���,0000-0010=����,0011=4λ,0111=8λ,1111=16λ

		SSPCR1 = (0x00 << 3) |              // SOD  �ӻ��������,1=��ֹ,0=����
        	(0x00 << 2) |              // MS   ����ѡ��,0=����,1=�ӻ�
            (0x01 << 1) |              // SSE  SSPʹ��,1=����SSP�������豸ͨ��
            (0x00 << 0);               // LBM  ��дģʽ
             
		SSPCPSR = 0x52;                     // PCLK��Ƶֵ
        //SSPIMSC = 0x07;                     // �ж����μĴ���
        SSPICR  = 0x03;                     // �ж�����Ĵ��� 
	}
}

/* open() and close() essentially control the SPI's CS pin.
 * there're the followin conditions you should use open() before you transmit 
 * a byte through SPI:
 * - you want to select a specific device connect to SPI. you can choose the 
 * 	device through parameter "devid" in spi_open()
 * - you want to adjust the detail timling when you try to sent multi-byte streams.
 *  open() will prepare the SPI for transmitting multi-byte stream.
 * 
 * @attention
 * you should call spi_configure() before spi_open() and spi_read/write()
 * 
 * @obsolete
 * spi_open() and spi_close() originally known as spi_enable() and spi_disable()
 * in module "cc2420rf"
 */
void spi_open( TiSpiAdapter * spi, uint8 devid )
{
    uint16 i = 0;
      
	// @TODO
	// you should distinguish SPI0 and SPI1
	
    #ifdef CONFIG_TARGET_OPENNODE_10
    IO1CLR = BM(CSN);
    #endif
        
    #ifdef CONFIG_TARGET_OPENNODE_20
    IO1CLR = BM(CSN);
    #endif
        
    #ifdef CONFIG_TARGET_OPENNODE_30
    IO1CLR = BM(CSN);
    #endif
        
    #ifdef CONFIG_TARGET_WLSMODEM_11
    IO0CLR  = BM(CSN);
    #endif
              
    #ifdef CONFIG_TARGET_DEFAULT
    IO1CLR = BM(CSN);
    #endif
        
    // the delay is to construct enough setup time of csn
    // attention the delay time not to be optimized to 0 
	//
    // @TODO you should use hal_delay here
	//#pragma optimize=none
    while(i < 500) 
    	i++;    
}

void spi_close( TiSpiAdapter * spi )
{
	uint16 i = 0;
        
    // the delay is to provide enough holdup time of csn
    // attention the delay time not to be optimized to 0 
	// @TODO
    while(i < 1500) 
    	i++;
        	
	#ifdef CONFIG_TARGET_OPENNODE_10
    IO1SET = BM(CSN);
    #endif
        
    #ifdef CONFIG_TARGET_OPENNODE_20
    IO1SET = BM(CSN);
    #endif
        
    #ifdef CONFIG_TARGET_OPENNODE_30
    IO1SET = BM(CSN);
    #endif
        
    #ifdef CONFIG_TARGET_WLSMODEM_11
    IO0SET  = BM(CSN);
	#endif

    #ifdef CONFIG_TARGET_DEFAULT
    IO1SET = BM(CSN);
    #endif
}

uint8 spi_read(TiSpiAdapter * spi, char * buf, uint8 capacity, uint8 opt )
{
	uint8 n = 0;
        
    for (n = 0; n < capacity; n++) 
	{ 
		spi_get(spi,buf + n); 
	} 
       
    return 0;
}

uint8 spi_write(TiSpiAdapter * spi,  char * buf, uint8 len, uint8 opt )
{
	uint8 n = 0;
    for (n = 0; n < len; n++) 
	{ 
		spi_put(spi,((BYTE*)(buf))[n]); 
	} 
      
    return 0;
}

uint8 spi_put(TiSpiAdapter * spi, char ch )
{
	uint8 ret = 0;
	
	#ifdef GDEBUG
	//uart_write( g_uart, "spi_put:\r\n", 10, 0x00 );
	#endif
	
    if (spi->id == 0)
    {
    	// SPI_SPSR; // clear all the flags
		SPI_SPDR = ch; 
        spi_wait(spi);
        ret = SPI_SPDR; 
	}
    else if (spi->id == 1)
    {
		SSPDR = ch;
        spi_wait(spi);	
        ret = SSPDR; 
	}
	
	return ret;
}

int8 spi_get(TiSpiAdapter * spi, char * pc )
{
    if(spi->id == 0)
    {
		SPI_SPDR = 0; 
        spi_wait(spi); 
        *pc = SPI_SPDR; 
	}
    if(spi->id == 1)
    {
		SSPDR = 0;
        spi_wait(spi);
        *pc = SSPDR; 	
	}
        
    return 0;
}

uint8 spi_status( TiSpiAdapter * spi )
{
	if (spi->id == 0)
	{
		return SPI_SPSR;
	}
	
	return 0x00;
}

void spi_wait(TiSpiAdapter * spi) 
{ 
	#ifdef GDEBUG
	uint8 status;
	#endif
	
    if (spi->id == 0) 
    {	
		#ifdef GDEBUG
    	//uart_write( g_uart, "spi_wait:\r\n", 11, 0x00 );
		while (TRUE)
		{
			status = SPI_SPSR;
			//uart_putchar( g_uart, status ); 
			if (status & 0x0010)
			{
				// Mode Error: 
				SPI_SPCR  = SPI_SPCR | 0x10; // MSTR = 1, SPI ������ģʽ
				hal_delay( 10 ); // @TODO: for test only, this should never happen!
				break;
			}

			if (status & 0x80)
				break;
		}
		#endif

		#ifndef GDEBUG
		//while  (!(SPI_SPSR & BM(7)));	
		while (TRUE)
		{
			status = SPI_SPSR;
			if (status & 0x0080)
				break;
		}
		#endif
	}
    else if(spi->id == 1) 
    {
		while (!(SSPSR & 0x01));
	}
}


// @TODO
// obsolete functions
// will be eliminated soon
//
void FASTSPI_TX_WORD_LE(TiSpiAdapter * spi, uint16 x) 
    { 
        spi_put(spi,x); 
        spi_put(spi,(x) >> 8); 
    }
    
void FASTSPI_TX_WORD(TiSpiAdapter * spi, uint16 x) 
    { 
        spi_put(spi,(x) >> 8); 
        spi_put(spi,(BYTE)(x)); 
    }
    

void FASTSPI_RX_WORD_LE(TiSpiAdapter * spi, uint16 *x) 
    { 
    	char temp;
    	spi_get(spi,&temp);
    	*x = (WORD)temp;
    	spi_get(spi,&temp);
    	*x |= ((WORD)temp) << 8;
        //spi_put(spi,0);
        //*x = (WORD)SPI_SPDR; 
        //spi_put(spi,0);
        //*x |= ((WORD)(SPI_SPDR)) << 8; 
    } 

void FASTSPI_RX_WORD(TiSpiAdapter * spi,uint16 *x) 
    { 
        char temp;
    	spi_get( spi, &temp );
    	*x = ((WORD)temp) << 8;
    	spi_get(spi,&temp);
    	*x |= ((WORD)temp);
        //spi_put(0); 
        //*x = ((WORD)SPI_SPDR) << 8; 
        //spi_put(0);
        //*x |= (WORD)SPI_SPDR; 
    } 

