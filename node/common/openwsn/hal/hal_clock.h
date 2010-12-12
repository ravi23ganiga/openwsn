hardware level: TiTimerAdapter, TiOsxTimerAdapter (formerly known as TiSysTimer)
rtl: rtl_time, rtl_systime, rtl_caltime, rtl_datetime, calendar time, world time
svc: svc_systimer, svc_timer

本来也是可以按照c语言方式用uint64来存储time即可，不用区分date和time
我们命名为datetime其实不好，应该改
对sensor而言，是不care具体的date time的，它无所谓day night，只要精度高即可
如果区分，都是在sink上实现的。

在MCU上，不妨就用一个整数，但是为了扩展考虑，可以后扩展，故又利用了b1位

Ti

TiTime
time的二进制存储
2B flag, extend flag, value 2^14 year
不好不好
要尽可能简单
只分2 segments or extended segments

based on ms
from application started --- as origin 0
the device doesn't  care year/month/day or more

the system timer also implement the TiOsxTimer interface, so it can be used 
to drive the osx kernel

typedef char[10] TiSystemTime;

first 6 byte is major 
last 4 byte is minor
and last is micro


typedef uint32 version_t;
#define TiVersion version_t

#define MAKE_VERSION(major,minor,micro) (((major&0x0FF)<<16) | ((minor&0xFF)<<8) | (micro&0xFF))
#define MAJOR_VERSION(ver) ((ver>>16) & 0xFF)
#define MINOR_VERSION(ver) ((ver>>8) & 0xFF)
#define MICRO_VERSION(ver) (ver&0xFF)

time64_majorptr( based on seconds )   6B
time64_minorptr( based on millseconds )  2B

time80_add
time80_sub
time80_majorptr( based on seconds )   6B
time80_minorptr( based on millseconds )  2B
time80_microptr( based on microseconds ) 2B
time80_

typedef uint32 TiTime32

typedef struct{
    unsigned char[6] major;
    uint16 minor;
}TiTime64;


typedef struct{
    unsigned char[6] major;
    uint16 minor;
    uint16 micro;
}TiTime80;

time80_pack
time80_unpack

    

systime_clear
systime_set()   
systime_get
systime_sub
systime_add
systime_major
systime_minor
systime_plus
systime_major
systime_minor

systimer_cease/sleepfor(ms)

#define TiSystemTime TiTime80

TiTime80 可以支持用micro支持到us
TiSystemTimer类用于计时

还是把这个类放到hal吧，以充分利用硬件能力提高性能

TiCountTimer 
TiClock
TiClockAdapter


systimer

TiTimer


High Resolution Time

/**
 * TiWorldTimer
 * This component is used to record the current date time. It's based on an physical
 * timer or physical real time clock(RTC). It can:
 * - Can records the precision time, even including the nano-seconds or more.
 * - Can records very long time duration. Currently, the time origin is 2000.01.01 00:00:00:0000
 * - The world timer is based on hardware timer (through TiTimerAdapter) or hardware 
 *   RTC (through TiRtcAdapter) depends on specified target hardware. So the implementation
 *   may various depends on the hardware. Remember to access this object through it's
 *   interface functions intead of accessing the structure variable directly.
 * 
 * The TiWorldTimer itself has an internal time counter. This counter will increase
 * when it receives an event. So generally, you should configure an TiTimerAdapter
 * or TiRtcAdapter to generate these events to drive the TiWorldTimer to run. Since
 * it's based on them, it's more easily to port to other platforms.
 * 
 * For efficiency and resolution reasons, this component uses TiDateTime to record 
 * the time instead of the inefficient TiCalendarTime. 
 * 
 * Attention currently, the TiDateTime occupies 10 bytes so that it's capable to 
 * records even the micro seconds (us). It's useful in high precision measurement 
 * applications. The TiDateTime can also expand to record nano seconds, but the 
 * current implementation doesn't support ns and ps by default.
 */

bool (* TiWorldTimerGetOffset)( void * object, char * TiDateTime );

/**
 * getoffset: This is an function pointer. This function pointer is used to return 
 *      the offset value inside a single piece of slice. Since the lowest resolution
 *      is the length of one slice, it's usually pretty large for high precision
 *      measurement applicatioins. Since the slice counter is increased by the call
 *      of wtm_inputevent() function, the time resolution is decided by the interval
 *      between two calls. This is usually done by the timer hardware's interrupt,
 *      and it's usually not too fast. For example, every 10 ms a interrupt is generated.
 *      So if you want to get the precise time value, you should add the offset inside
 *      one slice. The offset is usually read from the timer hardware. For example, 
 *      the timer_elapsed() function call return how many counts since last expiration.
 * 
 *      If you don't provide this function and the value is null, then the time resolution
 *      is decided by the call of wtm_inputevent() function.
 */



typedef struct{
  uint8 state;
  TiDateTime curtime;

  TiWorldTimerGetOffset getoffset;
  uint16 interval;
  uint16 counter0;
  uint16 counter1;
  int16 counter0delta;

  TiDateTime futuretime;
  uint8 expired;
  TiFunEventHandler listener;
  void * lisowner;
  uint8 option;
}TiWorldTimer;

/** 
 * Q: an example of getoffset function
 * R:
 * bool time0_get_offset( void * object, TiDateTime * value )
 * {
 *   memset( value, 0x00, sizeof(TiDateTime) );
 *   value->offset = time_elapsed( object );
 *   return true;
 * }
 */

TiWorldTimer * wtm_construct( char * buf, uint16 size );
void wtm_destroy( TiWorldTimer * wtm );

on_clk_trigger, provided by the application
This function will read the offset value from the 

/**
 * wtm_open()
 * Initialize an TiWorldTimer component. Attention, you should attach the wtm_inputevent()
 * and an hardware timer to really push the world timer component to run.
 *
 * @param slice_length Length of one time slice. Generally based on milliseconds.
 * 
 */
TiWorldTimer * wtm_open( TiWorldTimer * wtm, uint16 slice_length, TiFunEventHandler listener, void * lisowner );
void wtm_destroy( TiWorldTimer * wtm );

/**
 * Everytime this function called, the inside timer slice counter will increase. 
 * periodically call this function to push the world timer component to run continuously.
 * 
 * @attention
 * Generally, this function is often executed in interrupt mode. So don't do too
 * much work inside this function in order to keep timing precision.
 */
void wtm_inputevent( void * wtmptr, TiEvent * e );

/**
 * If you want the TiWorldTimer component to call the listener at some time, then
 * you should call wtm_evolve() to check for that event, because the listener is
 * actually called by the evolve mechanism.
 */
void wtm_evolve( TiWorldTimer * wtm, TiEvent * e );


void wtm_forward( TiWorldTimer * wtm, uint16 milliseconds );
void wtm_backward( TiWorldTimer * wtm, uint16 milliseconds );

void wtm_curtime( TiWorldTimer * wtm, , TiDateTime * curtime );
void wtm_setcurtime( TiWorldTimer * wtm, , TiDateTime * curtime );
TiDateTime * wtm_curtimeptr( TiWorldTimer * wtm );
void wtm_snapshot( TiWorldTimer * wtm, TiDateTime * curtime );

void wtm_setoffset( TiWorldTimer * wtm, TiDateTime * offset );
void wtm_tune( TiWorldTimer * wtm, TiDateTime * delta );

void wtm_setlistener( TiWorldTimer * wtm, TiFunEventHandler listener, void * lisowner );

/**
 * Set future time. If the current time equals the future time, then wtm_expired() will
 * return true. If the listener callback hander is set, then this handler will be 
 * invoked by the evolve() function.
 */

void wtm_setfuture( TiWorldTimer * wtm, TiDateTime * future );
void wtm_setinterval( TiWorldTimer * wtm, TiDateTime * interval, uint8 option );

/** 
 * Return how many ms elapsed since this interval begins.
 */
void wtm_elapsed( TiWorldTimer * wtm, TiDateTime * offset );

bool wtm_expired( TiWorldTimer * wtm );
void wtm_setexpired( TiWorldTimer * wtm, bool value );


