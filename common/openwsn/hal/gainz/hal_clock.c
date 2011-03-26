

TiClockAdapter * clock_construct( char * buf, uint16 size )
{
}

void clock_destroy( TiClockAdapter * clk )
{
}

on_clk_trigger, provided by the application
This function will read the offset value from the 



/* The only TiSysTimer instance. It's used by the OSX kernel only */
TiSysTimer * m_systimer = NULL;


static void _systimer_interrupt_handler( void * object, TiEvent * e );
static inline void _systm_enable( TiSysTimer * timer );
static inline void _systm_disable( TiSysTimer * timer );

