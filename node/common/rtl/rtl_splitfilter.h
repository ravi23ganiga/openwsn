#ifndef _RTL_SPLITFILTER_H_6784_
#define _RTL_SPLITFILTER_H_6784_

#include "rtl_foundation.h"
#include "rtl_iobuf.h"

/*******************************************************************************
 * TiSpliterFilter
 * ���յ������������з�frame����
 * rxlen ��¼��ǰframe�ĳ���
 * �ó�����Զ����ֲ��淶��frame������frame�������ȣ����̣����߳������ж����ֽڣ�
 * ������֮�������frameһ����һ���Ϸ���frame.
 *
 * this split filter can be used with socket stream and uart stream
 *
 * @state
 *	finished. still needs fully testing
 *
 * author zhangwei on 20090526
 *	- first version
 ******************************************************************************/

#define SPLITFILTER_SOF 0x01

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
	uint8 state;
	uintx rxlen;
}TiSplitFilter;

/* attention
 * txbuf is actaully not used in this object. it maybe eliminate in the future
 */

TiSplitFilter *     split_create( uintx rxbufsize, uintx txbufsize );
void 				split_free( TiSplitFilter * split );
TiSplitFilter *     split_construct( void * mem, uintx size, uintx rxbufsize, uintx txbufsize );
void 				split_destroy( TiSplitFilter * split );
TiIoBuffer * 		split_rxbuf( TiSplitFilter * split );
TiIoBuffer * 		split_txbuf( TiSplitFilter * split );
uintx 				split_rxhandle( TiSplitFilter * split, TiIoBuffer * input, TiIoBuffer * output );
uintx               split_txhandle( TiSplitFilter * split, TiIoBuffer * input, TiIoBuffer * output );

#ifdef __cplusplus
}
#endif

#endif
