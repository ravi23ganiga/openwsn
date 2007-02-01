//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// Global Variables with initialization and finalization
//
// 	This module is very important in the whole application. It does not only 
// declares and initializes all the global objects, but also create the 
// relationships between different objects.
// 	In TinyOS and nesC systems, these relationships are constructed by a separate
// Configurator object. As you have seem here, this is not necessary. You should
// understand and adopt the OOP programming model and use it in such conditions.
//  
//----------------------------------------------------------------------------
// @history
// @modified by zhangwei on 2006-10-18 
// 	first created
//
//----------------------------------------------------------------------------

#include "hal_foundation.h" 
#include "hal_global.h"
#include "hal_led.h"

static TUartDriver 			m_uart0;
static TUartDriver 			m_uart1;
static TSpiDriver 			m_spi0;
static TSpiDriver 			m_spi1;
volatile static TCc2420Driver 		m_cc2420;
static TTimer				m_timer0;
static TTimer				m_timer1;
static TTimer				m_timer2;
static TWatchdog 			m_watchdog;
static TMcp6s26                         m_mcp6s26;
static TVibrationSensor                 m_vibration;
static TAdConversion                    m_ad; 

TUartDriver *				g_uart0 = NULL; 
TUartDriver *				g_uart1 = NULL; 
TSpiDriver *				g_spi0 = NULL;  
TSpiDriver *				g_spi1 = NULL;
TCc2420Driver *				g_cc2420 = NULL;
TTimer * 					g_timer0 = NULL;
TTimer * 					g_timer1 = NULL;
TTimer * 					g_timer2 = NULL;
TWatchdog * 				g_watchdog = NULL;
TMcp6s26 *                               g_mcp6s26 = NULL;
TVibrationSensor *                       g_vibration;
TAdConversion *                         g_ad; 
TLocation g_loinfo;
TLocationService * g_loservice;

// an global variable to record whether the system is successfully initialized.
uint8 						g_hal_init = FALSE;

// @attention
//	though the debugio_xxx() used the g_debugio object, it has already declared
// in the debugio module to facilitate programming. so you need NOT declare 
// g_debugio here.
//
// @return
//	0:	success
//	-1 or other negtive:	failed
//
int8 hal_global_construct( void )
{
	g_hal_init = TRUE;
	
	g_uart0 	= uart_construct( 0, (char*)(&m_uart0), sizeof(TUartDriver) ); 
	//g_uart1     = uart_construct( 1, (char*)(&m_uart1), sizeof(TUartDriver) ); 
	g_spi0 		= spi_construct( 0, (char*)(&m_spi0), sizeof(TSpiDriver) );
	g_spi1 		= spi_construct( 1, (char*)(&m_spi1), sizeof(TSpiDriver) );
	
	g_cc2420 	= cc2420_construct( (char*)(&m_cc2420), sizeof(TCc2420Driver), g_spi );
	//g_mcp6s26 = mcp_construct( 0, (char*)(&m_mcp6s26), sizeof(TMcp6s26),g_spi0 );
	//g_ad      = ad_construct( 0, (char*)(&m_ad),  sizeof(TAdConversion) );
        //g_vibration 	= vib_construct(0,(char*)(&m_vibration), sizeof(TVibrationSensor), g_mcp6s26, g_ad  );

	/*
	g_timer0 	= timer_construct( 0, 0, (char*)(&m_timer0), sizeof(TTimer) );
	g_timer1 	= timer_construct( 0, 1, (char*)(&m_timer1), sizeof(TTimer) );
	g_timer2 	= timer_construct( 1, 2, (char*)(&m_timer2), sizeof(TTimer) );
	g_watchdog 	= watchdog_construct( (char*)(&m_watchdog), sizeof(TWatchdog), CONFIG_WATCHDOG_INTERVAL );

	if ( (g_uart0 == NULL) || (g_uart1 == NULL) || (g_spi == NULL) || (g_timer0 == NULL)
		|| (g_cc2420 == NULL) || (g_watchdog == NULL) )
	{
		g_hal_init = FALSE;
		assert( FALSE );
	}
	
	// after constructing all the global objects successfully and link them 
	// successfully, you should do more work to configure them to work.
	// here is a good place for you to check and start the hardware.
	//
	if (g_hal_init)
	{
		spi_configure( g_spi0 );
		spi_configure( g_spi1 );
		timer_configure( g_timer0, NULL, NULL, 1 );
		timer_configure( g_timer1, NULL, NULL, 1 );
		timer_configure( g_timer2, NULL, NULL, 1 );
		uart_configure( g_uart0, 9600, 8, 1, 0, 0x00 );
		uart_configure( g_uart1, 9600, 8, 1, 0, 0x00 );
	}
        */
	return (g_hal_init == TRUE) ? 0 : -1;
}

int8 hal_global_destroy( void )
{
	watchdog_destroy( g_watchdog );
	timer_destroy( g_timer0 );
	timer_destroy( g_timer1 );
	timer_destroy( g_timer2 );
	cc2420_destroy( g_cc2420 );
	spi_destroy( g_spi );
	uart_destroy( g_uart0 );
	uart_destroy( g_uart1 );
	
	mcp_destroy( g_mcp6s26 );
	return 0;
}
