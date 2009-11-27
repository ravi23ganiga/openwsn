#include "../common/hal/hal_configall.h"
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_uart.h"

 






#define uchar unsigned char 
#define uint unsigned int 
 
uchar g_bTxdPos=0;  //发送定位计数器 
uchar g_bTxdLen=0;  //等待发送字节数 
uchar g_bRxdPos=0;  //接收定位计数器 
uchar g_bRxdLen=0;  //等待接收字节数 
 
uchar g_aSendBuf[16];  //发送数据绶冲区 
uchar g_aRecvBuf[16];  //接收数据缓冲区 
 
 /*
//接收中断 
void __attribute((signal))__vector_19(void);
void __vector_19(void)
{ 
 uchar c=UDR0;  
 if(g_bRxdLen>0) 
 { 
   g_aRecvBuf[g_bRxdPos++]=c; 
   g_bRxdLen--; 
 } 
} 
 
//发送中断 
void __attribute((signal))__vector_21(void);
void __vector_21(void)
{ 
 if(--g_bTxdLen>0) 
   UDR0=g_aSendBuf[++g_bTxdPos]; 
} 
*/
 
//是否接收完成 
uchar IsRecvComplete(void)
{ 
 return g_bRxdLen==0; 
} 
 
//从发送缓冲区发送指定长度数据 
void SendToUart(uchar size) 
{ 
 g_bTxdPos=0; 
 g_bTxdLen=size; 

  UDR0=g_aSendBuf[0]; 
 while(g_bTxdLen>0); 
} 
 
//接收指定长度数据到接收缓冲区 
void RecvFromUart(uchar size,uchar bwait) 
{ 
 g_bRxdPos=0; 
 g_bRxdLen=size; 
 if(bwait) 
   while(g_bRxdLen>0); 
} 
 
int  main( void ) 
{ 
 
 HAL_SET_PIN_DIRECTIONS();
	led_init();
	led_on( LED_GREEN );

	led_on( LED_RED );
	led_on( LED_YELLOW );
 
 
 uchar i; 
 
    //uart 初始化 
 //接收使能、发送使能、接收中断允许、发送中断允许 
  UCSR0B=(1<<RXCIE)|(1<<TXCIE)|(1<<RXEN)|(1<<TXEN);  
  UBRR0L=12;  // baud=9600   UBRR=CK/(baud*16) -1  
   sei();//总中断允许 
  
 while(1) 
 { 
   //异步接收 16 字节数据 
   RecvFromUart(16,0); 
 
   //等待接收完成 
   while(!IsRecvComplete()); 
    
   //将接收到的数据复制到发送缓冲区 
   for(i=0;i<16;i++) 
   g_aSendBuf[i]=g_aRecvBuf[i]; 

g_bRxdPos = 0;
g_bRxdLen = 16;
    
   //发送回接收到的数据 
   //SendToUart(16); 
 } 
 }
