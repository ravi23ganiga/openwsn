
#include "hal_watchdog.h"

/* m_instance_count: the count of instances of TWatchdog object.
 * This static variable controls only one TWatchdog object exists in the system.
 * When you want to create the second or more TWatchdog objects, the construct()
 * function will return NULL.
 */
static uint8 m_instance_count = 0;

TWatchdog * watchdog_construct( char * buf, uint16 size, uint32 interval )
{
	TWatchdog * wdg;
	
	if (m_instance_count == 0)
		wdg = (sizeof(TWatchdog) <= size) ? (TWatchdog*)buf : NULL;
	else
		wdg = NULL;
		
	if (wdg != NULL)
	{
		wdg->interval = interval;
		wdg->state = 1;
		m_instance_count ++;
	}
	
	return wdg;
}

void watchdog_destroy( TWatchdog * wdg )
{
	watchdog_disable( wdg );
	if (m_instance_count > 0)
		m_instance_count ++;
}

void watchdog_configure( TWatchdog * wdg, uint8 opt )
{
	return;
}

void watchdog_enable( TWatchdog * wdg )
{
	// @TODO
}

void watchdog_disable( TWatchdog * wdg )
{
	if (wdg->state == 1)
	{
		// @TODO
	}
}

void watchdog_feed( TWatchdog * wdg )
{
	// @TODO
}
