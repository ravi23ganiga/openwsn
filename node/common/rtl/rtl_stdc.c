#include "rtl_stdc.h"

void rtl_memset( void * mem, char ch, int len )
{
	int i;
	char * p = (char *)mem;
	for (i=0; i<len; i++)
	{
		*p = ch;
		p++;
	}
}