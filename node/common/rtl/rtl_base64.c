//---------------------------------------------------------------------------
// http://www.cstc.net.cn/docs/docs.php?id=202
// ǳ̸Base64����
//---------------------------------------------------------------------------

#pragma hdrstop
#include <stdlib.h>
#include <stdio.h>
#include "rtl_base64.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

__inline int _get_b64index(char ch);

//---------------------------------------------------------------------------
// this module contains base64 test only
//---------------------------------------------------------------------------

const char * pTextDemo =
	"��ã�SnaiX\r\n"
	"\r\n"
	"��������һ��Base64�Ĳ����ʼ���\r\n"
	"\r\n"
	"Best Wishes!\r\n"
	"\r\n"
	"����������������������������               eSX?!\r\n"
	"����������������������������               snaix@yeah.net\r\n"
	"����������������������������������         2003-12-25"
	;

const char * p64 =
"xOO6w6OsU25haVgNCg0KoaGhodXiysfSu7j2QmFzZTY0tcSy4srU08q8/qOhDQoNCkJlc3QgV2lz"
"aGVzIQ0KIAkJCQkNCqGhoaGhoaGhoaGhoaGhoaGhoaGhoaGhoaGhoaEgICAgICAgICAgICAgICBl"
"U1g/IQ0KoaGhoaGhoaGhoaGhoaGhoaGhoaGhoaGhoaGhoSAgICAgICAgICAgICAgIHNuYWl4QHll"
"YWgubmV0DQqhoaGhoaGhoaGhoaGhoaGhoaGhoaGhoaGhoaGhoaGhoaGhICAgICAgICAgMjAwMy0x"
"Mi0yNQ0K";

int base64_test(int argc, char* argv[])
{
	//	printf("Hello World!\n");

	int len = strlen(pTextDemo);
	char* pBase64 = (char *)malloc(len * 2 + 4);
	char* pszSrc;
	int len_src;
	int len_b64;

	// �����ĳ���һ���ԭ�Ķ�ռ1/3�Ĵ洢�ռ䣬�뱣֤pBase64���㹻�Ŀռ�
	// todo? whether the last paramter should be 0 or not? i add this line to make
	// it compile passed
	len_b64 = base64_encode(pBase64, pTextDemo, 0);
	printf("[Base64]��\r\n%s\r\n\r\n", pBase64);

	pszSrc = (char *)malloc(len + 1);
	len_src = base64_decode(pszSrc, pBase64, 0);
	printf("[Դ��]��\r\n%s\r\n\r\n", pszSrc);

	free( pszSrc );
	free( pBase64 );

	getchar();

	return 0;
}

//---------------------------------------------------------------------------


__inline char _get_b64char(int index)
{
	const char szBase64Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	if (index >= 0 && index < 64)
		return szBase64Table[index];

	return '=';
}

// ��˫����ȡ���ֽ�
#define B0(a) (a & 0xFF)
#define B1(a) (a >> 8 & 0xFF)
#define B2(a) (a >> 16 & 0xFF)
#define B3(a) (a >> 24 & 0xFF)

// �����ĳ���һ���ԭ�Ķ�ռ1/3�Ĵ洢�ռ䣬�뱣֤base64code���㹻�Ŀռ�
inline int base64_encode(char * base64code, const char * src, int src_len)
{
	int len = 0, i, j;
	char * p64;
	unsigned char* psrc;
	int rest;
	unsigned long ulTmp;

	if (src_len == 0)
		src_len = strlen(src);

	psrc = (unsigned char*)src;
	p64 = base64code;
	for (i = 0; i < src_len - 3; i += 3)
	{
		unsigned long ulTmp = *(unsigned long*)psrc;
		register int b0 = _get_b64char((B0(ulTmp) >> 2) & 0x3F);
		register int b1 = _get_b64char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
		register int b2 = _get_b64char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F);
		register int b3 = _get_b64char((B2(ulTmp) << 2 >> 2) & 0x3F);
		*((unsigned long*)p64) = b0 | b1 << 8 | b2 << 16 | b3 << 24;
		len += 4;
		p64  += 4;
		psrc += 3;
	}

	// ����������µĲ���3�ֽڵĶ�����
	if (i < src_len)
	{
		rest = src_len - i;
		ulTmp = 0;
		for (j = 0; j < rest; ++j)
		{
			*(((unsigned char*)&ulTmp) + j) = *psrc++;
		}

		p64[0] = _get_b64char((B0(ulTmp) >> 2) & 0x3F);
		p64[1] = _get_b64char((B0(ulTmp) << 6 >> 2 | B1(ulTmp) >> 4) & 0x3F);
		p64[2] = rest > 1 ? _get_b64char((B1(ulTmp) << 4 >> 2 | B2(ulTmp) >> 6) & 0x3F) : '=';
		p64[3] = rest > 2 ? _get_b64char((B2(ulTmp) << 2 >> 2) & 0x3F) : '=';
		p64 += 4;
		len += 4;
	}

	*p64 = '\0';

	return len;
}


__inline int _get_b64index(char ch)
{
	int index = -1;
	if (ch >= 'A' && ch <= 'Z')
	{
		index = ch - 'A';
	}
	else if (ch >= 'a' && ch <= 'z')
	{
		index = ch - 'a' + 26;
	}
	else if (ch >= '0' && ch <= '9')
	{
		index = ch - '0' + 52;
	}
	else if (ch == '+')
	{
		index = 62;
	}
	else if (ch == '/')
	{
		index = 63;
	}

	return index;
}

// �����ĳ���һ���ԭ������ռ1/4�Ĵ洢�ռ䣬�뱣֤buf���㹻�Ŀռ�
inline int base64_decode(char * buf, const char * base64code, int src_len)
{
	int len = 0, i, j;
	unsigned char* psrc;
	char * pbuf;
	int rest;
	unsigned long ulTmp;
	register int b0;

	if (src_len == 0)
		src_len = strlen(base64code);

	psrc = (unsigned char*)base64code;
	pbuf = buf;
	for (i = 0; i < src_len - 4; i += 4)
	{
		unsigned long ulTmp = *(unsigned long*)psrc;

		register int b0 = (_get_b64index((char)B0(ulTmp)) << 2 | _get_b64index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
		register int b1 = (_get_b64index((char)B1(ulTmp)) << 4 | _get_b64index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
		register int b2 = (_get_b64index((char)B2(ulTmp)) << 6 | _get_b64index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;

		*((unsigned long*)pbuf) = b0 | b1 << 8 | b2 << 16;
		psrc  += 4;
		pbuf += 3;
		len += 3;
	}

	// ����������µĲ���4�ֽڵĶ�����
	if (i < src_len)
	{
		rest = src_len - i;
		ulTmp = 0;
		for (j = 0; j < rest; ++j)
		{
			*(((unsigned char*)&ulTmp) + j) = *psrc++;
		}

		b0 = (_get_b64index((char)B0(ulTmp)) << 2 | _get_b64index((char)B1(ulTmp)) << 2 >> 6) & 0xFF;
		*pbuf++ = b0;
		len  ++;

		if ('=' != B1(ulTmp) && '=' != B2(ulTmp))
		{
			register int b1 = (_get_b64index((char)B1(ulTmp)) << 4 | _get_b64index((char)B2(ulTmp)) << 2 >> 4) & 0xFF;
			*pbuf++ = b1;
			len  ++;
		}

		if ('=' != B2(ulTmp) && '=' != B3(ulTmp))
		{
			register int b2 = (_get_b64index((char)B2(ulTmp)) << 6 | _get_b64index((char)B3(ulTmp)) << 2 >> 2) & 0xFF;
			*pbuf++ = b2;
			len  ++;
		}

	}

	*pbuf = '\0';

	return len;
}

