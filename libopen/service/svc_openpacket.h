#ifndef _SVC_OPENPACKET_H_3812_
#define _SVC_OPENPACKET_H_3812_

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
//typedef __packed struct{
#pragma pack(1)   
typedef struct{
  uint8 type;
  char value[NET_MAX_PAYLOAD_LENGTH-4];
}TOpenData;
#pragma pack()   
 
/* TOpenPacket
 * it represents the packet transmitted on the network layer. 
 * it's structure is visible for upper application developers.
 */
//typedef __packed struct{
#pragma pack(1)   
typedef struct{
  uint16 control;
  uint8 seqid;
  uint16 netaddr;
  uint16 destaddr;
  uint16 srcaddr;
  uint8 datalen;
  char data[NET_MAX_PAYLOAD_LENGTH];
}TOpenPacket;
#pragma pack()   

char * opt_ownerframe( char * packet );
char * opt_data( char * packet );

#endif /* _SVC_OPENPACKET_H_3812_ */





 