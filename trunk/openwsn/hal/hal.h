
#ifndef _HAL_H_2998_
#define _HAL_H_2998_

/******************************************************************************
 * @author zhangwei on 2006-08-30
 * hal is the summary header file of all HAL header files to faciliate using
 * in other modules. attention that all the "*.h" file in the "hal" directory  
 * should NOT include "hal.h".
 * 
 * @attention
 * HAL中的对象及其方法要保持最大程度的简明．每一个接口变量和函数，能不增加就不增加．这样做
 * 是为了最大限度简化HAL在不同硬件平台上的移植．
 * 
 * @modified by zhangwei on 20061013
 * made great revision today. 
 * this is just the style of future hal.h. you should only include other hal header
 * files here! so other modules in the application can include "hal.h" only. 
 *****************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cpu.h"
#include "hal_interrupt.h"
#include "hal_target.h"
#include "hal_boot.h"
#include "hal_spi.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_cc2420.h"
#include "hal_flash.h"
#include "hal_eeprom.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_watchdog.h"
#include "hal_global.h"
#include "hal_mcp6s.h"
#include "hal_ad.h"
#include "hal_sensor_strain.h"
#include "hal_sensor_vibration.h"
#include "hal_uniqueid.h"
#include "hal_sensor_switch.h"

#endif
