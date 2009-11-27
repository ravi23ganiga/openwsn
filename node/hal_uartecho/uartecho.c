
#define UARTECHO_INTERRUPT
#undef  UARTECHO_INTERRUPT

#include "../common/hal/hal_configall.h"
#include <avr/wdt.h>
#include <string.h>
#include "../common/hal/hal_foundation.h"
#include "../common/hal/hal_uart.h"
#include "../common/hal/hal_led.h"
#include "../common/hal/hal_cpu.h"
#include "../common/hal/hal_target.h"

#define BUF_SIZE 128

static void _targetinit(void);
static void uartecho_simpleoutput(void);
static void uartecho_simple( void );
static void uartecho_readwrite( void );
static void uart_putbyte( TiUartAdapter * uart, uint8 val );

int main( void )
{
	//uartecho_simple();
	uartecho_readwrite();
    //uartecho_simpleoutput();

	return 0;
}

void _targetinit(void)
{
    // todo for xiaoyuezhang
    // ��ο��ú꣬���й�PIN direction����ֱ��д��uart_open�����У�����Ҫȡ��
    // �úꡣ����֮���Բ��������շ�������Ŀ���������û����ȷ����pin direction
	// HAL_SET_PIN_DIRECTIONS();

	target_init();

	wdt_disable();
	led_open();
	led_on( LED_ALL );
	hal_delay(500);
	led_off( LED_ALL );
	// hal_delay(500);
	// led_on( LED_RED );
}

/* putchar/getchar based uartecho */
void uartecho_simple( void )
{
	TiUartAdapter uart;
    char ch;

	_targetinit();
	uart_construct( (void*)(&uart), sizeof(uart) );
	uart_open( &uart, 0, 38400, 1, 0, 0 );

	#ifdef UARTECHO_INTERRUPT
    hal_enable_interrupts();
    #endif

    while (1) 
    {
        led_off( LED_RED );
        if (uart_getchar(&uart, &ch) >= 0)
        {
            led_on( LED_RED );
            uart_putchar(&uart, ch+1 );
        }
    }
    
    uart_close( &uart );
    uart_destroy( &uart );
}

/* read/write based uartecho */
void uartecho_readwrite( void )  
{
	TiUartAdapter uart;
    char mem[BUF_SIZE], * buf;
    uint8 count, len;

	_targetinit();
	uart_construct( (void*)(&uart), sizeof(uart) );
	uart_open( &uart, 0, 38400, 1, 0, 0 );


	#ifdef UARTECHO_INTERRUPT
    //hal_enable_interrupts();
    #endif

    uart_putchar( &uart, '*');

    buf = &(mem[0]);
    len = 0;
	while (1)
	{
        led_off( LED_ALL );
		count = uart_read( &uart, buf+len, BUF_SIZE-len, 0x00 );
		len += count;
        
        if (count > 0) 
        {
            // led_on( LED_YELLOW );
			// uart_putchar( &uart, '*' );
			// uart_putbyte( &uart, count );
			// uart_putchar( &uart, ' ' );
        }

		if (len > 0)
		{
			count = uart_write( &uart, buf, len, 0x00 );
			if (count > 0)
			{
				memmove( buf, buf+count, len-count );
				len -= count;
				led_on( LED_GREEN );
			}
		}

		hal_delay( 50 ); 
	}
    
    uart_close( &uart );
    uart_destroy( &uart );
}

void uart_putbyte( TiUartAdapter * uart, uint8 val )
{
    uart_putchar( uart, (val >> 4) + '0' );
    uart_putchar( uart, (val & 0x0F) + '0' );    
}

void uartecho_simpleoutput(void)
{
	TiUartAdapter uart;
    char i;

	_targetinit();
	uart_construct( (void*)(&uart), sizeof(uart) );
	uart_open( &uart, 0, 38400, 1, 0, 0 );

	#ifdef UARTECHO_INTERRUPT
    hal_enable_interrupts();
    #endif

    while (1) 
    {
        led_on( LED_RED );
        for (i=0; i<10; i++)
        {
            uart_putchar( &uart, '*');
            led_toggle( LED_RED );
            hal_delay( 100 );
        }
        uart_putchar( &uart, '\n' );
		hal_delay( 200 );
		led_off( LED_RED );
		hal_delay( 200 );
    }
    
    uart_close( &uart );
    uart_destroy( &uart );
}
