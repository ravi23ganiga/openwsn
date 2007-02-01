#include <stdlib.h>
#include "config.h"
#include "..\hal\hal_led.h"
#include "..\hal\hal_cc2420.h"
#include "..\hal\hal_cc2420rf.h"
#include "..\hal\hal_spi.h"
#include "..\hal\hal.h"
#include "start.h"
#include "location.h"

TCc2420Frame tx_packet;
TCc2420Frame rx_packet;           

//#define ANCHOR_NODE_1
//#define ANCHOR_NODE_2
//#define ANCHOR_NODE_3
#define LOCATION_REQUEST_NODE

#define LENGTH_UNIT 0.59 * 2      //0.59m

#define L_1_RSSI      244
#define L_2_RSSI      231
#define L_3_RSSI      227
#define L_4_RSSI      224
#define L_5_RSSI      222
#define L_6_RSSI      220
#define L_7_RSSI      219
#define L_8_RSSI      218
#define L_9_RSSI      215
#define L_10_RSSI     210
#define L_11_RSSI    1
#define L_12_RSSI    2
#define L_13_RSSI    3
#define L_14_RSSI    4
#define L_15_RSSI  5
#define L_16_RSSI   11
#define L_17_RSSI   6
#define L_18_RSSI   7
#define L_19_RSSI   8
#define L_20_RSSI    9
#define L_21_RSSI  10
#define L_22_RSSI   11 
#define L_23_RSSI    12 
#define L_24_RSSI    13 
#define L_25_RSSI  14
#define L_26_RSSI   15
#define L_27_RSSI   16
#define L_28_RSSI    17
#define L_29_RSSI    18
#define L_30_RSSI   19

#define L_1      1  * LENGTH_UNIT
#define L_2      2  * LENGTH_UNIT
#define L_3      3  * LENGTH_UNIT
#define L_4      4  * LENGTH_UNIT
#define L_5      5  * LENGTH_UNIT
#define L_6      6  * LENGTH_UNIT
#define L_7      7  * LENGTH_UNIT
#define L_8      8  * LENGTH_UNIT
#define L_9      9  * LENGTH_UNIT
#define L_10     10 * LENGTH_UNIT
#define L_11     11 * LENGTH_UNIT
#define L_12     12 * LENGTH_UNIT
#define L_13     13 * LENGTH_UNIT
#define L_14     14 * LENGTH_UNIT
#define L_15     15 * LENGTH_UNIT
#define L_16     16 * LENGTH_UNIT
#define L_17     17 * LENGTH_UNIT
#define L_18     18 * LENGTH_UNIT
#define L_19     19 * LENGTH_UNIT
#define L_20     20 * LENGTH_UNIT
#define L_21     21 * LENGTH_UNIT
#define L_22     22 * LENGTH_UNIT
#define L_23     23 * LENGTH_UNIT
#define L_24     24 * LENGTH_UNIT
#define L_25     25 * LENGTH_UNIT
#define L_26     26 * LENGTH_UNIT
#define L_27     27 * LENGTH_UNIT
#define L_28     28 * LENGTH_UNIT
#define L_29     29 * LENGTH_UNIT
#define L_30     30 * LENGTH_UNIT


int location_test()
{
    int8 length;
    uint8 n,temp;
    double anchor_1_x = 0,   anchor_1_y = 0;
    double anchor_2_x = 0,   anchor_2_y = 3.7;
    double anchor_3_x = 2.5, anchor_3_y = 3.5;
    double dis_1 = 0,dis_2 = 0,dis_3 = 0;
    double my_x,my_y;
    
    char * a1_string    = "signal from anchor node 1\n";
    char * a2_string    = "signal from anchor node 2\n";
    char * a3_string    = "signal from anchor node 3\n";
    char * rssi_string  = "the rssi value is : ";
    char * dis_string   = "the distance is : ";
    char * enter = "\n";
    char * space_string = "      ";
    char * location_string = "the node's location is: ";
    char * comma = ",";
    
    target_init();
    
    global_construct();
    spi_configure( g_spi );
    uart_configure( g_uart, 115200, 0, 0, 0, 0 );
    cc2420_configure( g_cc2420, CC2420_BASIC_INIT, 0, 0);
    cc2420_configure( g_cc2420, CC2420_CONFIG_PANID, g_cc2420->panid, 0);
    
        
    tx_packet.PanId = 0x2420;
    rx_packet.PanId = 0x2420;
    
    #ifdef LOCATION_REQUEST_NODE	
    tx_packet.srcAddr = 0x1000;
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, tx_packet.srcAddr, 0);
    for (n = 0; n < 2; n++) {
        tx_packet.Payload[n] = 8;   //88 represents localize request
    }
    #endif
    
    #ifdef ANCHOR_NODE_1	
    tx_packet.srcAddr = 0x1111;
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, tx_packet.srcAddr, 0);
    tx_packet.Payload[0] = 0;
    tx_packet.Payload[1] = 0;   //payload[0,1] represent anchor's x and y
    #endif	
    
    #ifdef ANCHOR_NODE_2	
    tx_packet.srcAddr = 0x2222;
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, tx_packet.srcAddr, 0);
    tx_packet.Payload[0] = 0;
    tx_packet.Payload[1] = 10;   //payload[0,1] represent anchor's x and y
    #endif	
    
    #ifdef ANCHOR_NODE_3	
    //tx_packet.destAddr = 0x1000;
    tx_packet.srcAddr = 0x3333;
    cc2420_configure( g_cc2420, CC2420_CONFIG_LOCALADDRESS, tx_packet.srcAddr, 0);
    tx_packet.Payload[0] = 10;
    tx_packet.Payload[1] = 0;   //payload[0,1] represent anchor's x and y
    #endif	


 
    cc2420_receive_on(g_cc2420);  
    IRQEnable(); 
    
    
    
    #ifdef LOCATION_REQUEST_NODE	
    while(1) 
    {
    	   
    	    
    	    //send request to anchor_1, if received infor of anchor_1, then do next
    	    tx_packet.destAddr = 0x1111;
    	    while(1)
    	    {
    	        led_twinkle(LED_RED,1);
    	        led_toggle(LED_GREEN);
    	        length = cc2420_writeframe( g_cc2420,tx_packet,2+11);
    	        if(length != -1) break;
    	    }
    	    
    	    while(1) 
    	    {
    	        led_twinkle(LED_GREEN,1);
    	        length = cc2420_readframe( g_cc2420,&rx_packet);
	        if(length > 11) 
	        {
	            uart_write(g_uart, a1_string,26, 0);
	            
	            //anchor_1_x     = rx_packet.Payload[0];
	            //anchor_1_y     = rx_packet.Payload[1];
	            
	            
	            rssi2distance(g_cc2420->rssi, &dis_1);
	            
	            //uart_putchar(g_uart,(char)anchor_1_x );
	            //uart_putchar(g_uart,(char)anchor_1_y );
	            
	            //uart_putchar(g_uart,(char)(rx_packet.srcAddr) );
	            //uart_putchar(g_uart,(char)(rx_packet.srcAddr >> 8) );
	            
	            uart_write(g_uart, rssi_string,20, 0);	            
	            temp = g_cc2420->rssi;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            
	            
	            uart_write(g_uart, dis_string,18, 0);
	            temp = (uint8)dis_1;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            uart_putchar(g_uart,(char)*enter );
	            break;
	        }
    	    }
    	    
    	    halWait(2000);
    	    
    	    //send request to anchor_2, if received infor of anchor_2, then do next
    	    tx_packet.destAddr = 0x2222;
    	    while(1)
    	    {
    	        led_twinkle(LED_RED,2);
    	        length = cc2420_writeframe( g_cc2420,tx_packet,2+11);
    	        if(length != -1) break;
    	    }
    	    
    	    while(1) 
    	    {
    	        led_twinkle(LED_GREEN,2);
    	        length = cc2420_readframe( g_cc2420,&rx_packet);
	        if(length > 11) 
	        {
	            uart_write(g_uart, a2_string,26, 0);
	            
	            //anchor_2_x     = rx_packet.Payload[0];
	            //anchor_2_y     = rx_packet.Payload[1];
	            
	            rssi2distance(g_cc2420->rssi, &dis_2);
	            
	            //uart_putchar(g_uart,(char)anchor_2_x );
	            //uart_putchar(g_uart,(char)anchor_2_y );
	            
	            //uart_putchar(g_uart,(char)(rx_packet.srcAddr) );
	            //uart_putchar(g_uart,(char)(rx_packet.srcAddr >> 8) );
	            
	            uart_write(g_uart, rssi_string,20, 0);	            
	            temp = g_cc2420->rssi;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            
	            
	            uart_write(g_uart, dis_string,18, 0);
	            temp = (uint8)dis_2;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            
	            uart_putchar(g_uart,(char)*enter );
	            uart_putchar(g_uart,(char)*enter );
	            break;
	        }
    	    }
    	    
    	    halWait(2000);
    	    
    	    //send request to anchor_3, if received infor of anchor_3, then do next
    	    tx_packet.destAddr = 0x3333;
    	    while(1)
    	    {
    	    	led_twinkle(LED_RED,3);
    	        length = cc2420_writeframe( g_cc2420,tx_packet,2+11);
    	        if(length != -1) break;
    	    }
    	    while(1) 
    	    {
    	        length = cc2420_readframe( g_cc2420,&rx_packet);
	        if(length > 11) 
	        {
	            uart_write(g_uart, a3_string,26, 0);
	            
	            //anchor_3_x     = rx_packet.Payload[0];
	            //anchor_3_y     = rx_packet.Payload[1];
	           
	            rssi2distance(g_cc2420->rssi, &dis_3);
	            
	            //uart_putchar(g_uart,(char)anchor_3_x );
	            //uart_putchar(g_uart,(char)anchor_3_y );
	            
	            //uart_putchar(g_uart,(char)(rx_packet.srcAddr) );
	            //uart_putchar(g_uart,(char)(rx_packet.srcAddr >> 8) );
	            
	            uart_write(g_uart, rssi_string,20, 0);	            
	            temp = g_cc2420->rssi;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            
	            
	            uart_write(g_uart, dis_string,18, 0);
	            temp = (uint8)dis_3;
	            if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	            temp = temp % 100;
	            if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	            temp = temp % 10;
	            uart_putchar(g_uart,(char)(temp + 48));
	            uart_putchar(g_uart,(char)*enter );
	            uart_putchar(g_uart,(char)*enter );
	            break;
	        }
    	    }
    	    
    	
    	    
    	    localize(&my_x,&my_y,anchor_1_x,anchor_1_y,dis_1,anchor_2_x,anchor_2_y,dis_2,anchor_3_x,anchor_3_y,dis_3);
    	    uart_write(g_uart, location_string,24, 0);
    	    
    	    temp = (uint8)my_x;
	    if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	    temp = temp % 100;
	    if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	    temp = temp % 10;
	    uart_putchar(g_uart,(char)(temp + 48));
    	    
    	    uart_putchar(g_uart,(char) (*comma));
    	    
    	    temp = (uint8)my_y;
	    if((temp / 100) > 0) uart_putchar(g_uart,(char)(temp / 100 + 48));
	    temp = temp % 100;
	    if((temp / 10) > 0) uart_putchar(g_uart,(char)(temp / 10 + 48));
	    temp = temp % 10;
	    uart_putchar(g_uart,(char)(temp + 48));
    	    
    	    uart_putchar(g_uart,(char)(*enter));
    	    uart_putchar(g_uart,(char)*enter );
    	    uart_putchar(g_uart,(char)(*enter));
    	    
    	    halWait(2000);
    }
    #endif
     
    #ifdef ANCHOR_NODE_1	
    while(1) 
    {

    	led_twinkle(LED_GREEN,1);
    	length = cc2420_readframe( g_cc2420,&rx_packet);
	if(length > 11) 
	//if(length != 0)
    	{
    	   if(rx_packet.Payload[0] == 8 && rx_packet.Payload[1] == 8)
    	   {
    	      	tx_packet.destAddr = 0x1000;//rx_packet.srcAddr;
    	      	while(1)
    	        {
    	            
    	            led_twinkle(LED_YELLOW,1);	
    	            length = cc2420_writeframe( g_cc2420,tx_packet,2+11);
    	            if(length != -1) break;
    	        }
    	   } 	
    	}
    }
    #endif
    
    #ifdef ANCHOR_NODE_2	
    while(1) 
    {
    	led_twinkle(LED_GREEN,1);
    	length = cc2420_readframe( g_cc2420,&rx_packet);
	if(length > 11) 
	//if(length != 0)
    	{
    	   if(rx_packet.Payload[0] == 8 && rx_packet.Payload[1] == 8)
    	   {
    	      	tx_packet.destAddr = 0x1000;//rx_packet.srcAddr;
    	      	while(1)
    	        {
    	            led_twinkle(LED_YELLOW,1);
    	            length = cc2420_writeframe( g_cc2420,tx_packet,2+11);
    	            if(length != -1) break;
    	        
    	        }
    	   } 	
    	}
    }
    #endif
    
    #ifdef ANCHOR_NODE_3	
    while(1) 
    {
    	led_twinkle(LED_GREEN,1);
    	length = cc2420_readframe( g_cc2420,&rx_packet);
	if(length > 11) 
    	//if(length != 0)
    	{
    	   if(rx_packet.Payload[0] == 8 && rx_packet.Payload[1] == 8)
    	   {
    	      	tx_packet.destAddr = 0x1000;//rx_packet.srcAddr;
    	      	while(1)
    	        {
    	            length = cc2420_writeframe( g_cc2420,tx_packet,2+11);
    	            if(length != -1) break;
    	        }
    	   } 	
    	}
    }
    #endif
}


void rssi2distance(uint8 rssi, double * dis)
{
    if((rssi >= L_1_RSSI) || (rssi < 30))  {*dis = L_1;     return;}
    if(rssi >= L_2_RSSI)                   {*dis = L_2 -  (rssi - L_2_RSSI)   / (L_3_RSSI - L_2_RSSI) * LENGTH_UNIT;   return;}
    if(rssi >= L_3_RSSI)                   {*dis = L_3 -  (rssi - L_3_RSSI)   / (L_4_RSSI - L_3_RSSI) * LENGTH_UNIT;   return;}
    if(rssi >= L_4_RSSI)                   {*dis = L_4 -  (rssi - L_4_RSSI)   / (L_5_RSSI - L_4_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_5_RSSI)                   {*dis = L_5 -  (rssi - L_5_RSSI)   / (L_6_RSSI - L_5_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_6_RSSI)                   {*dis = L_6 -  (rssi - L_6_RSSI)   / (L_7_RSSI - L_6_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_7_RSSI)                   {*dis = L_7 -  (rssi - L_7_RSSI)   / (L_8_RSSI - L_7_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_8_RSSI)                   {*dis = L_8 -  (rssi - L_8_RSSI)   / (L_9_RSSI - L_8_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_9_RSSI)                   {*dis = L_9 -  (rssi - L_9_RSSI)   / (L_10_RSSI - L_9_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_10_RSSI)                  {*dis = L_10 - (rssi - L_10_RSSI)  / (L_11_RSSI - L_10_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_11_RSSI)                  {*dis = L_11 - (rssi - L_11_RSSI)  / (L_12_RSSI - L_11_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_12_RSSI)                  {*dis = L_12 - (rssi - L_12_RSSI)  / (L_13_RSSI - L_12_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_13_RSSI)                  {*dis = L_13 - (rssi - L_13_RSSI)  / (L_14_RSSI - L_13_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_14_RSSI)                  {*dis = L_14 - (rssi - L_14_RSSI)  / (L_15_RSSI - L_14_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_15_RSSI)                  {*dis = L_15 - (rssi - L_15_RSSI)  / (L_16_RSSI - L_15_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_16_RSSI)                  {*dis = L_16 - (rssi - L_16_RSSI)  / (L_17_RSSI - L_16_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_17_RSSI)                  {*dis = L_17 - (rssi - L_17_RSSI)  / (L_18_RSSI - L_17_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_18_RSSI)                  {*dis = L_18 - (rssi - L_18_RSSI)  / (L_19_RSSI - L_18_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_19_RSSI)                  {*dis = L_19 - (rssi - L_19_RSSI)  / (L_20_RSSI - L_19_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_20_RSSI)                  {*dis = L_20 - (rssi - L_20_RSSI)  / (L_21_RSSI - L_20_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_21_RSSI)                  {*dis = L_21 - (rssi - L_21_RSSI)  / (L_22_RSSI - L_21_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_22_RSSI)                  {*dis = L_22 - (rssi - L_22_RSSI)  / (L_23_RSSI - L_22_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_23_RSSI)                  {*dis = L_23 - (rssi - L_23_RSSI)  / (L_24_RSSI - L_23_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_24_RSSI)                  {*dis = L_24 - (rssi - L_24_RSSI)  / (L_25_RSSI - L_24_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_25_RSSI)                  {*dis = L_25 - (rssi - L_25_RSSI)  / (L_26_RSSI - L_25_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_26_RSSI)                  {*dis = L_26 - (rssi - L_26_RSSI)  / (L_27_RSSI - L_26_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_27_RSSI)                  {*dis = L_27 - (rssi - L_27_RSSI)  / (L_28_RSSI - L_27_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_28_RSSI)                  {*dis = L_28 - (rssi - L_28_RSSI)  / (L_29_RSSI - L_28_RSSI) * LENGTH_UNIT;    return;}
    if(rssi >= L_29_RSSI)                  {*dis = L_29 - (rssi - L_29_RSSI)  / (L_30_RSSI - L_29_RSSI) * LENGTH_UNIT;    return;}
    *dis = 50;
    
}

void reverse_t(double *f)   //利用一维数组作为矩阵，求逆矩阵
{
        double a,b,c,d,e;
	a = *f;
	b = *(f + 1);
	c = *(f + 2);
	d = *(f + 3);
	e = a*d - b*c;
	*f = d/e;
	*(f + 1) = -1 * b /e;
	*(f + 2) = -1 * c /e;
	*(f + 3) = a/e;
}


void localize(double *x,double *y,double x1,double y1,double d1,double x2,double y2,double d2,double x3,double y3,double d3)
{
        double c2[4],dis2[2];
	c2[0] = 2 * (x1 - x2);
	c2[1] = 2 * (y1 - y2);
	c2[2] = 2 * (x2 - x3);
	c2[3] = 2 * (y2 - y3);
	dis2[0] = d2 * d2 - d1 * d1 -x2 * x2 -y2 * y2 + x1 * x1 + y1 * y1;
	dis2[1] = d3 * d3 - d2 * d2 -x3 * x3 -y3 * y3 + x2 * x2 + y2 * y2;
        compute_location(x,y,c2,dis2);
}


void compute_location(double *x,double *y,double *c2,double *d2)
{
        reverse_t(c2);
	*x =  c2[0] * d2[0] + c2[1] * d2[1];
	*y =  c2[2] * d2[0] + c2[3] * d2[1];
}
