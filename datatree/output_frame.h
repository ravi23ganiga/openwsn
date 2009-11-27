#ifndef _OUTPUT_FRAME_H_4327_
#define _OUTPUT_FRAME_H_4327_

#include "../common/rtl/rtl_openframe.h"
#include "../common/hal/hal_uart.h"

#undef  CONFIG_ASCII_OUTPUT
#define CONFIG_ASCII_OUTPUT

void output_openframe( TiOpenFrame * opf, TiUartAdapter * uart );

#endif
