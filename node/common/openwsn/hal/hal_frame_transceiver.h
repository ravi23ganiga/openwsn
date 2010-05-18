#ifndef _FRAME_TRANSCEIVER_H_4892_
#define _FRAME_TRANSCEIVER_H_4892_
/*******************************************************************************
 * this file defines the transceiver interface that can be used by the MAC layer
 *
 * @state
 *  released. compile passed. need further developing
 *
 * @history
 * @author zhangwei on 200609
 * @modified by zhangwei on 20090718
 *	- revision
 * @modified by zhangwei on 20090819
 *	- add default settings of panid, local address and pan id in cc2420_open()
 * @modified by zhangwei on 20090927
 *	- revision. divide all the interface functions into serveral group according 
 *    to their functions.
 * @modified by zhangwei on 2010.05.10
 *  - revision. 
 *
 ******************************************************************************/

#include "hal_configall.h"
#include "hal_foundation.h"
#include "../rtl/rtl_iobuf.h"

#define FTRX_MODE_RX            0
#define FTRX_MODE_TX            1
#define FTRX_MODE_SLEEP         2
#define FTRX_MODE_POWERDOWN     3

struct _TiFrameTxRxInterface;

typedef uint8  (* TiFunFtrxSend)( struct _TiFrameTxRxInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef uint8  (* TiFunFtrxRecv)( struct _TiFrameTxRxInterface * trx, TiIoBuf * iobuf, uint8 option );
typedef void   (* TiFunFtrxEvolve)( struct _TiFrameTxRxInterface * trx, TiEvent * e );
typedef void   (* TiFunFtrxSwitchToMode)( struct _TiFrameTxRxInterface * trx, uint8 mode );
typedef uint8  (* TiFunFtrxIsChannelClear)( struct _TiFrameTxRxInterface * trx );
typedef uint8  (* TiFunFtrxEnableAutoAck)( struct _TiFrameTxRxInterface * trx );
typedef uint8  (* TiFunFtrxDisableAutoAck)( struct _TiFrameTxRxInterface * trx );
typedef uint8  (* TiFunFtrxEnableAddrDecode)( struct _TiFrameTxRxInterface * trx );
typedef uint8  (* TiFunFtrxDisableAddrDecode)( struct _TiFrameTxRxInterface * trx );

typedef uint8  (* TiFunFtrxSetChannel)( struct _TiFrameTxRxInterface * trx, uint8 chn );
typedef uint8  (* TiFunFtrxSetPanId)( struct _TiFrameTxRxInterface * trx, uint8 panid );
typedef uint16 (* TiFunFtrxGetPanId)( struct _TiFrameTxRxInterface * trx );
typedef uint8  (* TiFunFtrxSetShortAddress)( struct _TiFrameTxRxInterface * trx, uint16 addr );
typedef uint16 (* TiFunFtrxGetShortAddress)( struct _TiFrameTxRxInterface * trx );
typedef void   (* TiFunFtrxSetTxPower)( struct _TiFrameTxRxInterface * trx, uint8 power );
typedef uint8  (* TiFunFtrxGetRssi)( struct _TiFrameTxRxInterface * trx );

// typedef uint8  (* TiFunFtrxGetLqi)( struct _TiFrameTxRxInterface * trx );
// typedef uint8  (* TiFunFtrxSetListener)( struct _TiFrameTxRxInterface * trx, TiFunEventHandler listener );


struct _TiFrameTxRxInterface{
    void * object;
    TiFunFtrxSend send;
    TiFunFtrxRecv recv;
    TiFunFtrxEvolve evolve;
    TiFunFtrxSwitchToMode switchtomode;
    TiFunFtrxIsChannelClear ischnclear;
    TiFunFtrxEnableAutoAck enable_autoack;
    TiFunFtrxDisableAutoAck disable_autoack;
    TiFunFtrxEnableAddrDecode enable_addrdecode;
    TiFunFtrxDisableAddrDecode disable_addrdecode;
    TiFunFtrxSetChannel setchannel;
    TiFunFtrxSetPanId setpanid;
    TiFunFtrxGetPanId getpanid;
    TiFunFtrxSetShortAddress setshortaddress;
    TiFunFtrxGetShortAddress getshortaddress;
    TiFunFtrxSetTxPower settxpower;
    TiFunFtrxGetRssi getrssi;
    //TiFunFtrxGetLqi getlqi;
    //TiFunFtrxSetListener;
};

#define TiFrameTxRxInterface struct _TiFrameTxRxInterface

#endif  /* _FRAME_TRANSCEIVER_H_4892_ */
