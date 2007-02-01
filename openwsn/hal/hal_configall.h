#ifndef _HAL_CONFIGALL_H
#define _HAL_CONFIGALL_H

// This is the config file of HAL. It should be as simple as possible. 
// Only the configuration macros should be defined here.

#include "..\configall.h"
#include "..\src\config.h"

// If you use uCOS-II in your system, you should define the following macro
// this will enable some hal_xxx functions bases on uCOS implementation!

#undef HAL_USE_UCOSII
#define HAL_USE_UCOSII

#define UART_READ_ENABLE 1
#define UART_WRITE_ENABLE 1

#endif