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
/***************************************************************************** 
 * @author zhangwei on 2006-07-20
 * TSPIDriver 
 *
 * @modified by zhangwei on 20061010
 * based on Huanghuan's mature code. 
 * support multiple SPI channel.
 * 
 ****************************************************************************/
#include "hal_spi.h"
#include "hal_led.h"


TSpiDriver * spi_construct( uint8 id, char * buf, uint8 size )
{
	TSpiDriver * spi;
	
	if (sizeof(TSpiDriver) <= size)
		spi = (TSpiDriver *)buf;
	else
		spi = NULL;
		
	if (spi != NULL)
	{
		memset( buf, 0x00, size );
		spi->id = id;
	}
	
	// @modified by zhangwei on 20061025
	//
	// I delete spi_configure(spi)
	// this line is in huanghuan's code 
	
	return spi;
}

void spi_destroy( TSpiDriver * spi )
{
}

/* configure the SPI here. for example, the interrupt settings 
 */
void spi_configure( TSpiDriver * spi )
{
        if (spi->id == 0)
        {
          PINSEL0 = (PINSEL0 & 0xffff00ff) | 0x00005500;
          SPI_SPCCR = 0x52;		               // 设置SPI时钟分频
 	  SPI_SPCR  = (0 << 3) |		       // CPHA = 0, 数据在SCK 的第一个时钟沿采样
 			        (0 << 4) |	       // CPOL = 0, SCK 为高有效
 			        (1 << 5) |	       // MSTR = 1, SPI 处于主模式
 			        (0 << 6) |	       // LSBF = 0, SPI 数据传输MSB (位7)在先
 			        (0 << 7);	       // SPIE = 0, SPI 中断被禁止
        }
        
        
        
        else if (spi->id == 1)
        {
        
          PINSEL1 = (PINSEL1 & (~(0xFF << 2))) | (0xAA << 2);
        
          SSPCR0 = (0x01 << 8) |              // SCR  设置SPI时钟分频
             (0x00 << 7) |              // CPHA 时钟输出相位,仅SPI模式有效 
             (0x00 << 6) |              // CPOL 时钟输出极性,仅SPI模式有效
             (0x00 << 4) |              // FRF  帧格式 00=SPI,01=SSI,10=Microwire,11=保留
             (0x07 << 0);               // DSS  数据长度,0000-0010=保留,0011=4位,0111=8位,1111=16位

          SSPCR1 = (0x00 << 3) |              // SOD  从机输出禁能,1=禁止,0=允许
             (0x00 << 2) |              // MS   主从选择,0=主机,1=从机
             (0x01 << 1) |              // SSE  SSP使能,1=允许SSP与其它设备通信
             (0x00 << 0);               // LBM  回写模式
             
          SSPCPSR = 0x52;                     // PCLK分频值
          //SSPIMSC = 0x07;                     // 中断屏蔽寄存器
          SSPICR  = 0x03;                     // 中断清除寄存器 
        }
	
}

uint8 spi_read(TSpiDriver * spi, char * buf, uint8 capacity, uint8 opt )
{
        UINT8 spiCnt = 0;
        
        for (spiCnt = 0; spiCnt < capacity; spiCnt++) { 
            spi_get(spi,buf + spiCnt); 
        } 
       
        return 0;
}

uint8 spi_write(TSpiDriver * spi,  char * buf, uint8 len, uint8 opt )
{
        uint8 spiCnt = 0;
        for (spiCnt = 0; spiCnt < len; spiCnt++) { 
            spi_put(spi,((BYTE*)(buf))[spiCnt]); 
        } 
      
        return 0;
}

void spi_put(TSpiDriver * spi, char ch )
{
        if (spi->id == 0)
        {
          SPI_SPDR = ch; 
          spi_wait(spi);
        }
        else if (spi->id == 1)
        {
          SSPDR = ch;
          spi_wait(spi);	
        }
}

int8 spi_get(TSpiDriver * spi, char * pc )
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


void spi_wait(TSpiDriver * spi) 
{ 
        if (spi->id == 0) 
        {	
        	while (!(SPI_SPSR & BM(7)));	
    	}
        else if(spi->id == 1) 
        {
        	while (!(SSPSR & 0x01));
        }
}


void FASTSPI_TX_WORD_LE(TSpiDriver * spi, uint16 x) 
    { 
        spi_put(spi,x); 
        spi_put(spi,(x) >> 8); 
    }
    
void FASTSPI_TX_WORD(TSpiDriver * spi, uint16 x) 
    { 
        spi_put(spi,(x) >> 8); 
        spi_put(spi,(BYTE)(x)); 
    }
    

void FASTSPI_RX_WORD_LE(TSpiDriver * spi, uint16 *x) 
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

void FASTSPI_RX_WORD(TSpiDriver * spi,uint16 *x) 
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

