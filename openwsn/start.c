//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
//	test cases
// 
// @modified by zhangwei on 2006-09-06
//	add support to ActionScheduler
//----------------------------------------------------------------------------

#include <string.h>
#include "foundation.h"
#include ".\hal\hal.h"
#include "global.h"
#include "start.h"

#define TESTCASE_NULL
#undef TESTCASE_NULL

#define TESTCASE_DEBUGUART
#undef  TESTCASE_DEBUGUART

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

#define TESTCASE_TIMER
#undef  TESTCASE_TIMER

#define TESTCASE_ACTSCHE
#undef  TESTCASE_ACTSCHE

#define TESTCASE_CC2420DEV
#undef  TESTCASE_CC2420DEV

#define TESTCASE_LOCATION
//#undef  TESTCASE_LOCATION

#define TESTCASE_uniqueid
#undef  TESTCASE_uniqueid

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

	#ifdef TESTCASE_DEBUGUART
	debuguart_test();
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
	
	#ifdef TESTCASE_ACTSCHE
	// @TODO: you should test each case separately.
	// each case represent a independent programming pattern of ActionScheduler.
	actsche_test(0);
	actsche_test(1);
	actsche_test(2);
	#endif
	
	#ifdef TESTCASE_CC2420DEV
	cc2420dev_test();
	#endif
	
	#ifdef TESTCASE_LOCATION
	location_test();
	//location_test_ago();
	#endif
	
	#ifdef TESTCASE_uniqueid
	uniqueid_run();
	#endif
	
	#ifdef TESTCASE_SENSOR
	sensor_test();
	#endif
	
	#ifdef TESTCASE_WLSMODEM
	wlsmodem_start();
	#endif
}
