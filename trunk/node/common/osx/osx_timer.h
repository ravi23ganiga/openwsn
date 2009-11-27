#ifndef _OSX_TIMER_H_4353_
#define _OSX_TIMER_H_4353_
/******************************************************************************
 * TiOsxTimer
 * the osx timer is used to drive the evolution of the whole osx kernel by periodically
 * call osx_hardevolve() function. it can also be used to drive some inaccurate
 * sampling tasks. if there's no osx timer, then the system can also drive the
 * osx kernel by infinite loop run. refer to the osx_execute() function. the infinite
 * loop run or osx_execute() can be placed into a separate thread of other RTOS.
 *
 * the MCU's hardware timers are usually very powerful. furthermore, they maybe
 * varies from chip suppliers. however, the OS core only needs a relatively simple
 * timer. it should be lightweight and efficient.
 *
 * for some MCU such as the ARM Cortex M3, the MCU core has already provide a
 * hardware SysTimer to help implement a OS core.
 *****************************************************************************/

#define TiOsxTimer TiSysTimer

inline TiOsxTimer * _osx_timer_construct( char * buf, uint8 size )
{
	return systm_construct( buf, size );
}

inline void _osx_timer_destroy( TiOsxTimer * timer )
{
	systm_destroy( timer );
}

inline TiOsxTimer * _osx_timer_open( TiOsxTimer * timer, tm_value_t interval, TiFunEventHandler listener, void * lisowner )
{
	return systm_open( timer, interval, listener, lisowner );
}

inline void _osx_timer_close( TiOsxTimer * timer )
{
	systm_close( timer );
}

inline void _osx_timer_start( TiOsxTimer * timer )
{
	systm_start( timer );
}

inline void _osx_timer_stop( TiOsxTimer * timer )
{
	//void systm_stop( timer );
}

inline bool _osx_timer_expired( TiOsxTimer * timer )
{
	//return systm_expired( timer );
    return true;
}

#endif
