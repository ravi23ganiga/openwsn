#include "svc_configall.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <../hal/hal_assert.h>
#include "svc_foundation.h"
#include "svc_console.h"

TiConsole * console_construct( char * buf, uint16 size, TiUartAdapter * uart )
{
	TiConsole * con;
	
	if (sizeof(TiConsole) < size)
		con = (TiConsole *)buf;
	else
		con = NULL;
		
	if (con != NULL)
	{
		con->uart = uart;
	}
	
	return con;
}

void console_destroy( TiConsole * con )
{
	assert( con != NULL );
	uart_destroy( con->uart );
}

void console_reset( TiConsole * con )
{
	uart_reset( con->uart );
}

char console_getchar( TiConsole * con )
{
	char ch = '\0';
	while (uart_getchar(con->uart, &ch) != 0)
		NULL;
	
	return ch;
}

void console_putchar( TiConsole * con, char ch )
{
	while (uart_putchar(con->uart, ch) != 0)
		NULL;
		
	return;
}

// ѭ������console_getchar()������console�������ж���һ�����ݣ����ݵ���ֹ���յ���־�ַ�
// endinputΪ׼��������־�ַ�ͨ����' '����<Enter>���ú������ص���������֣������ַ�����
// �ﵽ����size�涨�����ޣ�����������endinput��־�������ֹ���������㣬�ú�����һֱ�ȴ���
//
// @attention
//	buf�е��ַ�������һ����NULL�����ı�׼C�ַ�����Ӧ��Ϊ�ַ����鿴��
// @return
//	�������ַ��������������ֵ<size�������һ���ַ���Ȼ��endinput��־
//
uint16 console_read( TiConsole * con, char * buf, uint16 size, char endinput )
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

void console_write( TiConsole * con, char * buf, uint16 len )
{
	uint16 n;
	for (n=0; n<len; n++)
		console_putchar( con, buf[n] );
}

#ifdef CONSOLE_PROMPT_ENABLE
int16 console_prompt( TiConsole * con, char * msg, char * buf, uint16 size )
{
	console_writestring( con, msg );
	return console_readline( con, buf, size );
}
#endif

// read a line from the console. The line is ended by pressing the <Enter> key.
// @attention
// 	���۳��ֺ����������أ��ú������Ǳ�֤buf�з��ص���һ���Ϸ�����\0�����ı�׼C�ַ���
// 	
uint16 console_readline( TiConsole * con, char * buf, uint16 size )
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

void console_writeline( TiConsole * con, char * buf )
{
	console_write( con, buf, strlen(buf) );
	console_putchar( con, LF );
	console_putchar( con, CR );
}

int16 console_readstring( TiConsole * con, char * buf, uint16 size )
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

uint8 console_readuint8( TiConsole * con )
{
	NULL;
	return 0;
}

uint16 console_readuint16( TiConsole * con )
{
	NULL;
	return 0;
}

uint32 console_readuint32( TiConsole * con )
{
	NULL;
	return 0;
}

int8 console_readint8( TiConsole * con )
{
	NULL;
	return 0;
}

int16 console_readint16( TiConsole * con )
{
	NULL;
	return 0;
}

int32 console_readint32( TiConsole * con )
{
	NULL;
	return 0;
}

#define console_writechar(con,ch) console_putchar(con,ch)

void console_writestring( TiConsole * con, char * str )
{
	uart_write( con->uart, str, strlen(str), 0 );
}

void console_writeuint8( TiConsole * con, uint8 value )
{
	char buf[4];
	
	buf[0] = '0';
	buf[1] = 'x';
	buf[2] = (value >> 4) + '0';
	buf[3] = (value & 0x0F) + '0';
	
	console_write( con, &(buf[0]), 4 );
}

void console_writeuint16( TiConsole * con, uint16 value )
{
	NULL;
}

void console_writeuint32( TiConsole * con, uint16 value )
{
	NULL;
}

