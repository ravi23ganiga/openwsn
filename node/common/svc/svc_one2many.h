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

#ifndef _SVC_ONE2MANY_H_7244_
#define _SVC_ONE2MANY_H_7244_

/*�������룺
 *	TiOne2Many���������ͣ�һΪGATE����һΪSENSOR��
 *	����GATE��˵��������Ϊ��ʱ����DATA_REQUEST frame���õ�svc_timer��mac�����ض���ʽ��opf���󣩣�Ȼ��ȴ�������S�ڵ�����
 *	���õ�mac ��opf �����TiIoBuf���� �����ȴ�һ��ʱ���ͳ�ƺã�Ȼ�󷢵����ڣ��õ�UART����
 *	����״̬���ĸ���IDLE��WIO_SENDING��WIO_RECEIVING��SIO_SENDING
 *
 *	����SENSOR, ������Ϊ�ȴ���Ȼ���յ�DATA_REQUEST frame��opf��mac��ʱ�����hal_remoteled ģ��Ĵ����ȡ���ݣ�Ȼ���ͻ�G��
 *	����״̬��������WIO_RECEIVING��DATA_COLLECT_AND_SEND
 *
 */

#define GATEWAYTYPE 1
#define SENSORTYPE 0

#define TIMEISUP 1
#define TIMEISNOTUP 0

#define CONFIG_SIOBUF_SIZE 88
#define CONFIG_OPF_SIZE 127

#define MAC_GATE_PANID 0xFF
#define MAC_GATE_LOCAL 0x99
#define MAC_GATE_REMOTE 0xFF
/* TiOne2Many
 * an data collection service. 
 * 
 * state                    indicate the current state of state machine inside this service.
 * mac                      medium access protocol object
 * timer                    timer 
 * txbuf                    temporarily holding the frame received 
 * rxbuf                    holding the frame to be sent
 */
typedef struct{
	uint8 state;
	TiUartAdapter * uart;
	uint8 seqid;
	TiAloha * mac;
	//TiTimer * timer;
	//TiTimer * vti;
	//TiIoBuf * siobuf;
	//TiOpenFrame * txbuf;
	//TiOpenFrame * rxbuf;
    //TiOpenFrame * opf;
    
	//char siobuf_memory[IOBUF_HOPESIZE(CONFIG_SIOBUF_SIZE)];
	//char opf_memory[OPF_HOPESIZE(CONFIG_OPF_SIZE)];
	char txbuf_mem[ OPF_HOPESIZE(128) ];
	char rxbuf_mem[ OPF_HOPESIZE(128) ]; 
}TiOne2Many;

/* one2many_construct
 * It construct a TiOne2Many service on the specified memory block.
 */
TiOne2Many * one2many_construct( void * mem, uint16 memsize );
void one2many_destroy( TiOne2Many * o2m );
TiOne2Many * one2many_open( TiOne2Many * o2m, TiAloha * mac );
void one2many_close( TiOne2Many * o2m );

/* evolve the state machine by accepting events inputed */
void one2many_evolve( void * svcptr, TiEvent * e );

/* one2many_send()
 * Broadcast the REQUEST to all the neighbor nodes. The REQUEST should placed into 
 * buf, and "len" is the length of the REQUEST.
 * 
 * REQUEST format:
 *  [type 1B][2B receiver address][2B sender address][lumnance 2B]
 *  type = 0x01 indicates this is a REQUEST
 */
uint8 one2many_send( TiOne2Many * svc, TiIoBuf * buf, uint8 option );
uint8 one2many_broadcast( TiOne2Many * svc, TiIoBuf * buf, uint8 option );


/* one2many_recv()
 * Try to receive RESPONSE replied from neighbor nodes. The RESPONSE will be put 
 * into "buf". Attention the "size" must be larger enough to hold the RESPONSE.
 * buf, and "len" is the length of the REQUEST.
 * 
 * REQUEST format:
 *  [type 1B][2B receiver address][2B sender address][lumnance 2B]
 *  type = 0x02 indicates this is a RESPONSE
 */
uint8 one2many_recv( TiOne2Many * svc, TiIoBuf * buf, uint8 option );


#endif /* _SVC_ONE2MANY_H_7244_ */
