
hal_clock��hal_rtcӦ��ֻ����1��
���鱣��hal_clock
���Ƕ���Ľӿڲ���TiCalTime���Կ�����Ա�Ѻ�

����TiSytemTime�أ����ö���Ŀ⺯�����system time(uint64)��caltime��ת���أ�
�е�Ӳ��ֻ֧��systemtime, ��atmega128���е�ȴ֧��ֱ�Ӷ���caltime

hal_rtc => caltime
hal_clock => system time
hal_systicker => osx kernel ר�� support 32 bit system time, simple and efficient
  implement the osx timer interface required by the osx

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

