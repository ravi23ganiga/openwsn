#ifndef _SVC_OPENSEC_H_5329_
#ifndef _SVC_OPENSEC_H_5329_

/******************************************************************************
 * @author zhangwei on 20070302
 * @description
 * 	this module implements some utility functions to help the user/application
 * manipulate the payload of NET layer packet easily. now it is tensively used
 * in service layer and application layer.
 * 
 * @history
 * @modified by zhangwei on 20070302
 * 	first created
 *****************************************************************************/
  
#include "svc_foundation.h"

/******************************************************************************
 * TiOpenSections
 * a TiOpenSections object is essentially a payload of a NET layer packet. It contains
 * some sections. Each section has its own semantic meaning. However, OpenWSN
 * only regulate the format of the sections, while the application developed by 
 * the user determins the meaning of the section.
 *
 * all REQUEST and ACKNOWLEDGE command are some kind of section of a TiOpenSections 
 * object.
 *  
 * {[1B Data Type][1B Data Length][nB DataValue]}
 *
 * [Count]
 * the field count of later Data section. An APP layer packet can contain serveral 
 * data sections. each section can hold one type data such as Vibration Sensor, 
 * Location, Light Sensor, etc. The current implementation only used the low 4 bit 
 * to store the count. the higher 4 bits are remain unused.
 * 
 * [Data Type]
 * used by the service layer and user defined application layer both. 
 * 	b7,....b1,b0
 * 
 *  b7 = 1 indicate this section is used by the openwsn system. usually, this is 
 * 		   a service layer section. for user defined application layer sections, 
 *         b7 should always be 0.
 *  b6 b5  section control. 00 REQUEST 01 ACK  10 support section length byte 
 *         11 link another section 
 *  b4     reserved. may be for the expansion of [b3,...b0]
 *  b3..b0 indicate the section type. so now support 16 system sections and 16 
 *         user defined sections.
 * 
 *****************************************************************************/

#define TiOpenSection char * 

typedef struct{
  char * buf;
  uint8 len;
  uint8 index;
}TiOpenSections;

TiOpenSections * sec_construct( TiOpenSections * buf, uint8 size );
void sec_destroy( TiOpenSections * sec );
void sec_configure( TiOpenSections * sec, char * buf, uint8 len );

int8   sec_findfirst( TiOpenSections * sec, uint8 * id );
int8   sec_findnext( TiOpenSections * sec, uint8 * id );
void   sec_hasnext( TiOpenSections * sec );
int8   sec_length( TiOpenSections * sec, uint8 id );
int8   sec_write( TiOpenSections * sec, uint8 id, char * buf, uint8 len );
int8   sec_read( TiOpenSections * sec, uint8 id, char * buf, uint8 size );
char * sec_getinside( TiOpenSections * sec, uint8 id, uint8 * size );
uint8  sec_gettype( TiOpenSections * sec, uint8 id );
char * sec_getdata( TiOpenSections * sec, uint8 id, uint8 * len );
char * sec_get( TiOpenSections * sec, uint8 id, uint8 * type, uint8 * len );
int8   sec_set( TiOpenSections * sec, uint8 id, uint8 * type, char * buf, uint8 len );

#endif

