//----------------------------------------------------------------------------
// TDataSet
// this object represent an collection of data items. you can regarded it as a
// in-memory ultra-light incomplete database.
//
// dataset is different to Queue. the items in queue object has their order. 
// however, the order of data items in dataset can be sorted. 
//----------------------------------------------------------------------------

#ifndef _SVC_DATASET_H_7887_
#define _SVC_DATASET_H_7887_

#include "svc_configall.h"
#include "..\rtl\rtl_binaryxml.h"

// TDataItem 
// this object denotes a data packet payload received from the sensor network.
// or the query result item return to the upper level application. 
// 
struct _TDataItem{
  uint16 nodeid;
  uint32 x;
  uint32 y;
  uint32 timestamp;
  TBinaryXml * sensordata;
  struct _TDataItem * next;
};

typedef _TDataItem TDataItem;

DLLAPI TDataItem * _stdcall dataitem_create();
DLLAPI void			dataitem_free( TDataItem * item );
DLLAPI TDataItem *	dataitem_construct( TDataItem * item );
DLLAPI void			dataitem_destroy( TDataItem * item );
DLLAPI void			dataitem_clear( TDataItem * item );
DLLAPI int			dataitem_rawread( TDataItem * item, char * buf, uint16 capacity );
DLLAPI int			dataitem_rawwrite( TDataItem * item, char * buf, uint16 len );
DLLAPI int			dataitem_write( TDataItem * item, uint16 nodeid, uint32 x, uint32 y, uint32 timestamp, TBinaryXml * sensordata );
DLLAPI uint16		dataitem_get_nodeid( TDataItem * item );
DLLAPI int			dataitem_get_location( TDataItem * item, uint32 * x, uint32 * y );
DLLAPI uint32		dataitem_get_timestamp( TDataItem * item );
DLLAPI TBinaryXml *	dataitem_get_sensordata( TDataItem * item );


// TDataSet
// used to hold a collection of TDataItem. this object can be used as the 
// result of one query or as the cache in the QueryEngine.
//
struct _TDataSet{
  TDataItem * itemlist;
  struct _TDataSet * next;
};

typedef _TDataSet TDataSet;

DLLAPI TDataSet * dataset_create();
DLLAPI void dataset_free( TDataSet * ds );
DLLAPI void dataset_clear( TDataSet * ds );
DLLAPI int  dataset_save( TDataSet * ds, char * filename );
DLLAPI int  dataset_load( TDataSet * ds, char * filename );

#endif

