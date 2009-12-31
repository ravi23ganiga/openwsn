
#include "hal_configall.h"
//#include <avr/io.h>
#include "hal_foundation.h"
#include "hal_target.h"
#include "hal_cpu.h"
#include "hal_led.h"	

static uint8 m_ledstate = 0x00;

void led_open()
{
	m_ledstate = 0x00;
    
	// HAL_MAKE_RED_LED_OUTPUT();
    // HAL_MAKE_YELLOW_LED_OUTPUT();
    // HAL_MAKE_GREEN_LED_OUTPUT();
    // HAL_SET_RED_LED_PIN();
    // HAL_SET_YELLOW_LED_PIN();
    // HAL_SET_GREEN_LED_PIN();
    
	// for ICT GAINS/GAINZ platform
	// set PA0, PA1, and PA2 bit of PORTA as output pin. the initial state of these
	// pins are LEDs on. 

	DDRA |= (_BV(PA0));
	DDRA |= (_BV(PA1));
	DDRA |= (_BV(PA2));  

	// PORTA &= ~(_BV(PA0)); 
	// PORTA &= ~(_BV(PA1)); 
	// PORTA &= ~(_BV(PA2));

    led_off( LED_ALL );
}

void led_close()
{
    led_off( LED_ALL );
}

void led( uint8 id, bool state )
{
	if (state)
		led_off( id );
	else
		led_on( id );
}

void led_off( uint8 id )
{
	if (id & LED_RED)
	{
     	PORTA|=_BV(PA2);
		m_ledstate |= LED_RED;		
	}
	if (id & LED_GREEN)
	{
     	PORTA|=_BV(PA1);
		m_ledstate |= LED_GREEN;		
	}
	if (id & LED_YELLOW)
	{
        PORTA|=_BV(PA0);
		m_ledstate |= LED_YELLOW;		
	}
}

void led_on( uint8 id )
{
	if (id & LED_RED)
	{
      	PORTA&=~_BV(PA2);
		m_ledstate &= ~LED_RED;		
	}
	if (id & LED_GREEN)
	{
      	PORTA&=~_BV(PA1);
		m_ledstate &= ~LED_GREEN;		
	}
	if (id & LED_YELLOW)
	{
      	PORTA&=~_BV(PA0);
		m_ledstate &= ~LED_YELLOW;		
	}
}

void led_toggle( uint8 id )
{
	if (id & LED_RED)
	{
		if (m_ledstate & LED_RED)
		{
      		PORTA&=~_BV(PA2);
			m_ledstate &= ~LED_RED;		
		}
		else{
      		PORTA|=_BV(PA2);
			m_ledstate |= LED_RED;		
		}
	}
	if (id & LED_GREEN)
	{
		if (m_ledstate & LED_GREEN)
		{
      		PORTA&=~_BV(PA1);
			m_ledstate &= ~LED_GREEN;		
		}
		else{
      		PORTA|=_BV(PA1);
			m_ledstate |= LED_GREEN;		
		}
	}
	if (id & LED_YELLOW)
	{
		if (m_ledstate & LED_YELLOW)
		{
      		PORTA&=~_BV(PA0);
			m_ledstate &= ~LED_YELLOW;		
		}
		else{
      		PORTA|=_BV(PA0);
			m_ledstate |= LED_YELLOW;		
		}
	}
}

void led_twinkle( uint8 id , uint16 interval, uintx count )
{
    if (count == 0)
        count = ~count;

    led_off( LED_ALL );
	while (count > 0)
	{
		led_toggle( id );
		hal_delay( interval );
        count --;
	}
}
