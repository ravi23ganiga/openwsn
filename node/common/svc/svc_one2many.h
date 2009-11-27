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
