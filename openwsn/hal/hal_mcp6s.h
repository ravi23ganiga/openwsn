#ifndef _HAL_MCP6S_H_3467_
#define _HAL_MCP6S_H_3467_

/******************************************************************************
 * @author huanghuan on 200612
 * @note
 * 	MCP Driver for Microchip MCP6S26
 * @history
 * @modified by zhangwei on 20070101
 * 	revise source code from MaKun
 * 	adjust the interface
 *****************************************************************************/
 
#include "hal_foundation.h"
#include "hal_configall.h"
#include "hal_spi.h"


#define MCP6S26_SPI_CS_PIN 24
#define MCP6S26_SPI_CS_PORT 1

typedef struct{
  uint8 id;
  TSpiDriver * spi;
}TMcp6s26;

TMcp6s26 * mcp_construct(  uint8 id, char * buf, uint8 size, TSpiDriver * spi  );
void mcp_destroy( TMcp6s26 * mcp );

void mcp6s26_spi_enable(void);
void mcp6s26_spi_disable(void);
void mcp6s26_setchannel(TMcp6s26 * mcp,uint8 channel);
void mcp6s26_setgains( TMcp6s26 * mcp, uint8 ratio );

#endif
