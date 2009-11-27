/******************************************************************************
 * osxevent
 * this module tests the events queue in "osx". the timer hardware and UART's receiveing 
 * part will periodically generate events and put them into the queue. attention
 * the generating process is inside the interrupt service routine(ISR), while the 
 * event processing is outside of the ISR.
 *
 * @author by zhangwei on 20070910
 * @modified by zhangwei on 20090704
 *
 *****************************************************************************/

#include "../common/hal/hal_configall.h"
#include <stdio.h>
#include <stdlib.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_timer.h"
#include "../common/osx/osx_foundation.h"
#include "../common/osx/osx_kernel.h"

TiTimerAdapter		g_timer;
TiUartAdapter		g_uart;

static void _osx_event_listener(void * object, struct _TiHalEvent * e);

int main(void)
{
	// hal_init( _hal_listener, NULL );
    hal_init( NULL, NULL );
	target_init();

	// add an event listener to the hal layer. if there's a event raised by the hal 
	// layer, then it will be routed to the osx kernel and drive the kernel to do something.
	hal_setlistener( osx_post );

	// set the map between event id and event listener
	osx_attach( 0, _osx_event_listener, NULL );

	hal_enable_interrupts();
	while (1) {};
	return 0;
}

void _osx_event_listener(void * object, struct _TiHalEvent * e)
{
	led_toggle( LED_ALL );
}


/*


// osx test
// osx is an component based, lightweight operating system kernel. 
// this program tests the event and runnable service support of osx. 

TiRunService1 g_svc1
TiRunService2 g_svc2

int main()
{
	construct
	open;

	// configure the connections among services
	svc2_setlistener( svc2, (TiFunEventHandler)svc3_evolve, (void *)svc3 );

	// add an event listener to the hal layer. if there's a event raised by the hal 
	// layer, then it will be routed to the osx kernel and drive the kernel to do something.
	hal_setlistener( osx_post );

	// set the map between event id and event listener
	osx_attach( 1, svc1_evolve, svc1 );
	osx_attach( 2, svc1_evolve, svc1 );
	osx_attach( 3, svc2_evolve, svc2 );
	osx_attach( 0, svc2_evolve, svc2 );

	hal_enable_interrupts();

    // already support sleep/wakeup because the sleep/wakeup handler have been 
    // registered by the osx itself.

	while (1) { osx_evolve() };
	// or fired by timer interrupt to drive the osx kernel running

	close()
	destroy()
}
*/