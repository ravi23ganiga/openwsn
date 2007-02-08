#ifndef _SVC_OPENPACKET_H_3812_
#define _SVC_OPENPACKET_H_3812_

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

#include "svc_foundation.h"
#include "..\hal\hal_openframe.h"

/* @author zhangwei on 20070131
 * this file defines the format of a Network Layer Packet. the packet is the 
 * payload of a TOpenFrame.
 * 
 * attention that TOpenPacket and TOpenData are not a service/class, so you can 
 * NOT add member variables to them! they are BYTE-TO-BYTE mapping to the packets
 * transmitted. the other functions should be considered as util function rather
 * than service member functions.
 */


/******************************************************************************
 * IEEE 802.15.4 PPDU format
 * [4B Preamble][1B SFD][7b Framelength, 1b Reserved][nB PSDU/Payload]
 * 
 * IEEE 802.15.4 MAC DATA format (the payload of PHY frame)
 * Beacon Frame
 * [2B Frame Control] [1B Sequence Number][4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 * 
 * Data Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][n Data Payload][2 FCS]
 * 
 * ACK Frame
 * [2B Frame Control] [1B Sequence Number][2 FCS]
 * 
 * MAC Control Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 ADdress][1 Command Type][n Command Payload][2 FCS]
 *
 * Frame Control
 * b2b1b0  	frame type 000 beacon, 001 data 010 ACK 011 command 100-111 reserved
 * b12b13 	reserved.
 * 
 *****************************************************************************/

#define NET_MAX_PAYLOAD_LENGTH 118

/* system used data types */
#define ODA_TYPE_DEBUG 0x00
#define ODA_TYPE_TIMESYNC 0x00
#define ODA_TYPE_LOCATION 0x00
#define ODA_TYPE_UPGRADE 0x00

/* user defined data type 
 * the developer can add more TYPE defines if necessary. */
#define ODA_TYPE_USER 0x80
#define ODA_TYPE_TEMPSENSOR (ODA_TYPE_USER + 0) 
#define ODA_TYPE_VIBSENSOR (ODA_TYPE_USER + 1) 
#define ODA_TYPE_STRAINSENSOR (ODA_TYPE_USER + 2) 

/* TOpenData defination. It is often the payload of a TOpenPacket.
 * 	TOpenData == TOpenPacket.data
 * they two are often the same. 
 * 
 * @attention
 * 	here we don't provide the length member variable. because in most cases, 
 * the length of a type can be determined by "type". for example, the location
 * type data. for example:
 * 		[ODA_TYPE_LOCATION 1][X 4][Y 4][Z 4] 
 * it always contains 17 byte.
 * 
 * for those data with variable length, such as some sensor data, 
 * the BYTE just behind the "type" member, is the length of the data. for 
 * example:
 * 		[ODA_TYPE_UPGRADE 1][NEWDATA n]
 */
typedef __packed struct{
  uint8 type;
  char value[NET_MAX_PAYLOAD_LENGTH-4];
}TOpenData;
 
/* TOpenPacket
 * it represents the packet transmitted on the network layer. 
 * it's structure is visible for upper application developers.
 */
typedef __packed struct{
  uint16 control;
  uint8 seqid;
  uint16 netaddr;
  uint16 destaddr;
  uint16 srcaddr;
  uint8 datalen;
  char data[NET_MAX_PAYLOAD_LENGTH];
}TOpenPacket;

char * opt_ownerframe( char * packet );
char * opt_data( char * packet );



#endif /* _SVC_OPENPACKET_H_3812_ */





 