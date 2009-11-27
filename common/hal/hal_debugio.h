#ifndef _HAL_DEBUGIO_H_6579_
#define _HAL_DEBUGIO_H_6579_

/* hal_debugio
 * Debug input/output utilities for HAL layer. Currently, this module support input 
 * from UART and output to group LED. 
 * 
 * design principle:
 *	- as simple as possible. so its easier to be ported to other platforms.
 *  - small code footprint.
 * 
 * @author zhangwei in 200506
 *	- first created 
 * @modified by huanghuan in 200607
 *	- developed LPC2136 ARM7 version
 */

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_uart.h"
#include "hal_led.h"

/* The default configuration is to use UART0. You may need to modify it to adapt
 * to your own hardware platform */

#define CONFIG_DBO_UART0
#undef  CONFIG_DBO_UART1
#define CONFIG_DBO_TXBUFFER_SIZE 64

/* dbo_open()
 * before you use the dbo functions, you should call dbo_open() first to initialize
 * necessary PIN directions and UART parameters.
 * 
 * dbo_led(state)
 * display state value using the default three LEDs
 * state = 000 wll turn off all the LEDs. other values include 001 to 111.
 */

#ifdef CONFIG_DEBUG
void _dbo_open( uint8 uart_id, uint16 bandrate );
void _dbo_close();
char _dbo_getchar();
void _dbo_putchar( char c);
void _dbo_putbyte( uint8 val );
void _dbo_write( char * buf, uintx len );
void _dbo_write_n8toa( char * buf, uintx len );
uintx _dbo_asyncwrite( char * buf, uintx len );
void _dbo_evolve();
char _dbo_digit2hexchar( uint8 num );
void dbo_string( char * string );

inline void dbo_open( uint8 uart_id, uint16 baudrate ) {_dbo_open((uart_id),(baudrate));}
inline void dbo_close() {_dbo_close();}
inline void dbo_getchar() {_dbo_getchar();}
inline void dbo_putchar( char c ) {_dbo_putchar(c);}
inline void dbo_write( char * buf, uint8 len ) {_dbo_write(buf,len);}
inline void dbo_asyncwrite( char * buf, uint8 len ) {_dbo_asyncwrite(buf,len);}
inline char dbo_digit2hexchar( uint8 num ) {return _dbo_digit2hexchar(num);}

inline void dbo_mem(char * buf, uint8 len) {_dbo_write(buf,len);}
inline void dbo_char(char c) {_dbo_putchar(c);}
inline void dbo_byte(uint8 c) {_dbo_putchar(c);}
inline void dbo_uint8(uint8 n) {_dbo_putchar(n);}
inline void dbo_uint16(uint16 n) {_dbo_write((char*)&(n),sizeof(n));}
inline void dbo_uint32(uint32 n) {_dbo_write((char*)&(n),sizeof(n));}

inline void dbo_n8toa( uint8 n )
{
	_dbo_putchar(_dbo_digit2hexchar(((n)&0xF0) >> 4));	
	_dbo_putchar(_dbo_digit2hexchar((n)&0x0F));
}			

inline void dbo_n16toa( uint16 n )
{
	_dbo_putchar( dbo_digit2hexchar(((n)>>12) & 0x000F) );   
	_dbo_putchar( dbo_digit2hexchar(((n)>>8) & 0x000F) );	
	_dbo_putchar( dbo_digit2hexchar((n)>>4 & 0x000F) );	
	_dbo_putchar( dbo_digit2hexchar(n & 0x000F) );
}

inline void dbo_ledon( uint8 id ) 
{
	led_on(id);
}

inline void dbo_ledoff( uint8 id ) 
{
	led_off(id);
}

inline void dbo_led( uint8 state )
{
	(state & 0x04) ? led_on(LED1) : led_off(LED1);
	(state & 0x02) ? led_on(LED2) : led_off(LED2);
	(state & 0x01) ? led_on(LED3) : led_off(LED3);
}
#endif 

#ifndef CONFIG_DEBUG
#define dbo_open(uart_id,baudrate) {}
#define dbo_close() {}
#define dbo_getchar() {}
#define dbo_putchar() {}
#define dbo_write(buf,len) {}
#define dbo_asyncwrite(buf,len) {}
#define dbo_digit2hexchar(num) {}
#define dbo_mem(buf,len) {}
#define dbo_char(c) {}
#define dbo_byte(c) {}
#define dbo_uint8(n) {}
#define dbo_uint16(n) {}
#define dbo_uint32(n) {}
#define dbo_n8toa(n) {}
#define dbo_n16toa(n) {}								
#define dbo_ledon(id) {}
#define dbo_ledoff(id) {}
#define dbo_led(state) {}
#endif 

#endif /* _HAL_DEBUGIO_H_6579_ */
