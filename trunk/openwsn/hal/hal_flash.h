#ifndef _FLASH_H_5783_
#define _FLASH_H_5783_

#include "hal_foundation.h"

#define flash_read(faddr,maddr,size) flash_rawread(faddr,maddr,size)
#define flash_write(faddr,maddr,size) flash_rawwrite(faddr,maddr,size)

void flash_rawread( uint32 flashaddr, uint32 memaddr, uint32 size );
void flash_rawwrite( uint32 flashaddr, uint32 memaddr, uint32 size );

#endif