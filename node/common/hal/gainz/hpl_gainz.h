#ifndef _HPL_GAINZ_H_4874_
#define _HPL_GAINZ_H_4874_

/* hpl_gainz.h
 * this module contains the declarations specific to ICT's GAINZ platform/target board 
 */

#include "hpl_atmega128.h"

#define CONFIG_GAINZ_CLOCK_FREQUENCY 8000000UL
#define CONFIG_GAINZ_CLOCK_FREQUENCY_KHZ (CONFIG_GAINZ_CLOCK_FREQUENCY/1000)
#define CONFIG_GAINZ_WIRELESS_FREQUENCY 2400

#ifndef F_CPU
#define F_CPU CONFIG_GAINZ_CLOCK_FREQUENCY
#endif

#define CONFIG_SYSTEM_CLOCK CONFIG_GAINZ_CLOCK_FREQUENCY

#include <util/delay.h>


#endif

