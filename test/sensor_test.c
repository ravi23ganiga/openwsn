#include <stdlib.h>
#include "config.h"
#include "..\hal\hal.h"
#include "start.h"
#include "..\hal\hal_cc2420rf.h"

#define MCP6S26_TEST
//#define VIBRATION_TEST


void sensor_test()
{
    uint16 ad_value = 0;
    char * enter = "\n";
    
    target_init();
    global_construct();
    led_twinkle(LED_YELLOW,1);
    spi_configure( g_spi0 );
    uart_configure( g_uart, 115200, 0, 0, 0, 0 );
    //ad_configutre(g_ad);
    
    SET_VREG_INACTIVE();
    
    avccs_on();
    
    led_twinkle(LED_RED,1);
    //led_twinkle(LED_YELLOW,1);
    
    #ifdef MCP6S26_TEST
    while(1) 
    {
       //avccs_on();
       mcp6s26_setchannel(g_mcp6s26,0);
       //ad_value = ad_read(g_ad,NULL,0,0);
       //uart_putchar( g_uart,(char)ad_value);
       //uart_putchar( g_uart,(char)(ad_value >> 8));
       //uart_putchar( g_uart,*enter);
       led_twinkle(LED_YELLOW,3);
       
       
       mcp6s26_setchannel(g_mcp6s26,4);
       //ad_value = ad_read(g_ad,NULL,0,0);
       //uart_putchar( g_uart,(char)ad_value);
       //uart_putchar( g_uart,(char)(ad_value >> 8));
       //uart_putchar( g_uart,*enter);
       led_twinkle(LED_GREEN,3);
    }	
    #endif
    
    #ifdef VIBRATION_TEST
    while(1) 
    {
       led_twinkle(LED_RED,1);	
    }
    #endif
}