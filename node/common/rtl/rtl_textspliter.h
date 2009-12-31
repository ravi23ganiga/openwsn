/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
#ifndef _RTL_IOBUF_H_2857_
#define _RTL_IOBUF_H_2857_

/* attention
 * - avoid the very long packet lead to deadlock in this object. The reason is:
 *   tspl_read() can only return a packet when the input buffer size is big enough 
 *   to hold an entire packet. However, if the input buffer isn't big enough, then
 *   the packet will continue stay inside the spliter and it will be never read out.
 */

//ģ�����ƣ�rtl_textspliter(ANSI C, no depends)

#define CONFIG_SPLIT_BUFFER_SIZE 500

#define PAC_START_FLAG 0x01
#define PAC_END_FLAG 0x99

#define SPLITER_STATE_WAITFOR_START 0x01
#define SPLITER_STATE_WAITFOR_END 0x02

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	char  buf[CONFIG_SPLIT_BUFFER_SIZE];
	uint8 state;
	uint8 tmphead;
	uint8 tmprear;
}TiTextSpliter;

TiTextSpliter * tspl_construct( void * mem, uint16 size );
TiTextSpliter * tspl_destroy( TiTextSpliter * split );


// RX stream spliting.
// server��socket server or uart server��ֻҪ�յ����ݣ�
// �Ϳ��԰ѵ���split_rxhandle����֮��
// Parameter-
// input: 	server��socket server or uart server���յ�������
// len: 	�������ݵĳ���
// output:	������packet, �Ѿ��޳�Packet��START��END�ַ�
// size: 	�����������С��output buffer����󵽿��Է������packet
// return:	0 when not found a packet; >0 data length inside output buffer
uint16 tspl_read( TiTextSpliter * split, char * input, uint8 len, char * output, uint16 size );

// TX stream assemble
// Parameter-
// input: �����packet��Ӧ��һ��������packet
// output: ��װ��START/END����ַ�֮���packet
uint16 tspl_write( TiTextSpliter * split, char * input, uint16 len, char * output, uint16 size );

#ifdef __cplusplus
}
#endif

#endif
