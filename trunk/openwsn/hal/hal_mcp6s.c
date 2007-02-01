#include "hal_mcp6s.h"
#include "hal_spi.h"
#include "hal_uart.h"
#include "hal_global.h"

/* @param
 * 	spi		point to an outside spi driver
 * 			assume the spi driver is OK
 */
TMcp6s26 * mcp_construct(  uint8 id, char * buf, uint8 size, TSpiDriver * spi )
{
	TMcp6s26 * mcp;
	char* out_string = "mcp consturct succesful!\n";
	
	if (sizeof(TMcp6s26) > size)
		mcp = NULL;
	else
		mcp = (TMcp6s26 *)buf;
		
	
	if (mcp != NULL)
	{	
		memset( (char*)mcp, 0x00, sizeof(TMcp6s26) );
	        mcp->id = id;
	        mcp->spi = spi;
	        uart_write(g_uart, out_string,25, 0);
	}
	
	return mcp;
}

void mcp_destroy( TMcp6s26 * mcp )
{
}


void mcp6s26_spi_enable()
{
    uint16 i = 0;
    
    #if MCP6S26_SPI_CS_PORT == 0 
    IO0CLR  = BM(MCP6S26_SPI_CS_PIN);   
    #endif
	         
    #if MCP6S26_SPI_CS_PORT == 1 
    IO1CLR  = BM(MCP6S26_SPI_CS_PIN);   
    #endif	
    
    while(i < 500) i++;	
}

void mcp6s26_spi_disable()
{
    uint16 i = 0;
    
    while(i < 1500) i++;
    
    #if MCP6S26_SPI_CS_PORT == 0 
    IO0SET  = BM(MCP6S26_SPI_CS_PIN);   
    #endif
             
    #if MCP6S26_SPI_CS_PORT == 1 
    IO1SET  = BM(MCP6S26_SPI_CS_PIN);   
    #endif		
}

void mcp6s26_setchannel(TMcp6s26 * mcp,uint8 channel)
{
    uint8 ch;
    if(channel <= 5 ) ch = channel;
    else              ch = 0;
    mcp6s26_spi_enable();	
    spi_put(mcp->spi,0x41);
    spi_put(mcp->spi,ch);
    mcp6s26_spi_disable();
}

void mcp_setgains( TMcp6s26 * mcp, uint8 ratio )
{
     uint8 ch;
    if(ratio <= 7 ) ch = ratio;
    else            ch = 0;
    mcp6s26_spi_enable();	 
    spi_put(mcp->spi,0x40);
    spi_put(mcp->spi,ch);
    mcp6s26_spi_disable();
}