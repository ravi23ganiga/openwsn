#ifndef _SVC_NIO_SESSION_H_4214_ 
#define _SVC_NIO_SESSION_H_4214_ 

/**
 * @file
 * @brief TiNioSession is an data structure to transfer REQUEST/RESPONSE pair between components.
 *
 * @author OpenWSN (openwsn@gmail.com)
 * @author Jiang Ridong (jrd072623@163.com)
 * @since 1.5
 * @see svc_nio_acceptor, svc_nio_dispatch, svc_nio_service  
 * @link TiNioAcceptor, TiNioDispatcher, TiNioService
 * 
 * @history
 * @author OpenWSN on 2010.12.12
 *	- first created
 */
 
#include "svc_configall.h"
#include "../rtl/rtl_framequeue.h"
#include "svc_foundation.h"

/*
 * @status
 * 	interface design done
 */

/**
 * TiNioSession
 * TiNioSession is an data structure to transfer REQUEST/RESPONSE pair between 
 * components. Generally, there's one frame only in the rxque for processing. After 
 * processing, the component can place the result frame into the txque. The result
 * frame count can varies from 0 to N, where N depends on the available space in 
 * the txque.
 */
 
typedef struct{
  TiFrameQueue * rxque;
  TiFrameQueue * txque;
  uint8 option;
}TiNioSession;
 
TiNioSession * nio_session_open( TiNioSession * ses, TiFrameQueue * rxque, TiFrameQueue * txque );
void nio_session_close( TiNioSession * ses );
 
#define nio_session_rxque(ses) (ses->rxque)
#define nio_session_txque(ses) (ses->txque)
 
#define nio_session_front(ses) (fmque_front(ses->rxque))
#define nio_session_popfront(ses) (fmque_popfront(ses->rxque))
#define nio_session_read(ses,frame) (fmque_pushback((ses->txque),(frame)))
#define nio_session_write(ses,frame) (fmque_pushback((ses->txque),(frame)))
#define nio_session_writewith(ses,frame,opt) {fmque_pushback((ses->txque),(frame));ses->option=(opt);}
 
#endif
 