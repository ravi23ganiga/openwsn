//---------------------------------------------------------------------------

#pragma hdrstop

#include "rtl_iobuf.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

TiIoBuffer * 	iobuf_create( uintx size );
void 			iobuf_free( TiIoBuffer * iobuf );
TiIoBuffer * iobuf_destroy( void * buf, uintx memsize );
void iobuf_free( TiIoBuffer * iobuf );
TiIoBuffer * iobuf_duplicate( TiIoBuffer * iobuf );

uintx iobuf_write( TiIoBuffer * iobuf, char * data, uintx len );
uintx iobuf_writechar( TiIoBuffer * iobuf, char c );
uintx iobuf_writebool( TiIoBuffer * iobuf, bool value );
uintx iobuf_writeint8( TiIoBuffer * iobuf, uint8 value );
uintx iobuf_writeuint8( TiIoBuffer * iobuf, uint8 value );
uintx iobuf_read( TiIoBuffer * iobuf, char * data, uintx len );
char * iobuf_buffer( TiIoBuffer * iobuf );
char * iobuf_data( TiIoBuffer * iobuf );
char * iobuf_length( TiIoBuffer * iobuf );
bool iobuf_full( TiIoBuffer * iobuf );
bool iobuf_empty( TiIoBuffer * iobuf );


