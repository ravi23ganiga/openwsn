#ifndef _QUERYENGINE_H_4873_
#define _QUERYENGINE_H_4873_

//----------------------------------------------------------------------------
// @author zhangwei on 20070510
//                   
// The Key Idea of OpenWSN Query Interface
// this module implements the QUERY interface for wsn. the original data collected 
// from the wsn network are the following tuples:
//
//	rawdata = {node id/location, timestamp, {sensor type, sensor value}}
//		{sensor type, sensor value} is a BinaryXml format data packet.
//
// while, the fundamental query operation will return the following:
//  
//  range query result = {field_range, time_range, {sensor type, sensor value}}
//		field range = {x1,y1.x2.y2}
//		time_range = {time_start, time_end}
// or the following:
//  specific query result = {node_id/location, time_range, {sensor type, sensor value}}
//----------------------------------------------------------------------------

#include "svc_configall.h"
#include "svc_dataset.h"

// TQueryDataItem 
// this object denotes a data packet payload received from the sensor network.
// or the query result item return to the upper level application. 
//
// Each time the query engine received a packet from the network, it will buffer
// it on the disk or cached in the memory. 
// 
// @attention: zhangwei suggest BerkeleyDB to save such data. because BDB is quite 
// high performance and powerful to deal with TB level data. it has been proved to 
// be very robust for real applications. however, for the first version, you can 
// simply cache some record in the memory and discard those that are too old. 
// 
// TQueryDataSet
// used to hold a collection of TQueryDataItem. this object can be used as the 
// result of one query or as the cache in the QueryEngine.
//
#define TQueryDataItem TDataItem
#define TQueryDataSet TDataSet
#define TQueryResult TDataSet

// the following high level query functions are based on query_engine_execute()
// they are used to facilitate high level GUI developing
//
#define QUERY_CONTROL_NODE_ENABLE		0x0001
#define QUERY_CONTROL_TIMESTART_ENABLE	0x0002
#define QUERY_CONTROL_TIMEEND_ENABLE	0x0004
#define QUERY_CONTROL_LOCATION1_ENABLE	0x0008
#define QUERY_CONTROL_LOCATION2_ENABLE	0x0010
#define QUERY_CONTROL_LOCATION3_ENABLE	0x0020
#define QUERY_CONTROL_RADUIS_ENABLE		0x0040
#define QUERY_CONTROL_MAX_ENABLE		0x0080
#define QUERY_CONTROL_MIN_ENABLE		0x0100

typedef struct{
  uint32 control;
  uint16 node;
  uint32 timestart;
  uint32 timeend;
  uint32 x1;
  uint32 y1;
  uint32 x2;
  uint32 y2;
  uint32 radius;
  uint32 maxvalue;
  uint32 minvalue;
}TQueryRange;

// TQueryCondition
// used to transfer query condition parameters between high level GUI applications
// and lower level QueryEngine.  attention TQueryCondition is essentially a list 
// to describe complicated user inputed conditions. currently, you can only implement
// the processing of one item only.
//
struct _TQueryCondition{
  uint32 type;
  TQueryRange range;
  struct _TQueryCondition * next;
};
typedef _TQueryCondition TQueryCondition;

DLLAPI TQueryCondition * query_cond_create();
DLLAPI void query_cond_free( TQueryCondition * cond );
DLLAPI void query_cond_clear( TQueryCondition * cond );

// TQueryEngine
// interprete and execute the query received from GUI application
//
typedef struct{
  uint8 state;
  TQueryCondition * cond;
  TQueryDataSet * dataset;
  TQueryDataSet * cache;
  char * filename;
}TQueryEngine;

DLLAPI TQueryEngine * query_engine_create();
DLLAPI void query_engine_free( TQueryEngine * qe );
DLLAPI void query_engine_clear( TQueryEngine * qe );
DLLAPI int  query_engine_execute( TQueryEngine * qe, TQueryCondition * cond, TQueryDataSet ** dataset );

DLLAPI int	query_by_node( TQueryEngine * qe, uint16 nodeid, uint32 timestart, 
		uint32 timeend, uint8 sentype, uint8 * value, uint8 capacity );
DLLAPI int	query_by_range( TQueryEngine * qe, TQueryRange * range, uint8 sentype, uint8 * value, uint8 capacity );
DLLAPI int	query_by_location( TQueryEngine * qe, uint32 x, uint32 y, uint32 timestart, 
		uint32 timeend, uint8 sentype, uint8 * value, uint8 capacity, uint8 * probability, int option );
DLLAPI int	query_by_value( TQueryEngine * qe, uint32 control, uint32 minvalue, uint32 maxvalue, 
		TQueryRange * range, uint8 sentype, uint8 * value, uint8 capacity, uint8 * probability );

#endif
