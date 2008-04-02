#include "os_foundation.h"
#include <../hal/hal_interrupt.h>
#include "os_interrupt.h"

// option
// [b7]  attached or not
// [b6]
// [b4 b3 b2 b1 b0] priority
//
typedef struct{
  uint8 option;
  TOsInterruptHandler handler;
}TOsInterrupt;

static TOsInterrupt g_inttable[CONFIG_INTERRUPT_CAPACITY];

// if your mcu only support 1 hardware interrupt, you can use the following
// trick to simulate multiple interrupt
//
static void os_global_interrupt();


void os_attach( uint8 num, TOsInterruptHandler handler, uint8 prior )
{
	os_detach( num );
	
	g_inttable[num].option = 0;
	g_inttable[num].option |= prior;
	g_inttable[num].handler = handler;
	g_inttable[num].option |= 0x80;  

void os_detach( uint8 num )
{
	g_inttable[num].option &= 0xEF;
}

void os_enable( uint8 num )
{
	g_inttable[num].option |= 0x50;
}

void os_disable( uint8 num )
{
	g_inttable[num].option &= 0xBF;
}

// 
void os_enter_critical()
{
	hal_disable_interrupts();
}

void os_leave_critical()
{
	hal_enable_interrupts();
}

// trigger the interrupt handler manually
// 
void os_trigger( uint8 num )
{
	if (g_inttable[num].option & 0xC0 == 0xC0)
	{
		g_inttable[num].handler();
	}
}


void os_global_interrupt()
{
	return;
}

