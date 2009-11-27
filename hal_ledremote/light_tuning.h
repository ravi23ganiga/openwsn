
#include "../common/hal/hal_configall.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_interrupt.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_target.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_timer.h"


int light_tune_init(void);
int light_tune_by_luminance( uint16 lum );
