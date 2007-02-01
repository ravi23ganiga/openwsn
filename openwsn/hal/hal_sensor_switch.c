#include "hal_foundation.h"
#include "hal_sensor_switch.h"


void avccs_on(void)
{
    #if AVCCS_SWITCH_PORT == 0 
    IO0SET  = BM(AVCCS_SWITCH_PIN);   
    #endif
	         
    #if AVCCS_SWITCH_PORT == 1 
    IO1SET  = BM(AVCCS_SWITCH_PIN);   
    #endif	
}

void avccs_off(void)
{
    #if AVCCS_SWITCH_PORT == 0 
    IO0CLR  = BM(AVCCS_SWITCH_PIN);  
    #endif
	         
    #if AVCCS_SWITCH_PORT == 1 
    IO1CLR  = BM(AVCCS_SWITCH_PIN);    
    #endif	
}