//ģ�����ƣ�rtl_textcodec(ANSI C, no depends)

#define CONFIG_SPLIT_BUFFER_SIZE 500
#define CONFIG_TEXTPAC_SIZE 300

#define PAC_START_FLAG 0x01
#define PAC_END_FLAG 0x02

#define SPLITER_STATE_WAITFOR_START 0
#define SPLITER_STATE_WAITFOR_END 0

typedef struct{
	uint8 state;
	char splitbuf[CONFIG_SPLIT_BUFFER_SIZE];
	uint8 len;
	uint8 option;
	uint8 textpac_len;
	char  textpac[CONFIG_TEXTPAC_SIZE]
}TiSteamSpliter;

// RX stream spliting.
// server��socket server or uart server��ֻҪ�յ����ݣ�
// �Ϳ��԰ѵ���split_rxhandle����֮��
// Parameter-
// input: 	server��socket server or uart server���յ�������
// len_in: 	�������ݵĳ���
// output:	������packet, �Ѿ��޳�Packet��START��END�ַ�
// len_out: 	�����������С��output buffer����󵽿��Է������packet
// return:	0 when not found a packet; >0 data length inside output buffer
uint16 split_rxhandle( TiStreamSpliter * split, char * input, uint16 len_in, char * output, uint16 len_out );

// TX stream assemble
// Parameter-
// input: �����packet��Ӧ��һ��������packet
// output: ��װ��START/END����ַ�֮���packet
uint16 split_txhandle( TiStreamSpliter * split, char * input, uint16 len, char * output, uint16 size );
