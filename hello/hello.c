#include "../common/hal/hal_configall.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"

/* @todo
 * hello.c should use TiUartAdapter instead of "dbo_"
 */

int main(void)
{
	char * msg = "hello! \r\n";
    uint8 i;

	target_init();
	dbo_open(0, 38400);

    for (i=0; i<9; i++)
        dbo_putchar( msg[i] );
	dbo_write( msg, 9 );
    while (1) {};

    return 0;
} 
