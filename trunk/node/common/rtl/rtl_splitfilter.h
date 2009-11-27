#ifndef _RTL_SPLITFILTER_H_6784_
#define _RTL_SPLITFILTER_H_6784_

#include "rtl_foundation.h"
#include "rtl_iobuf.h"

/*******************************************************************************
 * TiSpliterFilter
 * 对收到的数据流进行分frame处理
 * rxlen 记录当前frame的长度
 * 该程序可自动发现不规范的frame，例如frame不够长度（过短）或者超长（有多余字节）
 * 并丢弃之。输出的frame一定是一个合法的frame.
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
