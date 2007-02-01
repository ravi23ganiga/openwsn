
#include "hal_foundation.h"
#include "hal_led.h"
#include "hal_cpu.h"
/*
void led( uint8 idx, uint8 state )
{
	
}
*/
void led_off( uint8 idx )
{
	switch(idx)
	{
	    case LED_GREEN:    
	         #if LED_GREEN_PORT == 0 
	         IO0SET  = BM(LED_GREEN_PIN);    break;
	         #endif
	         
	         #if LED_GREEN_PORT == 1 
	         IO1SET  = BM(LED_GREEN_PIN);    break;
	         #endif
	         
	    case LED_RED:    
	         #if LED_RED_PORT == 0 
	         IO0SET  = BM(LED_RED_PIN);      break;
	         #endif
	         
	         #if LED_RED_PORT == 1 
	         IO1SET  = BM(LED_RED_PIN);      break;
	         #endif
	         
	    case LED_YELLOW:   
	         #if LED_YELLOW_PORT == 0 
	         IO0SET  = BM(LED_YELLOW_PIN);   break;
	         #endif
	         
	         #if LED_YELLOW_PORT == 1 
	         IO1SET  = BM(LED_YELLOW_PIN);   break;
	         #endif
	         
	}
}

void led_on( uint8 idx )
{
	switch(idx)
	{
	    case LED_GREEN:    
	         #if LED_GREEN_PORT == 0 
	         IO0CLR  = BM(LED_GREEN_PIN);   break;
	         #endif
	         
	         #if LED_GREEN_PORT == 1 
	         IO1CLR  = BM(LED_GREEN_PIN);   break;
	         #endif
	         
	    case LED_RED:    
	         #if LED_RED_PORT == 0 
	         IO0CLR  = BM(LED_RED_PIN);     break;
	         #endif
	         
	         #if LED_RED_PORT == 1 
	         IO1CLR  = BM(LED_RED_PIN);     break;
	         #endif
	         
	    case LED_YELLOW:   
	         #if LED_YELLOW_PORT == 0 
	         IO0CLR  = BM(LED_YELLOW_PIN);   break;
	         #endif
	         
	         #if LED_YELLOW_PORT == 1 
	         IO1CLR  = BM(LED_YELLOW_PIN);   break;
	         #endif
	         
	}
}

void led_toggle( uint8 idx )
{
	switch(idx)
	{
	    case LED_GREEN:    
	         {
	         	#if LED_GREEN_PORT == 0
	         	if(IO0SET & BM(LED_GREEN_PIN))  IO0CLR  = BM(LED_GREEN_PIN); 
	         	else                            IO0SET  = BM(LED_GREEN_PIN);   	     
	         	break;
	         	#endif
	         	
	         	#if LED_GREEN_PORT == 1
	         	if(IO1SET & BM(LED_GREEN_PIN))  IO1CLR  = BM(LED_GREEN_PIN); 
	         	else                            IO1SET  = BM(LED_GREEN_PIN);   	     
	         	break;
	         	#endif
	         }
	    case LED_RED:      
	         {
	         	#if LED_RED_PORT == 0
	         	if(IO0SET & BM(LED_RED_PIN))  IO0CLR  = BM(LED_RED_PIN); 
	         	else                          IO0SET  = BM(LED_RED_PIN);   	     
	         	break;
	         	#endif
	         	
	         	#if LED_RED_PORT == 1
	         	if(IO1SET & BM(LED_RED_PIN))  IO1CLR  = BM(LED_RED_PIN); 
	         	else                          IO1SET  = BM(LED_RED_PIN);   	     
	         	break;
	         	#endif
	         }
	    case LED_YELLOW:   
	         {
	         	#if LED_YELLOW_PORT == 0
	         	if(IO0SET & BM(LED_YELLOW_PIN))  IO0CLR  = BM(LED_YELLOW_PIN); 
	         	else                             IO0SET  = BM(LED_YELLOW_PIN);   	     
	         	break;
	         	#endif
	         	
	         	#if LED_YELLOW_PORT == 1
	         	if(IO1SET & BM(LED_YELLOW_PIN))  IO1CLR  = BM(LED_YELLOW_PIN); 
	         	else                             IO1SET  = BM(LED_YELLOW_PIN);   	     
	         	break;
	         	#endif
	         }
	}
}

void led_twinkle( uint8 idx ,uint16 delay)
{
  	switch(idx)
	{
	    case LED_GREEN:
	         {
	         	led_toggle(LED_GREEN);
	                halWait(200*delay);	
	                led_toggle(LED_GREEN);
	                halWait(200*delay);
	                break;
	         }
	    case LED_RED:
	         {
	         	led_toggle(LED_RED);
	                halWait(200*delay);	
	                led_toggle(LED_RED);
	                halWait(200*delay);
	                break;
	         }
	    case LED_YELLOW:
	         {
	         	led_toggle(LED_YELLOW);
	                halWait(200*delay);	
	                led_toggle(LED_YELLOW);
	                halWait(200*delay);
	                break;
	         }	
	}
}


