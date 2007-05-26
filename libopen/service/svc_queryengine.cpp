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
#include "svc_queryengine.h"

// @TODO

static int query_engine_test()
{
}


