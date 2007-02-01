
#include "hal_foundation.h"
//#include <assert.h>
#include "hal_sensor_vibration.h"
#include "hal_mcp6s.h"
#include "hal_ad.h"
#include "hal_sensor_switch.h"
#include "hal_uart.h"
#include "hal_global.h"

TVibrationSensor * vib_construct( uint8 id, char * buf, uint8 size, TMcp6s26 * mcp, TAdConversion * ad  )
{
	TVibrationSensor * vib;
	char* out_string = "vibration consturct succesful!\n";
	
	if ( sizeof(TVibrationSensor) > size )
	    vib = NULL;
	else
	    vib = (TVibrationSensor *)buf;
	    
	if (vib != NULL)
	{
	    memset( (char*)vib, 0x00, sizeof(TVibrationSensor) );
	    vib->id = id;
	    vib->mcp = mcp;
	    vib->ad = ad;
            uart_write(g_uart, out_string,31, 0);
        }

	return vib;
	
}

void vib_destroy( TVibrationSensor * vib )
{
	NULL;
}

uint8 vib_read( TVibrationSensor * vib, char * buf, uint8 size, uint8 opt )
{
        int8 i; 
        uint16 value;

	avccs_on();

	for(i=0; i<4; i++)
   	{
    	    mcp6s26_setchannel( vib->mcp, i);
	    value = ad_read( vib->ad,NULL, 2, opt );
        }
    *buf = (uint8)value;
    *(buf + 1) = (uint8) (value >> 8);
    //avccs_off();
    return 8;
}

void vib_config(uint8 ifsleep)
{
  switch(ifsleep)
  {
      case SLEEP  :
      	   #if     VIBRATION_SLEEP_PORT == 0    
           IO0CLR = BM(VIBRATION_SLEEP_PIN);  break;
           #endif
    
           #if     VIBRATION_SLEEP_PORT == 1             
           IO1CLR = BM(VIBRATION_SLEEP_PIN);  break;
           #endif
           
      case WAKE   :
      	   #if     VIBRATION_SLEEP_PORT == 0    
           IO0SET = BM(VIBRATION_SLEEP_PIN);  break;
           #endif
    
           #if     VIBRATION_SLEEP_PORT == 1             
           IO1SET = BM(VIBRATION_SLEEP_PIN);  break;
           #endif
           
      default: 	
           #if     VIBRATION_SLEEP_PORT == 0    
           IO0SET = BM(VIBRATION_SLEEP_PIN);  break;
           #endif
    
           #if     VIBRATION_SLEEP_PORT == 1             
           IO1SET = BM(VIBRATION_SLEEP_PIN);  break;
           #endif
  }	
}



