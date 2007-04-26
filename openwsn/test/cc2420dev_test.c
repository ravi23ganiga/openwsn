/****************************************Copyright (c)**************************************************
**                               Guangzou ZLG-MCU Development Co.,LTD.
**                                      graduate school
**                                 http://www.zlgmcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			main.c
** Last modified Date:  2004-09-16
** Last Version:		1.0
** Descriptions:		The main() function example template
**
**------------------------------------------------------------------------------------------------------
** Created by:			Chenmingji
** Created date:		2004-09-16
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:

 * @modified by zhangwei on 20061013 
 * 黄欢写的cc2420driver测试主程序，张伟整理 20061013
 * 
 * @modified by zhangwei on 20061019
 * revised today. mainly introduce the global variables.

**
********************************************************************************************************/

#include <stdlib.h>
#include "config.h"
#include "..\hal\hal.h"
#include "start.h"


#define TX
//#define RX


#define PACKET
//#define CHAR_STREAM

uint8 tx_frame[128];
uint8 rx_frame[128];            //using CHAR_STREAM

TCc2420Frame tx_test;
TCc2420Frame rx_test;           //using PACKET


int cc2420dev_test (void)
{
    ///*     Fof RF test	
    UINT8 n;
    int8 length;
    uint16 temp;
    uint8 ledPeriod;
    char * out_string = "the rssi value is : ";
    char * enter      = "\n";
  
    target_init();
    
    global_construct();
    spi_configure( g_spi );
    uart_configure( g_uart, 115200, 0, 0, 0, 0 );
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0);
    
    tx_test.panid = 0x2420;
    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, tx_test.panid, 0);
    
    #ifdef TX	
    tx_test.nodeto = 0x5678;
    tx_test.nodefrom = 0x1234;
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, tx_test.nodefrom, 0);
    #endif

    
    #ifdef RX	
    tx_test.nodeto = 0x1234;
    tx_test.nodefrom = 0x5678;
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, tx_test.nodefrom, 0);
    #endif	
    

    //to write the payload
    for (n = 0; n < 10; n++) {
        tx_test.payload[n] = 2;
        tx_frame[9 + n] = 2;
    }
    tx_test.payload[2] = 1;  

    tx_frame[0] = tx_frame[1] = tx_frame[2] = 0;
    tx_frame[3] = 0x20; tx_frame[4] = 0x24;
    tx_frame[5] = 0x78; tx_frame[6] = 0x56;
    tx_frame[7] = 0x34; tx_frame[8] = 0x12; 
    
    
    cc2420_receive_on(g_cc2420);  
    IRQEnable(); 
	
	// The main loop:
	#ifdef TX
	while (TRUE) 
	{    
          
          //transmit using packet
          #ifdef PACKET
          tx_test.payload[0]++;
          if(tx_test.payload[0] == 5) tx_test.payload[0] = 1;
          
          led_twinkle(LED_GREEN,1);
          
          length = cc2420_write(g_cc2420,tx_test,10 + 11,0);
          
          /*
          if(length == -1) {led_twinkle(LED_RED,5);uart_putchar(g_uart,(char)0x00);}
          else {led_twinkle(LED_RED,1);uart_putchar(g_uart,(char)0x11);}
          */ 
	  halWait(2000);
	  #endif
	  /////////////////////////////////////////
	  
	  
	  
	  //transmit using char streams
	  #ifdef CHAR_STREAM         
          tx_frame[9]++;
          if(tx_frame[9] = 5) tx_frame[9] = 1;
          cc2420_rawwrite( g_cc2420, (char *)tx_frame, 10 + 11,0);
          
          led_twinkle(LED_GREEN,1);
       	  //cc2420_sendframe(g_cc2420);
	  halWait(3000);	  
	  #endif
	  ///////////////////////////////////////
        }
        #endif
        
        
        #ifdef RX
        ledPeriod = 1;
        while (TRUE) 
	{ 
	  
	  //receive using packet
          #ifdef PACKET
	  led_twinkle(LED_GREEN,ledPeriod);
	  
	  length = cc2420_read( g_cc2420,&rx_test,0,0);
	  
	  if(length > 11) {
	  ledPeriod = rx_test.payload[0];
	  
	  temp = g_cc2420-> rssi;
	  
	  uart_write( g_uart, out_string, 20,0  );
	  if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	  temp = temp % 100;
	  if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	  temp = temp % 10;
	  uart_putchar(g_uart,(char)(temp + 48));
	  uart_putchar(g_uart,*enter);
	  }
	  #endif
	  //////////////////////////////////////////////////////////////
	  
	  
	  //receive using char streams
          #ifdef CHAR_STREAM
	  led_twinkle(LED_GREEN,ledPeriod);
	  

	  length = cc2420_rawread( g_cc2420,(char *)rx_frame, 0,0 );
	  
	  if(length > 11) {
	  ledPeriod = rx_frame[9];
	  uart_write( g_uart, (char *)rx_frame, length ,0  );
	  }
	  #endif
	  //////////////////////////////////////////////////////////////
        }
        #endif
        
	global_destroy();

	return 0;															
}