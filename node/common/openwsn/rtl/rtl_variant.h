#ifndef _RTL_VARIANT_H_56YT_
#define _RTL_VARIANT_H_56YT_
/*******************************************************************************
 * @copyright(c)	2004-2010, XXX Tech. Co. Ltd
 * @author			Zhang Wei
 * @version			1.0
 * @Date			2004-09-08
 * @status			developing
 *
 * @description
 *  The file is the implementation file of functions defined in sysdefn.h
 *
 * @interface
 * @implementation
 *		short description of some important algorithms here
 *
 * @history
 * @modified by Zhang Wei on 2004-09-09
 *	- first created
 * @modified by Zhang Wei on 2010.04.28
 *  - revision
 ******************************************************************************/

/*
 * TVarDescription is a general object to hold variants. It is similar to the OLE variant object.
 * You can put all kinds of values into this object.
 *
 * Be carefult to distinguish three kind of cases:
 * 1) the object has not been created
 * 2) the object has been created but has NOT been assigned a value. It's current value
 *	should be NULL
 * 3) the object has been created and has assigned a non_NULL value.
 *
 * @wwarning: The current version doesn't provide elegant support to NULL values yet!
 *	Be careful when you use this object!
 */

#include "rtl_configall.h"
#include "rtl_foundation.h"

typedef enum {
  VAR_TYPE_NULL = 0,
  VAR_TYPE_BIT,
  VAR_TYPE_BOOL,
  VAR_TYPE_CHAR,
  VAR_TYPE_INT8,
  VAR_TYPE_UINT8,
  VAR_TYPE_INT16,
  VAR_TYPE_UINT16,
  VAR_TYPE_INT32,
  VAR_TYPE_UINT32,
  VAR_TYPE_F32,
  VAR_TYPE_F64,
  VAR_TYPE_STRING,
  VAR_TYPE_BINARY,
}TiVarType;

typedef bool (* TiFunOnValueChanged)( void * var, void * e );

typedef struct{
    uint16      memsize;
    uint8       type;
    char *      name;
    uint16      size;
    uint16      datalen;
	TiFunOnValueChanged onvaluechanged;
}TiVariant;

TiVariant * var_construct( void * mem, uint16 memsize );
void var_destroy( TiVariant * var );
TiVariant * var_create( uint16 memsize );
void var_free( TiVariant * var );
TiVariant var_resize( TiVariant * var, uint16 newsize );
TiVariant * var_duplicate( TiVariant * var );
TiVariant * var_assignfrom( TiVariant * var, TiVariant * varfrom );
TiVariant * var_assignto( TiVariant * var, TiVariant * varto );

void var_clear( TiVariant * var );
bool var_isnull( TiVariant * var );
uint8 var_type( TiVariant * var );
void var_settype( TiVariant * var, uint8 type );
uint16 var_size( TiVariant * var );
uint16 var_length( TiVariant * var );
char * var_dataptr( TiVariant * var );
uint16 var_setvalue( TiVariant * var, uint8 type, char * buf, uint16 len );
uint16 var_getvalue( TiVariant * var, uint8 * type, char * buf, uint16 size );

void var_set_bit( TiVariant * var, uint8 bit );
uint8 var_get_bit( TiVariant * var );
void var_set_bool( TiVariant * var, bool value );
bool var_get_bool( TiVariant * var );

void var_set_char( TiVariant * var, char value );
char var_get_char( TiVariant * var );

void var_set_int8( TiVariant * var, char value );
int8 var_get_int8( TiVariant * var );

void var_set_uint8( TiVariant * var, char value );
uint8 var_get_uint8( TiVariant * var );

void var_set_int16( TiVariant * var, char value );
int16 var_get_int16( TiVariant * var );

void var_set_uint16( TiVariant * var, char value );
uint16 var_get_uint16( TiVariant * var );

bool var_set_string( TiVariant * var, char * str );
char * var_get_string( TiVariant * var, char * buf, uint16 size );


int32 var_attach( TiVariant * var, TiFunOnValueChanged handler );
int32 var_detach( TiVariant * var );
void var_dump( TiVariant * var, char * buf, uint32 size );

/*
TVarInfo *  var_create( char * name, TVarType type, void * data, uint32 datalen );
TVarInfo *  var_init( void * buf, uint32 buflen, char * name, TVarType type, void * data, uint32 datalen );
void        var_free( TVarInfo * var );
int32       var_get_name( TVarInfo * var, char * buf, uint32 buflen );
int32       var_set_name( TVarInfo * var, char * name );
void *      var_databuf( TVarInfo * var );
uint32      var_capacity( TVarInfo * var );
int32       var_get_data( TVarInfo * var, void * buf, uint32 buflen );
int32       var_set_data( TVarInfo * var, void * data, uint32 datalen );
TVarInfo *  var_duplicate( TVarInfo * var );
void        var_clear( TVarInfo * var );
int32       var_assign( TVarInfo * var_to, TVarInfo * var_from );
int32       var_isnull( TVarInfo * var );


#define VAR_MAX_NAME_LENGTH 32
#define VAR_INSIDE_LENGTH (sizeof(long double))

#define VAR_STATUS_BIT_NULL     0x01
#define VAR_STATUS_BIT_OFFSET   0x02

typedef struct{
  uint32        var_size;
  uint32        var_status;
  char          var_name[ VAR_MAX_NAME_LENGTH ];
  TVarType      var_type;
  uint32	    var_datalen;
  uint32        var_tag;
  uint32 	    var_reserved[1];
  void *        var_onvaluechanged;
  union{
	void *  	var_pointer;
    char        var_char;
	char *  	var_string;
	int8		var_int8;
	int16		var_int16;
	int32		var_int32;
	uint8		var_uint8;
	uint16		var_uint16;
	uint32		var_uint32;
	float   	var_float;
	double  	var_double;
	long double var_longdouble;
	byte *		var_binary;
	uint32  	var_offset32;
	byte   		var_value[ VAR_INSIDE_LENGTH ];
  };
}TVarInfo;

typedef int32 (* TPFunOnValueChanged)( TVarInfo * var, void * oldvalue, uint32 oldlen,
    void * newvalue, uint32 newlen );

TVarInfo *  var_create( char * name, TVarType type, void * data, uint32 datalen );
TVarInfo *  var_init( void * buf, uint32 buflen, char * name, TVarType type, void * data, uint32 datalen );
void        var_free( TVarInfo * var );
int32       var_get_name( TVarInfo * var, char * buf, uint32 buflen );
int32       var_set_name( TVarInfo * var, char * name );
void *      var_databuf( TVarInfo * var );
uint32      var_capacity( TVarInfo * var );
int32       var_get_data( TVarInfo * var, void * buf, uint32 buflen );
int32       var_set_data( TVarInfo * var, void * data, uint32 datalen );
int32       var_attach( TVarInfo * var, TPFunOnValueChanged event_callback );
int32       var_detach( TVarInfo * var );
TVarInfo *  var_duplicate( TVarInfo * var );
void        var_clear( TVarInfo * var );
int32       var_assign( TVarInfo * var_to, TVarInfo * var_from );
int32       var_isnull( TVarInfo * var );
void        var_dump( TVarInfo * var, char * buf, uint32 buflen );
*/

#endif /* _RTL_VARIANT_H_56YT_ */


