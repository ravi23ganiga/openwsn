#include "../common/rtl/rtl_configall.h"
#include <string.h>
#include "../common/hal/hal_debugio.h"
#include "../common/rtl/rtl_foundation.h"
#include "svc_streamspliter.h"

����ֵ����output buffer�����ݵĳ���
uint16 split_rxhandle( TiStreamSpliter * split, char * input, uint16 len, char * output, uint16 size )
{
	uint16 i = 0;
	uint16 pac_head = 0;
	uint16 pac_end = 0;

	=> please change the variable name from option to state
	=> ��Ȼ��,������Ҳ��С״̬����˼��

	switch (split->state)
	{
	case SPLITER_STATE_WAITFOR_START:
	case SPLITER_STATE_WAITFOR_END:
	}

	=> ûϸ����γ���,����������·,��switch+��ǰ״̬�жϵ�˼������ȷ��

	switch (split->option)
	{
	case 0x00://�����δ�յ���PAC_START_FLAG
		for(i=0;i<len_in;i++)//�����յ����ֽ�ֱ���õ�PAC_START_FLAG��
		{
			if(input[i]==PAC_START_FLAG)
			{
				split->option = 0x01;
				pac_head = i+1;
				break;
			}
		}

		for(i=pac_head;i<len_in;i++)//����յ�PAC_START_FLAG����Ѱ�Ҵ��ֽ������Ƿ����PAC_EDN_FLAG��
		{
			if(input[i]==PAC_END_FLAG)
			{
				split->option = 0x00;
				pac_end = i-1;
				break;
			}
		}
		if(pac_head==0)//û���յ���PAC_START_FLAG������

		else if(pac_head<pac_end)//�յ�PAC_START_FLAG��PAC_END_FLAG
		{
			memmove(output, &input[pac_head], pac_end-pac_head+1);//ֱ��ȡ��packet
			memmove(input, &input[pac_end+1], len_in-pac_end);//��ʣ���ֽ�Ǩ�ƣ��������
			split->textpac_len = pac_end-pac_head+1;
			// �ص㣺
			// ��ͬʱ�յ�PAC_START_FLAG��PAC_END_FLAG��ʣ����ֽ������账��
			// �ʵ�������split_rxhandle�������������output����ֻ��һ����
			// ��ɰ��ᱻ�°�����
			// �˴���split�ṹ��ret��Ա����֤���ص������µİ���С��
			// ���ƺ����׵��Ǵ���output����ʱ����ͬʱ�޸�ret��

			split_rxhandle( split, input, len_in-pac_end-1, output, len_out );		
		}
		else //�յ���PAC_START_FLAG����û��PAC_END_FLAG,ȫ���ֽ����������
		{
			memmove(split->buf, &input[pac_head], len_in-pac_head);//���޸�
		}

		return split->ret;
		break;

	case 0x01://����յ���PAC_START_FLAG��ȴû�յ�PAC_END_FLAG
		for(i=0;i<len_in;i++)//ֱ��Ѱ��PAC_END_FLAG��
		{
			if(input[i]==PAC_END_FLAG)
			{
				split->option = 0x00;
				pac_end = i-1;
				break;
			}
		}
		if(pac_end==0)//���û���ҵ�
			memmove(split->buf, &input[0], len_in);//ȫ���������
		else 
		{
			memmove(split->buf, &input[0], pac_end+1);//���ּ������
			memmove(output, split->buf, queue_end);//ȡ��packet
			split->textpac_len = queue_end;
			split_rxhandle( split, input, len_in-pac_end-1, output, len_out );		
						
		}

		return split->textpac_len;
		break;
	
	
	}
}



uint16 split_txhandle( TiStreamSpliter * split, char * input, uint16 len, char * output, uint16 size )
{
	ֻҪ��inputǰ��ƴ��start/stop�ַ�Ȼ��move��output��ȥ����
}
