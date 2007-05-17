/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available

 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#include <string.h>
#include "foundation.h"
#include ".\hal\hal.h"
#include "global.h"
#include "start.h"

/*****************************************************************************
 * @author zhangwei on 2006-07-25
 *	test cases
 * 
 * @modified by zhangwei on 2006-09-06
 *	add support to ActionScheduler
 * 
 ****************************************************************************/ 

#define TESTCASE_NULL
#undef TESTCASE_NULL

#define TESTCASE_DEBUGIO
//#undef  TESTCASE_DEBUGIO

#define TESTCASE_SIOECHO
#undef  TESTCASE_SIOECHO

#define TESTCASE_UARTECHO
#undef  TESTCASE_UARTECHO

#define TESTCASE_UARTANDDEBUG
#undef  TESTCASE_UARTANDDEBUG

#define TESTCASE_CC2420ECHO
#undef  TESTCASE_CC2420ECHO

#define TESTCASE_WLSECHO
#undef  TESTCASE_WLSECHO

#define TESTCASE_SNIFFER
#undef  TESTCASE_SNIFFER

#define TESTCASE_SIMPLEROUTER
#undef  TESTCASE_SIMPLEROUTER

#define TESTCASE_ANALYZER
#undef  TESTCASE_ANALYZER

#define TESTCASE_TIMERLEDINQUIRE
#undef  TESTCASE_TIMERLEDINQUIRE

#define TESTCASE_TIMERLEDINTERUPT
#undef  TESTCASE_TIMERLEDINTERUPT

#define TESTCASE_TIMER
#undef  TESTCASE_TIMER

#define TESTCASE_ACTSCHE
#undef  TESTCASE_ACTSCHE

#define TESTCASE_CC2420TX
#undef  TESTCASE_CC2420TX

#define TESTCASE_CC2420RX
#undef  TESTCASE_CC2420RX

#define TESTCASE_CC2420DEV
#undef  TESTCASE_CC2420DEV

#define TESTCASE_OPENMAC
#undef  TESTCASE_OPENMAC


#define TESTCASE_LOCATION
#undef  TESTCASE_LOCATION

#define TESTCASE_UNIQUEID
#undef  TESTCASE_UNIQUEID

#define TESTCASE_SENSOR
#undef  TESTCASE_SENSOR

#define TESTCASE_WLSMODEM
#undef  TESTCASE_WLSMODEM

#ifdef TESTCASE_NULL
  #undef TESTCASE_DEBUGUART
  #undef TESTCASE_SIOECHO
  #undef TESTCASE_UARTECHO
  #undef TESTCASE_CC2420ECHO
  #undef TESTCASE_WLSECHO
  #undef TESTCASE_SNIFFER
  #undef TESTCASE_SIMPLEROUTER
  #undef TESTCASE_ANALYZER
  #undef TESTCASE_TIMER
  #undef TESTCASE_UCOSTEST
  #undef TESTCASE_WLSMODEM
#endif


//----------------------------------------------------------------------------
// 测试程序入口点
// 建议按照下面的顺序进行逐项测试，我特地按照调试的难易和关联程序排列了一下
// 其用哪一项测试，是通过上面的MACRO控制的。
//----------------------------------------------------------------------------

void app_start()
{
	#ifdef TESTCASE_NULL
	app_run();
	#endif

	#ifdef TESTCASE_DEBUGIO
	debugio_test();
	#endif
	
	#ifdef TESTCASE_UARTANDDEBUG
	uartanddebug_run();
	#endif
	
	#ifdef TESTCASE_SIOECHO
	siocomm_test();
	#endif
	
	#ifdef TESTCASE_UARTECHO
	uartecho_run();
	#endif
	
	#ifdef TESTCASE_CC2420ECHO
	cc2420_test();
	#endif
	
	#ifdef TESTCASE_WLSECHO
	wls_test();
	#endif
	
	#ifdef TESTCASE_SNIFFER
	sniffer_run();
	#endif
	
	#ifdef TESTCASE_SIMPLEROUTER
	splrouter_run();
	#endif
	
	#ifdef TESTCASE_ANALYZER
	analyzer_run();
	#endif
	
	#ifdef TESTCASE_TIMER
	timer_test();
	#endif
	
	#ifdef TESTCASE_TIMERLEDINQUIRE
	timerled_test();
	#endif
	
	#ifdef TESTCASE_TIMERLEDINTERUPT
	timerupt_test();
	#endif
	
	#ifdef TESTCASE_ACTSCHE
	// @TODO: you should test each case separately.
	// each case represent a independent programming pattern of ActionScheduler.
	actsche_test(0);
	actsche_test(1);
	actsche_test(2);
	#endif
	
	#ifdef TESTCASE_CC2420TX
	cc2420tx_test();
	#endif
	
	#ifdef TESTCASE_CC2420RX
	cc2420rx_test();
	#endif
	
	#ifdef TESTCASE_OPENMAC
	openmac_run();
	#endif
	
	#ifdef TESTCASE_CC2420DEV
	cc2420dev_test();
	#endif
	
	#ifdef TESTCASE_LOCATION
	location_test();
	#endif
	
	#ifdef TESTCASE_UNIQUEID
	uniqueid_run();
	#endif
	
	#ifdef TESTCASE_SENSOR
	sensor_test();
	#endif
	
	#ifdef TESTCASE_WLSMODEM
	wlsmodem_start();
	#endif
}
