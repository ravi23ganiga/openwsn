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

#include "rtl_configall.h"
#include <string.h>
#ifdef CONFIG_DYNA_MEMORY
#include <stdlib.h>
#endif
#include "rtl_foundation.h"
#include "rtl_textspliter.h"
#include "../hal/hal_debugio.h"
#include "rtl_assert.h"


#ifdef CONFIG_DYNA_MEMORY
inline TiTextSpliter * tspl_create( uintx pktsize )
{
	uint16 memsize = TSPL_HOPESIZE(pktsize);
	TiTextSpliter * split = (TiTextSpliter *)malloc( memsize );
	tspl_construct( split, memsize );
	return split;
}
#endif

#ifdef CONFIG_DYNA_MEMORY
inline void tspl_free( TiTextSpliter * split )
{
	tspl_destroy( split );
	free( split );
}
#endif

#ifdef TSPL_VERSION20
TiTextSpliter * tspl_construct( void * mem, uint16 size )
{
	rtl_assert( sizeof(TiTextSpliter) <= size );
	memset( mem, 0x00, size );
	TiTextSpliter * split = (TiTextSpliter *)(mem);
	
	split->state = SPLITER_STATE_WAITFOR_START;
	split->tmphead = 0;
	split->tmprear = 0;

	return split;
}

void tspl_clear( TiTextSpliter * split )
{
	//memset( split, 0x00, )
	//unsigned char buf[TSPL_RXBUF_SIZE];
	split->state = 0;
	split->tmphead = 0;
	split->tmprear = 0;

}
#endif

#ifdef TSPL_VERSION30
TiTextSpliter * tspl_construct( void * mem, uint16 size )
{
	rtl_assert( sizeof(TiTextSpliter) <= size );
	memset( mem, 0x00, size );
	TiTextSpliter * split = (TiTextSpliter *)(mem);

	split->state = SPLITER_STATE_1;

	split->txbuf = iobuf_construct( &split->txmem[0], sizeof(split->txmem) );
	split->rxbuf = iobuf_construct( &split->rxmem[0], sizeof(split->rxmem) );
	return split;
}

void tspl_clear( TiTextSpliter * split )
{

	split->state = 0;

}
#endif

void tspl_destroy( TiTextSpliter * split )
{
	iobuf_clear(split->rxbuf);
	iobuf_clear(split->txbuf);
}


/* tspl_read
 * This function should be called every time new data received. The new data is indicate 
 * by the parameter "input". If the rxhandle can find an entire frame/packet inside the
 * internal buffer, then it will move the frame/packet into "output" buffer. The return
 * value is the length of the frame/packet in the output buffer. If there's no packet/frame
 * found, then 0 will be returned.
 *
 * Example
 *   char buf[128];
 *   char packet[128]
 *   while (1)
 *   {
 *     len = uart_read( g_uart, buf, 128, 0x00 );
 *     if (len > 0)
 *     { 
 *       if (tspl_rxhandle(split, buf, len, packet, 128) > 0)
 *       {
 *          push the packet into a queue for later processing
 *       }
 *     }
 *   }
 *
 * In another thread, you can visit the queue to get the incoming packets.
 */
#ifdef TSPL_VERSION10
uint16 tspl_rxhandle( TiTextSpliter * split, char * input, uint8 len, char * output, uint16 size )
{
	uint16 i = 0;
	uint16 pac_head = 0;
	uint16 pac_end = 0;
	uint16 len, size;

	len = iobuf_length(input);//�����ݳ���

	//��input����splitbuf������
	split->tmphead = split->tmprear;
	split->tmprear += len ;
	if(tmprear>=TSPL_RXBUF_SIZE)
	{
		split->tmphead = 0;
		split->tmprear = 0;
		split->tmprear += len ;
		memmove(&split->buf[split->tmphead], iobuf_data(input), len);
	}
	else
	{
		memmove(&split->buf[split->tmphead], iobuf_data(input), len);
	}

	switch (split->state)
	{
		case SPLITER_STATE_WAITFOR_START://״̬1

			for(i=0;i<split->tmprear;i++)//��ͷѰ��PAC_START_FLAG��
			{
				if(split->buf[i]==PAC_START_FLAG)
				{
					pac_head = i+1;
					break;
				}
			}

			if(pac_head==0)//û���յ���PAC_START_FLAG�����buf
			{
				split->tmphead = 0;
				split->tmprear = 0;
				return 0;
			}
			else// if( pac_head > 0 )//�յ���PAC_START_FLAG
			{
				for(i=pac_head;i<split->tmprear;i++)//����յ�PAC_START_FLAG����Ѱ�Ҵ��ֽ������Ƿ����PAC_EDN_FLAG��
				{
					if(split->buf[i]==PAC_END_FLAG)
					{
						pac_end = i;
						break;
					}
				}

				if(pac_end>0)//�յ�PAC_END_FLAG
				{
					//memmove(output, &split->buf[pac_head], pac_end-pac_head);//ȡ��packet
					iobuf_write(output, &split->buf[pac_head], pac_end-pac_head);
					memmove(&split->buf[0], &split->buf[pac_end+1], split->tmprear-pac_end-1);//���������������������ǰ��
					split->tmprear = split->tmprear-pac_end-1;
					split->tmphead = 0;
					return pac_end-pac_head;//�´��Խ����case
				}
				else //�յ���PAC_START_FLAG����û��PAC_END_FLAG
				{
					memmove(&split->buf[0], &split->buf[pac_head], split->tmprear-pac_head);//���������������������ǰ��
					split->tmprear = split->tmprear-pac_head;
					split->tmphead = 0;
					split->state = SPLITER_STATE_WAITFOR_END;
					return 0;//�´β������case
				}

			}
			break;

		case SPLITER_STATE_WAITFOR_END://����յ���PAC_START_FLAG��ȴû�յ�PAC_END_FLAG

			for(i=split->tmphead;i<split->tmprear;i++)//ֱ��Ѱ��PAC_END_FLAG��
			{
				if(split->buf[i] == PAC_END_FLAG)
				{
					pac_end = i;
					break;
				}
			}

			if(pac_end==0)//���û���ҵ�
			{
				return 0;
			}
			else
			{
				//memmove(output, &split->buf[0], pac_end);//ȡ��packet
				iobuf_write(output, &split->buf[0], pac_end);
				memmove(&split->buf[0], &split->buf[pac_end+1], split->tmprear-pac_end-1);//���������������������ǰ��
				split->tmprear = split->tmprear-pac_end-1;
				split->tmphead = 0;
				split->state = SPLITER_STATE_WAITFOR_START;
				return pac_end;
			}

			break;

		default:
			return 0;
			break;
	}
}
#endif

#ifdef TSPL_VERSION10
uint16 tspl_txhandle( TiTextSpliter * split, char * input, uint16 len, char * output, uint16 size )
{
	rtl_assert( size > len+2 );
	output[0] = PAC_START_FLAG;
	memmove( &output[1], input, len );
	output[len] = PAC_END_FLAG;
	return len+2;

}
#endif

#ifdef TSPL_VERSION20
/*
 * ˼·:
 * 1.����״̬��һΪSPLITER_STATE_WAITFOR_START����һΪSPLITER_STATE_WAITFOR_END;
 * 2.������ǣ�һΪpac_head��ʾ֡��ʼ���PAC_START_FLAG��buf�����е��±꣬��һΪpac_rear��ʾ
 * ֡�������PAC_END_FLAG��buf�����е��±ꡣ
 * 3.��������ǣ�tmphead��tmprear���input����������buf�е���βλ�á�
 *
 * input��ʾ�����յ��������ݣ����bufû���㹻�ռ����������������buf��֮��input���ݷŵ��ڲ�bufβ����
 * 1����SPLITER_STATE_WAITFOR_START״̬ʱ��
 * 	��buf�����Ѱ��PAC_START_FLAG��
 * 		����Ҳ��������buf������0��
 * 		����ҵ�����ȡ֡���ȣ�
 * 			�жϵ�ǰbuf���Ƿ��������֡���������Ƿ����֡���ȣ���
 * 				�������������֡������ʣ�����ݡ�
 * 				���������������0������PAC_START_FLAG���������ݣ��ı�״̬Ϊ��end��
 *
 * 2����SPLITER_STATE_WAITFOR_END״̬ʱ�������������һ������󣩣�
 * 	 �жϵ�ǰbuf���Ƿ��������֡���������Ƿ����֡���ȣ���
 * 		�������������֡������ʣ�����ݣ��ı�״̬Ϊ��start��
 * 		���������������0��
 */
uint16 tspl_rxhandle( TiTextSpliter * split, TiIoBuf * input, TiIoBuf * output )
{
	uint16 i = 0;
	uint16 pac_head = 0;
	uint16 pac_end = 0;
	uint16 len;
	bool judge=false;

	len = iobuf_length(input);//�����ݳ���

	//��input����splitbuf������
	split->tmphead = split->tmprear;
	split->tmprear += len ;
	if(split->tmprear>=TSPL_RXBUF_SIZE)
	{
		split->tmphead = 0;
		split->tmprear = 0;
		split->tmprear += len ;
		memmove(&split->buf[split->tmphead], iobuf_data(input), len);
		split->state = SPLITER_STATE_WAITFOR_START;
		split->count = 0;
	}
	else
	{
		memmove(&split->buf[split->tmphead], iobuf_data(input), len);
	}

	switch (split->state)
	{
		case SPLITER_STATE_WAITFOR_START://״̬1

			for(i=0;i<split->tmprear;i++)//��ͷѰ��PAC_START_FLAG��
			{
				if(split->buf[i]==PAC_START_FLAG)
				{
					pac_head = i+1;
					break;
				}
			}

			if(pac_head==0)//û���յ���PAC_START_FLAG�����buf
			{
				split->tmphead = 0;
				split->tmprear = 0;
				return 0;
			}
			else// if( pac_head > 0 )//�յ���PAC_START_FLAG
			{
				split->count = split->buf[pac_head];
				pac_head = pac_head + 2;//�����ݴ���ʼ
				judge = ((split->tmprear-pac_head)>=split->count);//�ж������Ƿ�֡

				if(judge)//��֡
				{
					//memmove(output, &split->buf[pac_head], pac_end-pac_head);//ȡ��packet
					iobuf_write(output, &split->buf[pac_head], split->count);
					pac_end = pac_head + split->count;
					memmove(&split->buf[0], &split->buf[pac_end], split->tmprear-pac_end);//���������������������ǰ��
					split->tmprear = split->tmprear-pac_end;
					split->tmphead = 0;
					return split->count;//�´��Խ����case
				}
				else //�յ���PAC_START_FLAG����û��PAC_END_FLAG
				{
					memmove(&split->buf[0], &split->buf[pac_head], split->tmprear-pac_head);//���������������������ǰ��
					split->tmprear = split->tmprear-pac_head;
					split->tmphead = 0;
					split->state = SPLITER_STATE_WAITFOR_END;
					return 0;//�´β������case
				}

			}
			break;

		case SPLITER_STATE_WAITFOR_END://����յ���PAC_START_FLAG����֡����

			judge = (split->count>split->tmprear);

			if(judge)//���û���ҵ�
			{
				return 0;
			}
			else
			{
				//memmove(output, &split->buf[0], pac_end);//ȡ��packet
				iobuf_write(output, &split->buf[0], split->count);
				memmove(&split->buf[0], &split->buf[split->count], split->tmprear-split->count);//���������������������ǰ��
				split->tmprear = split->tmprear-split->count;
				split->tmphead = 0;
				split->state = SPLITER_STATE_WAITFOR_START;
				return split->count;
			}

			break;

		default:
			return 0;
			break;
	}
}
#endif

#ifdef TSPL_VERSION20
uint16 tspl_txhandle( TiTextSpliter * split, TiIoBuf * input, TiIoBuf * output )
{
	// todo tspl_rxhandle
	uint16 count = iobuf_copyfrom( output, input );
	iobuf_clear( input );
	return count;
}
#endif

#ifdef TSPL_VERSION30
uint16 tspl_rxhandle( TiTextSpliter * split, TiIoBuf * input, TiIoBuf * output, uint8 * success )
{
	char * data = iobuf_data(input);
	uint16 t_len = iobuf_length(input);
	uint16 count = 0;
	*success = 0;

	while(count<t_len)
	{

		switch(split->state)
		{
			case SPLITER_STATE_1://����ǿ�ʼ��־�����״̬2�������ڴ�ѭ��Ѱ�ҿ�ʼ��־��
				if( data[count] == PAC_START_FLAG )
				{
					split->state = SPLITER_STATE_2;
				}
				break;
			case SPLITER_STATE_2:
				split->exp_len = data[count];//��ʹ����0x55��Ҳ��Ϊ�ǳ��ȶ��ǿ�ʼ��־��//���16λ֡���ȵĸ�ֵ���������͸�λ��
				split->state = SPLITER_STATE_3;
				break;
			case SPLITER_STATE_3:
				split->exp_len = ( ( split->exp_len<<8 )|data[count] );
				split->state = SPLITER_STATE_4;
				break;
			case SPLITER_STATE_4:

				/*if( data[count] == PAC_END_FLAG )//����ǽ�β��־�����֡�����Ƿ���ȷ�����򷵻�֡
				{
					if(iobuf_length(split->buf) == split->exp_len)
					{
						iobuf_copyto( split->buf, output );
						iobuf_clear( split->buf );
						split->state = SPLITER_STATE_1;//�ҵ��㹻�����ݣ����س�ʼ״̬
						return count;//ֱ�ӷ��أ�Ϊ�˷�ֹm_rxbuf��������������֡,��whileѭ���������У�����ɵ�һ��֡���ߵ�
					}
					else//����ǰһ��֡
					{
						iobuf_clear( split->buf );
						split->state = SPLITER_STATE_1;
					}
				}*/

				if( data[count] == PAC_START_FLAG )//����ǿ�ʼ��־
				{
					if(iobuf_length(split->rxbuf) == split->exp_len)//���֡�����Ƿ���ȷ
					{
						iobuf_copyto( split->rxbuf, output );
						iobuf_clear( split->rxbuf );
						split->state = SPLITER_STATE_2;
						*success = 1;
						return count;//Ϊ�˷�ֹm_rxbuf��������������֡��ɵĵ�һ��֡���ߵ�
					}
					else//��Ͷ���
					{
						iobuf_clear( split->rxbuf );
						split->state = 2;
					}
				}
				else if(iobuf_full(split->rxbuf))//������֡�����
				{
					iobuf_clear( split->rxbuf );
					split->state = SPLITER_STATE_1;
				}
				else//��������ݣ��򱣴�
				{
					iobuf_pushbyte(split->rxbuf, data[count]);
					if(iobuf_length(split->rxbuf) == split->exp_len)
					{
						iobuf_copyto( split->rxbuf, output );
						iobuf_clear( split->rxbuf );
						split->state = SPLITER_STATE_1;//�ҵ��㹻�����ݣ����س�ʼ״̬
						*success = 1;
						return count;//Ϊ�˷�ֹm_rxbuf��������������֡,��whileѭ���������У�����ɵ�һ��֡���ߵ�
					}
				}
				break;
			default:
				break;
		}
		count++;
	}
	return count;
}
#endif

#ifdef TSPL_VERSION30
uint16 tspl_txhandle( TiTextSpliter * split, TiIoBuf * input, TiIoBuf * output )
{//����ͷ���ͳ���
	uint16 count;
	count = iobuf_pushbyte(split->txbuf, PAC_START_FLAG);

	count = count + iobuf_pushbyte(split->txbuf, ((iobuf_length(input)&0xFF00)>>8) );//�ȴ����ȵĸ�λ

	count = count + iobuf_pushbyte(split->txbuf, iobuf_length(input)&0x00FF );//��λ

	count = count + iobuf_pushback(split->txbuf, iobuf_ptr(input), iobuf_length(input));

	//count = iobuf_pushbyte(split->buf, PAC_END_FLAG);


	if( (count>0) && (count<iobuf_size(output)) )
	{
		iobuf_append( output, split->txbuf );
		iobuf_clear( split->txbuf );
	}
	else
	{
		iobuf_clear(split->txbuf);
		count = 0;
	}
	return count;
}
#endif
