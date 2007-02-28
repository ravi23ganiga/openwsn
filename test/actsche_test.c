

#include "..\foundation.h"
#include "..\hal\hal.h"
#include "..\service\svc.h"
#include "actsche_test.h" 

// you should do test1 -> test3 one by one.
// this value can be 1,2,3
//
uint8 g_testcast = 1; 

int8 action_demo1( void * param );
int8 action_demo2( void * param );

void actsche_test( uint8 testcase )
{
	TActionScheduler m_acts;
	TActionScheduler * acts = &m_acts;
	
/*	
	acts_construct( acts, sizeof(TActionScheduler) );

	acts_begin_duration( acts );
	acts_inputevent( acts, acts_action_demo1, NULL, 500 );
	acts_inputevent( acts, acts_action_demo2, NULL, 1000 );
	acts_inputevent( acts, acts_action_demo1, NULL, 1000 );
	acts_inputevent( acts, acts_action_demo1, NULL, 2000 );
	acts_inputevent( acts, acts_action_demo2, NULL, 2500 );

	acts_start_softdriven( acts );	
	acts_destroy( acts );

	switch (testcase)	
	{
	case 1:
		acts_start_softdriven( acts, 30 );
		break;
	case 2:
		acts_start_querydriven( acts, 30 );
		break;
	case 3:
		acts_configure( g_timer, acts_default_timer_callback );
		acts_start_interruptdriven( acts );
		break;
	} 
*/
	acts = acts;
}

int8 action_demo1( void * param )
{
	led_toggle( LED_GREEN );
	return 0;
}

int8 action_demo2( void * param )
{
	led_toggle( LED_RED );
	return 0;
}

