#ifndef _HAL_WDT_H_8390_
#define _HAL_WDT_H_8390_

/* compiled with WinAVR (avr gcc)
 *
 * reference
 * - atmega 128 datasheet
 * - A GNU Development Environment for the AVR Microcontroller on WDT, 
 *		http://users.rcn.com/rneswold/avr/x1803.html
 *
 * state
 *	ok.
 *
 * @author zhangwei on 20090520
 * @modified by zhangwei on 20090710
 */

#include <avr/wdt.h>

#undef TEST_WATCHDOG

/* Watchdog Prescaler
 */
/*
enum {
  WDT_PERIOD16		= 0x00, // 47ms
  WDT_PERIOD32		= 0x01, // 94ms
  WDT_PERIOD64		= 0x02, // 0.19s
  WDT_PERIOD128		= 0x03, // 0.38s
  WDT_PERIOD256		= 0x04, // 0.75s
  WDT_PERIOD512		= 0x05, // 1.5s
  WDT_PERIOD1024	= 0x06, // 3.0s
  WDT_PERIOD2048	= 0x07  // 6.0s
};
*/

/* predefined constant symbols:
 *	WDTO_15MS
 *	WDTO_30MS
 *	WDTO_60MS
 *	WDTO_120MS
 *	WDTO_250MS
 *	WDTO_500MS
 *	WDTO_1S
 *	WDTO_2S
 * 
 * the following three functions are declared in <wdt.h> provided by WinAVR. so 
 * there's no implementation in this module.
 *
 * wdt_disable
 * Disables the watchdog timer
 *
 * wdt_enable
 * Enables the watchdog timer. The passed value, timeout, is loaded in the watchdog control register.
 *
 * wdt_reset
 * Resets the watchdog timer. This function generates a single wdr instruction. 
 * Your application must guarantee that this function is called sooner than the 
 * timeout rate of the watchdog. Otherwise the processor will reset.
 */

/* attention:
 *	If you include <avr/wdt.h>, it has already provides the following three functions:
 *		wdt_reset()
 *		wdt_disable()
 *		wdt_enable(timeout)
 *  However, if you port openwsn to other platforms, you're may need to implement them.
 *  Uncomment the following three functions on other platforms please.
 */

/*
void wdt_reset();
void wdt_enable( uintx timeout );
void wdt_disable();
*/

#ifdef TEST_WATCHDOG
void wdt_test()
{
	wdt_disable();
	wdt_enable(100);
	wdt_reset();
}
#endif

#endif
