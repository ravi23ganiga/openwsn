/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 ****************************************************************************/ 

#ifndef _RTL_BINARYXML_H_4236_
#define _RTL_BINARYXML_H_4236_

/*****************************************************************************
 * @name BinaryXML
 * @author zhangwei on 20070331
 * 
 * this module implements a BinaryXML for embedded systems.
 * since we need to read/write different type data in the packets, we hope to
 * design a flexible packet payload data structure. so we design the BinaryXML.
 * it will take the old TOpenSections instead.  
 * 
 * 
 * @history
 * @modified by xxx on 200704
 * 
 ****************************************************************************/ 

#include "rtl_configall.h" 
#include "rtl_foundation.h"

#define BXML_PROPERTY_TOTAL_LENGTH 0
#define BXML_PROPERTY_XXXX

typedef struct{
  char * buf;
  uint8 capacity;
  uint8 length;
  uint8 current;
}TiBinaryXml;

//#define xml_update  xml_write

TiBinaryXml * xml_construct( TiBinaryXml * xml, char * xmldata, uint8 size );
void  xml_destroy( TiBinaryXml * xml );
void  xml_attach( TiBinaryXml * xml, char * xmldata, uint8 len ); 
uint8 xml_newnode( TiBinaryXml * xml, uint8 request_size );
uint8 xml_append( TiBinaryXml * xml, uint8 parid, uint8 property, char * data, uint8 datalen );
uint8 xml_write( TiBinaryXml * xml, uint8 id, uint8 parid, uint8 property, char * data, uint8 datalen );
uint8 xml_read( TiBinaryXml * xml, uint8 id, uint8 * parid, uint8 * property, char * data, uint8 size );
void  xml_remove( TiBinaryXml * xml, uint8 id );
uint8 xml_findparent( TiBinaryXml * xml, uint8 id );
uint8 xml_findchild( TiBinaryXml * xml, uint8 id );
uint8 xml_findprev( TiBinaryXml * xml, uint8 id );
uint8 xml_findnext( TiBinaryXml * xml, uint8 id );

#endif

