
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "svc_foundation.h"
#include "svc_console.h"

TConsole * console_construct( char * buf, uint16 size, TUartDriver * uart )
{
	TConsole * con;
	
	if (sizeof(TConsole) < size)
		con = (TConsole *)buf;
	else
		con = NULL;
		
	if (con != NULL)
	{
		con->uart = uart;
	}
	
	return con;
}

void console_destroy( TConsole * con )
{
	assert( con != NULL );
	uart_destroy( con->uart );
}

void console_reset( TConsole * con )
{
	uart_reset( con->uart );
}

char console_getchar( TConsole * con )
{
	char ch = '\0';
	while (uart_getchar(con->uart, &ch) != 0)
		NULL;
	
	return ch;
}

void console_putchar( TConsole * con, char ch )
{
	while (uart_putchar(con->uart, ch) != 0)
		NULL;
		
	return;
}

// 循环调用console_getchar()函数从console输入流中读入一批数据，数据的终止以收到标志字符
// endinput为准。结束标志字符通常是' '或者<Enter>。该函数返回的情况有两种：输入字符个数
// 达到参数size规定的上限，或者遇到了endinput标志。如果终止条件不满足，该函数将一直等待。
//
// @attention
//	buf中的字符串不是一个以NULL结束的标准C字符串，应作为字符数组看待
// @return
//	缓冲区字符个数，如果该数值<size，则最后一个字符必然是endinput标志
//
uint16 console_read( TConsole * con, char * buf, uint16 size, char endinput )
{
	uint16 count=0;
	char ch;
	
	while (count < size)
	{
		ch = console_getchar( con );
		buf[count] = ch;
		count ++;
		
		if (ch == endinput)
			break;
	}
	
	return count;
}

void console_write( TConsole * con, char * buf, uint16 len )
{
	uint16 n;
	for (n=0; n<len; n++)
		console_putchar( con, buf[n] );
}

#ifdef CONSOLE_PROMPT_ENABLE
int16 console_prompt( TConsole * con, char * msg, char * buf, uint16 size )
{
	console_writestring( con, msg );
	return console_readline( con, buf, size );
}
#endif

// read a line from the console. The line is ended by pressing the <Enter> key.
// @attention
// 	不论出现何种条件返回，该函数总是保证buf中返回的是一个合法的以\0结束的标准C字符串
// 	
uint16 console_readline( TConsole * con, char * buf, uint16 size )
{
	uint16 len;
	char endflag = CR;
	len = console_read( con, buf, size-1, endflag );
	if (len < size-1)
		buf[len-1] = '\0';
	else
		buf[size-1] = '\0';
	return len;
}

void console_writeline( TConsole * con, char * buf )
{
	console_write( con, buf, strlen(buf) );
	console_putchar( con, LF );
	console_putchar( con, CR );
}

int16 console_readstring( TConsole * con, char * buf, uint16 size )
{
	uint16 count;
	count = console_read( con, buf, size, ' ' );
	if (count < size)
	{
		buf[count] = '\0';
		count ++;
	}
		
	return count;
}

#define console_readchar(con) console_getchar(con)

uint8 console_readuint8( TConsole * con )
{
	NULL;
	return 0;
}

uint16 console_readuint16( TConsole * con )
{
	NULL;
	return 0;
}

uint32 console_readuint32( TConsole * con )
{
	NULL;
	return 0;
}

int8 console_readint8( TConsole * con )
{
	NULL;
	return 0;
}

int16 console_readint16( TConsole * con )
{
	NULL;
	return 0;
}

int32 console_readint32( TConsole * con )
{
	NULL;
	return 0;
}

#define console_writechar(con,ch) console_putchar(con,ch)

void console_writestring( TConsole * con, char * str )
{
	uart_write( con->uart, str, strlen(str), 0 );
}

void console_writeuint8( TConsole * con, uint8 value )
{
	char buf[4];
	
	buf[0] = '0';
	buf[1] = 'x';
	buf[2] = (value >> 4) + '0';
	buf[3] = (value & 0x0F) + '0';
	
	console_write( con, &(buf[0]), 4 );
}

void console_writeuint16( TConsole * con, uint16 value )
{
	NULL;
}

void console_writeuint32( TConsole * con, uint16 value )
{
	NULL;
}

