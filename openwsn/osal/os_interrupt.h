// provide an unified interrupt management interface for upper layers ("service" or "application" layer)
//
// since "openwsn" is based on 16b/32b MCU such as MSP430 or ARM7, it supports multiple interrupts.
// for some simple MCU such as PIC16F, it only support one interrupt. but i think it doesn't matter.  I mean 
// the design should support multiple interrupts.
//
// for simplicity and efficiency, "openwsn" would support a simple interrupt priority mechanism rather 
// than a complicated one. it support at most 8 interrupts by default. each has a unique priority number.
// the count can be configured by a macro at compiling time.

#define CONFIG_INTERRUPT_CAPACITY 8

static struct{} g_intinfo[CONFIG_INTERRUPT_CAPACITY];

void os_attach(int i, int vec, int prior);
void os_detach( int i);
void os_enable(int i);
void os_disable(int i); 
void os_enter_critical(); // = disable_all
void os_leave_critical(); // = enable_all
