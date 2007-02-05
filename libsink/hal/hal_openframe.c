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

#include "hal_foundation.h"
#include "hal_openframe.h"

/*****************************************************************************
 * Address Manipulations
 * assume 802.15.4 specification address.
 * 
 * @TODO
 * the 802.15.4 address contains 20 byte at most. this is too complicated for 
 * research using. however, we still adopt this design. attention that we only
 * use 4 byte address in current implementation (20070130)  
 ****************************************************************************/ 

uint16 mac_getshortpan( TOpenAddress * addr )
{
	uint16 * mem = (uint16 *)addr;
	return (* mem);
}

uint16 mac_getshortid( TOpenAddress * addr )
{
	uint16 * mem = (uint16 *)((char*)addr+2);
	return (* mem);
}

void  mac_setshortpan( TOpenAddress * addr, uint16 pan )
{
	uint16 * mem = (uint16 *)addr;
	*mem = pan;
}

void  mac_setshortid( TOpenAddress * addr, uint16 id )
{
	uint16 * mem = (uint16 *)((char*)addr+2);
	*mem = id;
}

/*****************************************************************************
 * Frame Manipulations
 * assume 802.15.4 specification frame.
 ****************************************************************************/
 
TOpenFrame * opf_init( char * buf, uint8 size )
{
	TOpenFrame * opf;
	assert( size <= OPF_FRAME_SIZE );
	opf = (TOpenFrame *)buf;
	opf->length = 0x7F & size;
	return opf;
}

/* refer to the specification of 802.15.4, page 112 */
uint8 opf_type( char * buf )
{
	uint8 type = (opf_control(buf) & 0x03);
	return type;
}

uint8 opf_length( char * buf )
{
	return buf[0] & 0x7F;
}

uint16 opf_control( char * buf )
{  
	uint16 * addr = (uint16 *)(buf + 1);
	return *addr;
}

uint8 opf_seqid( char * buf )
{
	return buf[3];
}

uint16 opf_addrfrom( char * buf )
{
	uint16 * addr = (uint16 *)(buf + 4);
	return *addr;
}

uint16 opf_addrto( char * buf )
{
	uint16 * addr = (uint16 *)(buf + 6);
	return *addr;
}

uint8 opf_command( char * buf )
{
	return buf[8];
}

void opf_settype( char * buf, uint8 type )
{
	buf[2] |= 0x03;
}

void opf_setlength( char * buf, uint8 length )
{
	buf[0] = length & 0x7F;
}

void opf_setaddrfrom( char * buf, uint16 addrfrom )
{
	* opf_addrfrom_addr(buf) = addrfrom; 
}

void opf_setaddrto( char * buf, uint16 addrto )
{
	* opf_addrto_addr(buf) = addrto; 
}

char * opf_length_addr( char * buf )
{
	return buf;
}

char * opf_control_addr( char * buf )
{
	return buf + 1;
}

char * opf_seqid_addr( char * buf )
{
	return buf + 3;
}

char * opf_addrfrom_addr( char * buf )
{
	return buf + 4;
}

char * opf_addrto_addr( char * buf )
{
	return buf + 6;
}

char * opf_command_addr( char * buf )
{
	return buf + 8;
}

/* payload address of PHY data unit
 * the payload of PHY frame is essentially the MAC layer frame. 
 */
char * opf_psdu( char * buf )
{
	return buf + 1; 
}

/* payload address of MAC frame 
 * the payload of MAC frame is essentially the NET layer packet.
 */
char * opf_msdu( char * buf )
{
	return buf + 8;
}

/* returns the pointer to the TOpenPacket */
char * opf_packet( char * buf )
{
	return buf + 1;
}
