
#include <string.h>
#include "../common/hal/hal_configall.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"


static void debugio_test( void );

int main(void)
{
    debugio_test();
    return 0;
}

void debugio_test( void )
{
	char * msg = "debugio_test output! \r\n";

	target_init();

	led_open();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );

	dbo_open(0, 38400);
	dbo_write( msg, strlen(msg) );
    while (1) {};
}
