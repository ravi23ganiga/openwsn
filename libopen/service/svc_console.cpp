
#include "svc_foundation.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>
#include "svc_console.h"
#include "svc_siocomm.h"
 
//CWinThread *pThread; 
extern TSioComm   m_TSioComm;
extern TSioComm  *g_pTSioComm;
extern TUartDriver  m_TUartDriver;
extern TUartDriver *g_pTUartDriver;

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

	g_pTUartDriver = uart_construct(0, (char *)&m_TUartDriver, sizeof(TUartDriver));
        g_pTSioComm = sio_construct((char *)&m_TSioComm, 
			sizeof(TSioComm), 
			g_pTUartDriver, 
			SIO_OPTION_FRAME_SUPPORT);
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

// ѭ������console_getchar()������console�������ж���һ�����ݣ����ݵ���ֹ���յ���־�ַ�
// endinputΪ׼��������־�ַ�ͨ����' '����<Enter>���ú������ص���������֣������ַ�����
// �ﵽ����size�涨�����ޣ�����������endinput��־�������ֹ���������㣬�ú�����һֱ�ȴ���
//
// @attention
//	buf�е��ַ�������һ����NULL�����ı�׼C�ַ�����Ӧ��Ϊ�ַ����鿴��
// @return
//	�������ַ��������������ֵ<size�������һ���ַ���Ȼ��endinput��־
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
// 	���۳��ֺ����������أ��ú������Ǳ�֤buf�з��ص���һ���Ϸ�����\0�����ı�׼C�ַ���
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
	console_write( con, buf,(uint16) strlen(buf) );
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
	uart_write( con->uart, str,(uint8) strlen(str), 0 );
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
