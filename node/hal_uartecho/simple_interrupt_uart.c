#include "../common/hal/hal_configall.h"
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_uart.h"

 






#define uchar unsigned char 
#define uint unsigned int 
 
uchar g_bTxdPos=0;  //���Ͷ�λ������ 
uchar g_bTxdLen=0;  //�ȴ������ֽ��� 
uchar g_bRxdPos=0;  //���ն�λ������ 
uchar g_bRxdLen=0;  //�ȴ������ֽ��� 
 
uchar g_aSendBuf[16];  //��������緳��� 
uchar g_aRecvBuf[16];  //�������ݻ����� 
 
 /*
//�����ж� 
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
 
//�����ж� 
void __attribute((signal))__vector_21(void);
void __vector_21(void)
{ 
 if(--g_bTxdLen>0) 
   UDR0=g_aSendBuf[++g_bTxdPos]; 
} 
*/
 
//�Ƿ������� 
uchar IsRecvComplete(void)
{ 
 return g_bRxdLen==0; 
} 
 
//�ӷ��ͻ���������ָ���������� 
void SendToUart(uchar size) 
{ 
 g_bTxdPos=0; 
 g_bTxdLen=size; 

  UDR0=g_aSendBuf[0]; 
 while(g_bTxdLen>0); 
} 
 
//����ָ���������ݵ����ջ����� 
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
 
    //uart ��ʼ�� 
 //����ʹ�ܡ�����ʹ�ܡ������ж����������ж����� 
  UCSR0B=(1<<RXCIE)|(1<<TXCIE)|(1<<RXEN)|(1<<TXEN);  
  UBRR0L=12;  // baud=9600   UBRR=CK/(baud*16) -1  
   sei();//���ж����� 
  
 while(1) 
 { 
   //�첽���� 16 �ֽ����� 
   RecvFromUart(16,0); 
 
   //�ȴ�������� 
   while(!IsRecvComplete()); 
    
   //�����յ������ݸ��Ƶ����ͻ����� 
   for(i=0;i<16;i++) 
   g_aSendBuf[i]=g_aRecvBuf[i]; 

g_bRxdPos = 0;
g_bRxdLen = 16;
    
   //���ͻؽ��յ������� 
   //SendToUart(16); 
 } 
 }
