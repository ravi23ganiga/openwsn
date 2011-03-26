#ifndef _BIT_TRANSCEIVER_H_4892_
#define _BIT_TRANSCEIVER_H_4892_
/*******************************************************************************
 * this file defines the transceiver interface that can be used by the MAC layer
 *
 * @state
 *  developing
 *
 * @history
 * @author zhangwei on 200609
 * @modified by zhangwei on 2010.05.10
 *	- revision
 *
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_iobuf.h"

#define BTRX_MODE_RX            0
#define BTRX_MODE_TX            1
#define BTRX_MODE_SLEEP         2
#define BTRX_MODE_POWERDOWN     3

struct _TiBitTxRxInterface;

typedef uint8  (* TiFunBtrxSend)( struct _TiBitTxRxInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef uint8  (* TiFunBtrxRecv)( struct _TiBitTxRxInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef void   (* TiFunBtrxEvolve)( struct _TiBitTxRxInterface * trx, TiEvent * e );
typedef void   (* TiFunBtrxSwitchToMode)( struct _TiBitTxRxInterface * trx, uint8 mode );
typedef uint8  (* TiFunBtrxIsChannelClear)( struct _TiBitTxRxInterface * trx );

typedef uint8  (* TiFunBtrxSetChannel)( struct _TiBitTxRxInterface * trx, uint8 chn );
typedef void   (* TiFunBtrxSetTxPower)( struct _TiBitTxRxInterface * trx, uint8 power );
typedef uint8  (* TiFunBtrxGetRssi)( struct _TiBitTxRxInterface * trx );

// typedef uint8  (* TiFunBtrxGetLqi)( struct _TiBitTxRxInterface * trx );
// typedef uint8  (* TiFunBtrxSetListener)( struct _TiBitTxRxInterface * trx, TiFunEventHandler listener );

/* _TiBitTxRxInterface
 * bit stream sending/receving transceiver interface
 */

struct _TiBitTxRxInterface{
    void * object;
    TiFunBtrxSend send;
    TiFunBtrxRecv recv;
    TiFunBtrxEvolve evolve;
    TiFunBtrxSwitchToMode switchtomode;
    TiFunBtrxIsChannelClear ischnclear;
    TiFunBtrxSetChannel setchannel;
    TiFunBtrxSetTxPower settxpower;
    TiFunBtrxGetRssi getrssi;
    //TiFunBtrxGetLqi getlqi;
    //TiFunBtrxSetListener;
};

#define TiBitTxRxInterface struct _TiBitTxRxInterface

#endif /* _BIT_TRANSCEIVER_H_4892_ */

