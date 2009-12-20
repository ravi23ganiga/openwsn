#ifndef _HAL_FOUNDATION_H_3721_
#define _HAL_FOUNDATION_H_3721_
/******************************************************************************
 * @author zhangwei on 20060906
 * foundation.h
 * this is the base foundation file of all the files in the application.
 *
 * @modified by zhangwei on 20060906
 * add "signed" in the typedef of int8
 * this is because some compile will assume char as unsigned type, while here
 * we need a signed char.
 *****************************************************************************/

#define CONFIG_INT2HANDLER_ENABLE
#define CONFIG_INT2HANDLER_CAPACITY 32
#define CONFIG_NANOS_ENABLE

#include "hal_configall.h"
#include <stdint.h>
#include "../rtl/rtl_foundation.h"

/* in WinAVR(avr-gcc), <stdint.h> contains the basic types used in WinAVR such as:
 * int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t,
 * intptr_t, uintptr_t, size_t, wchar_t 
 */

/* System wide event identifier */

#define EVENT_RESET                     1
#define EVENT_RESTART                   2
#define EVENT_WAKEUP                    3
#define EVENT_SLEEP                     4
#define EVENT_TIMER_EXPIRED             5
#define EVENT_UART_DATA_ARRIVAL         7
#define EVENT_DATA_ARRIVAL              7
#define EVENT_DATA_COMPLETE             8
#define EVENT_REQUEST                   9
#define EVENT_REPLY                     10
#define EVENT_ACTION_DONE               11


#ifdef __cplusplus
extern "C" {
#endif

struct _TiHalEvent{
  uintx id;
  void (* handler)(void * object, struct _TiHalEvent * e);
  void * objectfrom;
  void * objectto;
};
typedef struct _TiHalEvent TiHalEvent;

#define TiHalEventId uintx

typedef void (* TiHalEventHandler)(void * object, TiHalEvent * e);
typedef void (* TiFunInterruptHandler)(void);


/* the following type is used to implement atomic mechanism in hal_cpu module.
 * the current settings is adapt to atmega128 8 bit MCU only. you may need to change 
 * the definition when porting to a new architecture. */

#define hal_atomic_t cpu_atomic_t
typedef uint8 cpu_atomic_t;



/******************************************************************************
 * global variable
 *****************************************************************************/

/* g_atomic_level: to keep the atmic nested level. defined in this module. */

extern uint8 g_atomic_level;

/* interrupt number to object's handler mapping table.
 * this table is used to save the relationship between interrupt number and object's
 * event handler. when the hardware interrupt occurs, the kernel can use this
 * table to map the hardware interrupt call to the object's handler call.
 *
 * 	{interrupt number, objects handler, object}
 */

typedef struct{
  uint8 num;
  TiFunEventHandler handler;
  void * owner;
}_TiIntHandlerItem;

/* "iht_" denotes "interrupt number - handler table" */

#ifdef CONFIG_INT2HANDLER_ENABLE
extern _TiIntHandlerItem m_int2handler[CONFIG_INT2HANDLER_CAPACITY];
#endif


/******************************************************************************
 * software initialization of the hal layer
 *****************************************************************************/

void hal_init( TiFunEventHandler listener, void * object );

/******************************************************************************
 * interaction with the upper layer such as the osx kernel or other listeners
 *
 * osx is a ultralight OS kernel running on top of the hal layer. the following 
 * types and functions are used to help interfacing the hal layer objects to 
 * the osx kernel. 
 * 
 * though the following is used to help using osx kernel, this module doesn't 
 * require to include osx modules when compiling. 
 * 
 * @attention
 *	- you must call hal_swinit() some where before you can call the following 
 * functions successfully. 
 *****************************************************************************/


void hal_setlistener( TiFunEventHandler listener, void * listener_owner );
void hal_notifylistener( TiEvent * e );
void hal_notify_ex( TiEventId eid, void * objectfrom, void * objectto );


/* the following two function are used with osx kernel only.
 * since the osx kernel is built on top of the hal layer, you should not call osx_post() 
 * directly in your hal layer module. you should call hal_osxpost() instead. 
 */ 
/*
typedef void (* TiFunOsxPostEvent)( TiEvent * e );
void hal_set_osxpost( TiFunOsxPostEvent osx_post );
void hal_osxpost( TiEventId eid, void * objectfrom, void * objectto );
*/

/* nos (nanos) is an ultralight none-preemptive operating system kernel running
 * on atmega MCU only now. it's much more like a classical operating system.
 * generally, suggest using "osx kernel" instead of "nos" in the openwsn project. 
 * 
 * the following two function are used with nano os kernel only.
 * if you want to use nano os in your application, you should provide a hal_set_nospost()
 * in this module, and the nano os kernel will call this function during its' 
 * initialization.
 * 
 * since the nano os is built on top of the hal layer, you should not call os_post() 
 * directly in your hal layer module. you should call hal_nospost() instead. 
 */ 
/*
#ifdef CONFIG_NANOS_ENABLE 
typedef void (*hal_nostask_t)(void);
void hal_set_nospost( bool (* nospost)(hal_nostask_t task) );
void hal_nospost( void (* task)(void) );
#endif
*/




/*
defined in hal_timer
#ifdef CONFIG_TARGET_GAINZ
  #define tm_value_t uintx
#else
  #define tm_value_t uint32
#endif
*/

//--------------------------------------------------------------------
// obsolete functions
// will be eliminated in the future

typedef int (*_compar_fn_t)(const void *, const void *);

enum  {
  FAIL = 0, 
  SUCCESS = 1
};

enum  {
  TIMER_REPEAT = 0, 
  TIMER_ONE_SHOT = 1, 
  NUM_TIMERS = 2
};

enum  {
  Timer0_maxTimerInterval = 230
};

#define MAX_RFD_DEVICE     5

typedef union _BYTE_VAL
{
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
    } bits;
    BYTE Val;
} BYTE_VAL;

typedef union _WORD_VAL
{
    BYTE v[2];
    WORD Val;
    struct
    {
        BYTE LSB;
        BYTE MSB;
    } byte;
} WORD_VAL;

#define LSB(a)          ((a).v[0])
#define MSB(a)          ((a).v[1])

typedef union _DWORD_VAL
{
    DWORD Val;
    struct
    {
        BYTE LOLSB;
        BYTE LOMSB;
        BYTE HILSB;
        BYTE HIMSB;
    } byte;
    struct
    {
        WORD LSW;
        WORD MSW;
    } word;
    BYTE v[4];
} DWORD_VAL;


#define LOWER_LSB(a)    ((a).v[0])
#define LOWER_MSB(a)    ((a).v[1])
#define UPPER_LSB(a)    ((a).v[2])
#define UPPER_MSB(a)    ((a).v[3])


#ifdef __cplusplus
}
#endif

#endif /*_HAL_FOUNDATION_H_*/
