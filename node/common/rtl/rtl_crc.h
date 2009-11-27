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

#ifndef _RTL_CRC_H_4278_
#define _RTL_CRC_H_4278_

/*****************************************************************************
 * @author zhangwei on 2005-07-19
 * @note: Interface file CRC check module.
 *	thanks Ruijie to give the original source code (first version). 
 * 
 * @history
 * 2005-07-19 first created by zhangwei
 * @modified by zhangwei on 20061030
 * revised and released as part of RTL
 ****************************************************************************/

/*  CRCProduce
*********************************************************************************************************
* Description: ����CRCУ���ֶ�.
* Arguments  : buf,  ֡����������ָ�룬ָ���һ����У����ַ�
*                     number, ��У��֡���ַ�����
* Returns    :   fcsOut, CRCУ����
*                    ���⣬������������buf��������ĩβ�����CRCУ����  
* Complete date  : 2005.5.20
                   Modified by xxx at xxxx.xx.xx:
*********************************************************************************************************
*/
extern unsigned short crc_produce(unsigned char *buf, unsigned short number);

/*  CRCCheck
*********************************************************************************************************
* Description: CRCУ�麯��.
* Arguments  : buf,  ֡����������ָ�룬ָ���һ����У����ַ�
*                     number, ��У��֡���ַ����������԰���CRCУ����
* Returns    :   fcsOut, CRCУ����
*                    ���⣬������֡����CRCУ����ʱ��CRCУ����ȷʱ���fcsOutΪ0  
* Complete date  : 2005.5.20
                   Modified by xxx at xxxx.xx.xx:
*********************************************************************************************************
*/
extern unsigned short crc_check(unsigned char *buf, unsigned short number);

#endif