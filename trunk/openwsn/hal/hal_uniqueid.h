#ifndef _UNIQUEID_H_4289_
#define _UNIQUEID_H_4289_

/***************************************************************************** 
 * @author  makun on  2006-12-14
 * uniqueid 
 * based on Huanghuan's mature code. 
 * find the uniqueid of ds2401. 
 * 
 * @modified by zhangwei on 20061226
 * adjust some notation and re-format the file
 * modified the prototype of uid_read(). pls confirm the implementation.
 * i think serialnumber[6] is unecessary.
 *
 ****************************************************************************/
#include "hal_foundation.h"

typedef struct{
  uint8 id;
  uint8 familycode;
  uint8 serialnumber[6];
  uint8 crcbyte;
}TUniqueIdDriver;

TUniqueIdDriver * uid_construct( uint8 id, char * buf, uint8 size , uint8 opt);
void  uid_destroy( TUniqueIdDriver * uid );
uint8 uid_read( TUniqueIdDriver * uid, char * buf, uint8 size, uint8 opt );
uint8 uid_reset(void);

#endif