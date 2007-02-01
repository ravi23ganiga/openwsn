#include "hal_target.h"
#include "hal_cc2420.h"
#include "hal_led.h"
#include "hal_mcp6s.h"
#include "hal_sensor_strain.h"
#include "hal_sensor_switch.h"
#include "hal_sensor_vibration.h"

/* initialize the target hardware 
 * this function MUST run successfully or the later code will dead.
 */
void target_init( void )
{
    uint16 Fdiv;
    
    #ifdef TARGET_OPENNODE_10
    PINSEL0 = PINSEL0 | 0x000c0000;                   //FIFOP is EXTIN3	
    #endif
    
    #ifdef TARGET_OPENNODE_20
    PINSEL0 = PINSEL0 | 0x80000000;                   //FIFOP is EXTIN2	
    PINSEL0 = PINSEL0 & 0xbfffffff;
    
    PINSEL1 = PINSEL1 | 0x10000000; 
    PINSEL1 = PINSEL1 & 0xdfffffff;                   //P0.30 is AD0.3
    
    #if     AVCCS_SWITCH_PORT == 0    
    IO0DIR |= BM(AVCCS_SWITCH_PIN);  
    #endif
    
    #if     AVCCS_SWITCH_PORT == 1             
    IO1DIR |= BM(AVCCS_SWITCH_PIN);  
    #endif
    
    #if     MCP6S26_SPI_CS_PORT == 0    
    IO0DIR |= BM(MCP6S26_SPI_CS_PIN);  
    #endif
    
    #if     MCP6S26_SPI_CS_PORT == 1             
    IO1DIR |= BM(MCP6S26_SPI_CS_PIN);  
    #endif  
    #endif
    
    #ifdef TARGET_WLSMODEM_11
    PINSEL0 = PINSEL0| 0x80000000;                   //FIFOP are EXTIN2	
    PINSEL0 = PINSEL0& 0xbfffffff;
    #endif
   
    #if     VIBRATION_SLEEP_PORT == 0    
    IO0DIR |= BM(VIBRATION_SLEEP_PIN);  
    #endif
    
    #if     VIBRATION_SLEEP_PORT == 1             
    IO1DIR |= BM(VIBRATION_SLEEP_PIN);  
    #endif
      
    #if     LED_GREEN_PORT == 0    
    IO0DIR |= BM(LED_GREEN_PIN);  
    #endif
    
    #if     LED_GREEN_PORT == 1             
    IO1DIR |= BM(LED_GREEN_PIN);  
    #endif


    #if     LED_RED_PORT == 0     
    IO0DIR |= BM(LED_RED_PIN);    
    #endif
    
    #if     LED_RED_PORT == 1                     
    IO1DIR |= BM(LED_RED_PIN);   
    #endif
    
    
    #if     LED_YELLOW_PORT == 0   
    IO0DIR |= BM(LED_YELLOW_PIN); 
    #endif
    
    #if     LED_YELLOW_PORT == 1                          
    IO1DIR |= BM(LED_YELLOW_PIN); 
    #endif
    
    
    
    #if     RESET_N_PORT == 0      
    IO0DIR |= BM(RESET_N);    
    #endif
    
    #if     RESET_N_PORT == 1                          
    IO1DIR |= BM(RESET_N);    
    #endif


    #if     VREG_EN_PORT == 0      
    IO0DIR |= BM(VREG_EN);    
    #endif
    
    #if     VREG_EN_PORT == 1                          
    IO1DIR |= BM(VREG_EN);    
    #endif
   
    
    #if     CSN_PORT == 0          
    IO0DIR |= BM(CSN);       
    #endif
    
    #if     CSN_PORT == 1                         
    IO1DIR |= BM(CSN);       
    #endif
    
    #if     CCA_PORT == 0          
    IO0DIR &= (~BM(CCA));     
    #endif
    
    #if     CCA_PORT == 1                          
    IO1DIR &= (~BM(CCA));     
    #endif
    
    
    #if     SFD_PORT == 0          
    IO0DIR &= (~BM(SFD));    
    #endif
    
    #if     SFD_PORT == 1                         
    IO1DIR &= (~BM(SFD));     
    #endif
    
    
    #if     FIFO_PORT == 0        
    IO0DIR &= (~BM(FIFO));    
    #endif
    
    #if     FIFO_PORT == 1                        
    IO1DIR &= (~BM(FIFO));    
    #endif
 
    
    /* initialze UART0 for startup using. 
     * the default configurations will be used. you may change it in the future 
     * @modified by zhangwei on 20060628
     * huanghuan delete the following section.
     * but i think UART0 config should be kept here. that's better than to delete them.
     */
    PINSEL0 = (PINSEL0 & (~0x0f)) | 0x05;             //UART0 INIT
    U0LCR = 0x83;                   // DLAB=1,允许设置波特率
    Fdiv  = (Fpclk / 16) / 115200;	// 设置波特率
    U0DLM = Fdiv / 256;
    U0DLL = Fdiv % 256;
    U0LCR = 0x03;	
     
    return; 
}    
	





