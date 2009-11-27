#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_string.h"

int strlen( char * str )
{
	int counter=0;
	char * pc = str;
	while (*pc != NULL)
	{
		counter ++;
		pc++;
	}
	return counter;
}
