
#ifndef _HAL_OPENFRAME_H_4335_
#define _HAL_OPENFRAME_H_4335_

/******************************************************************************
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
 ****************************************************************************** 
 * @author zhangwei on 20070130
 * this module implements the openframe interface
 * the structure of openframe is the same as 802.15.4 frame format. thus it can 
 * be easlily ported to other platforms that support TI/Chipcon cc2420 transceivers.
 * 
 * though this module is not related to hardware. it is still placed into HAL
 * because "hal_cc2420" will use it.
 *  
 * @history
 * @modified by xxx on 20070201
 *  
 *****************************************************************************/

#include "hal_foundation.h"
	
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
 
/* you cannot change following defination. they are defined according to 
 * IEEE 802.15.4 specification. RTS and CTS are exception!
 */  
#define OPF_TYPE_BEACON 0x00
#define OPF_TYPE_DATA 	0x01
#define OPF_TYPE_ACK	0x02
#define OPF_TYPE_MACCMD 0x03
#define OPF_TYPE_RTS	0x04
#define OPF_TYPE_CTS	0x05
#define OPF_TYPE_RESERVED 0x06	

typedef struct{
  uint8 len;
  char value[20];
}TOpenAddress;

uint16 mac_getshortpan( TOpenAddress * addr );
uint16 mac_getshortid( TOpenAddress * addr );
void   mac_setshortpan( TOpenAddress * addr, uint16 pan );
void   mac_setshortid( TOpenAddress * addr, uint16 id );

/* the maximum frame length according to IEEE 802.15.4 specification
 */  
#define OPF_FRAME_SIZE 0x7E
#define OPF_PAYLOAD_SIZE (OPF_FRAME_SIZE-8)


/* @attention
 * due to the __packed keyword, the source code can only be compiled with 
 * ARM compiler (provided in ADS)
 *
 * _packed is a ARMCC keyword. it indicates the compile arrange the member variables
 * in the following structure byte by byte. you should attention this if you 
 * want to port OpenMAC to other platform and compiles.
 */
typedef __packed struct{
	uint8 	length;
	uint16 	control;
	uint8  	seqid;
	uint16 	panid;
	uint16 	nodeid;
	uint8  	payload[OPF_PAYLOAD_SIZE];
}TOpenFrame;

TOpenFrame * opf_init( char * buf, uint8 size );
uint8  opf_type( char * buf );
uint8  opf_length( char * buf );
uint16 opf_control( char * buf );
uint8  opf_seqid( char * buf );
uint16 opf_addrfrom( char * buf );
uint16 opf_addrto( char * buf );
uint8  opf_command( char * buf );

void   opf_settype( char * buf, uint8 type );
void   opf_setlength( char * buf, uint8 length );
void   opf_setaddrfrom( char * buf, uint16 addrfrom );
void   opf_setaddrto( char * buf, uint16 addrto );

char * opf_length_addr( char * buf );
char * opf_control_addr( char * buf );
char * opf_seqid_addr( char * buf );
char * opf_addrfrom_addr( char * buf );
char * opf_addrto_addr( char * buf );
char * opf_command_addr( char * buf );
char * opf_psdu( char * buf );
char * opf_msdu( char * buf );

#endif
