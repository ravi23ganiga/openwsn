

#include "hal_flash.h"

/*
typedef struct{
  uint32 addr;
  uint32 size;
  uint16 blocksize;
  uint32 index;
}TFlash;

TFlash * flash_construct( char * buf, uint32 size, uint32 flashaddr, uint32 flashsize,
	uint16 blocksize )
{
	TFlash * flash;
	
	if (sizeof(TFlash) < size)
		flash = (TFlash *)buf;
	else
		flash = NULL;
		
	if (flash != NULL)
	{
		memset( buf, 0x00, sizeof(TFlash) );
		flash->addr = flashaddr;
		flash->size = flashsize;
		flash->blocksize = blocksize;
		index = 0;
	}
	
	return flash;
}

void flash_destroy( TFlash * flash )
{
	NULL;
}

void flash_reset( TFlash * flash )
{
	flash->index = 0;
}

boolean flash_seek( TFlash * flash, int32 index )
{
	flash->index = index;
}

uint32 flash_read( TFlash * flash, char * buf, uint32 size )
{
	//flash_rawread()
}

uint32 flash_write( TFlash * flash, char * buf, uint32 len )
{
	//flash_rawwrite
}
*/
void flash_rawread( uint32 flashaddr, uint32 memaddr, uint32 size )
{
	// TODO
}

void flash_rawwrite( uint32 flashaddr, uint32 memaddr, uint32 size )
{
	// TODO
}

