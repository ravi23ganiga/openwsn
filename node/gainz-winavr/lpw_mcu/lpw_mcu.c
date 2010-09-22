#include "../../common/openwsn/hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/sleep.h>
#include "../../common/openwsn/svc/svc_foundation.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/hal/hal_cpu.h"
#include "../../common/openwsn/hal/hal_led.h"
#include "../../common/openwsn/hal/hal_targetboard.h"

/*********************************************************************************/

int main(void)
{ 

	target_init();
	led_open();
	led_on( LED_ALL );
	hal_delay( 1000 );
	led_off( LED_ALL );
    
	ACSR=(1<<ACD);//ģ��Ƚ����ر�

	ADCSRA=(0<<ADEN);//ADC�ر�

	WDTCR=(1<<WDCE)|(1<<WDE);
	WDTCR=0x00;//�ر�WDT

    MCUCSR=(1<<JTD);//��JTAG�ӿ���Ƭ�ڵ���ϵͳ*

/*��˿λ��λ����connect��fuseѡ��ﲻѡBODEN����Ĭ��ֵΪ1��
  �������������*/

	while(1)
	{
	  set_sleep_mode(SLEEP_MODE_EXT_STANDBY);//��չSTANDBYYģʽ
	  sleep_enable();
	  sleep_cpu();
	  sleep_disable();
	}

}    

/*****************************************************************
����˯��ģʽ��
   �������ģʽ
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    
    ����ADC ��������ģʽ
    set_sleep_mode(SLEEP_MODE_ADC);
    sleep_enable();
    sleep_cpu();
    sleep_disable();

    �������ģʽ
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    
	����ʡ��ģʽ
    set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    
	����STANDBYģʽ
    set_sleep_mode(SLEEP_MODE_STANDBY);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
    
	������չSTANDBYģʽ
    set_sleep_mode(SLEEP_MODE_EXT_STANDBY);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
	*/
