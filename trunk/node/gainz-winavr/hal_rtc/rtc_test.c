/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
#include "../common/hal/hal_configall.h"
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_timer.h"
#include "../common/hal/hal_debugio.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_assert.h"
#include "hal_rtc.h"

static TiRtc g_rtc;

int main()
{
	target_init();
	HAL_SET_PIN_DIRECTIONS();
	wdt_disable();
	led_on( LED_ALL );
	hal_delay( 500 );
	led_off( LED_ALL );
	dbo_open(0, 38400);


	TiRtc * rtc;
	rtc = rtc_construct( (void *)&g_rtc, sizeof(g_rtc) );
	rtc = rtc_open( rtc, NULL, NULL, 0x00 );
	rtc_start( rtc );

	while(1)
	{}
	
	// if not use listener 
	while (!rtc_expired(rtc))
	{
		//do something;
	}

	// rtc_expired now

}
