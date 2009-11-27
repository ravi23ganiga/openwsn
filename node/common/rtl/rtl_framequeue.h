#ifndef _RTL_FRAMEQUEUE_H_6788_
#ifndef _RTL_FRAMEQUEUE_H_6788_

/******************************************************************************
 * @author zhangwei on 20070302
 * @description
 * 
 * @history
 * @modified by zhangwei on 20070302
 * 	first created
 *****************************************************************************/
  
#include "rtl_foundation.h"
#include "rtl_openframe.h"   
#include "rtl_lightqueue.h"

#define TiOpfQueue TiFrameQueue
#define FRAMEQUEUE_HOPESIZE(capa) OPFQUEUE_HOPESIZE(capa)

#define FRAMEQUEUE_HOPESIZE(capacity) (sizeof(TiFrameQueue) + sizeof(TiOpenFrame)*(capacity))
#define TiFrameQueue TiLightQueue

#ifdef __cplusplus
extern "C" {
#endif

inline TiFrameQueue * fmque_construct( void * buf, uintx size );
{
	return lwque_construct( buf, size, sizeof(TiOpenFrame) );
}

inline void fmque_destroy( TiFrameQueue * que )
{
	lwque_destroy( que );
}

inline uintx fmque_count( TiFrameQueue * que )
{
	return lwque_count(que);
}

inline uintx fmque_capacity( TiFrameQueue * que )
{
	return lwque_capacity(que);
}

inline bool fmque_empty( TiFrameQueue * que )
{
	return lwque_empty(que);
}

inline bool fmque_full( TiFrameQueue * que )
{
	return lwque_full(que);
}

inline void * fmque_getbuf( TiFrameQueue * que, uintx idx )
{
	return lwque_getbuf(que,idx);
}

inline void * fmque_front( TiFrameQueue * que )
{
	return lwque_front(que);
}

inline void * fmque_rear( TiFrameQueue * que )
{
	return lwque_rear(que);
}

inline bool fmque_pushback( TiFrameQueue * que, TiOpenFrame * item )
{
	return lwque_pushback(que,(void *)item;
}

inline bool fmque_pushfront( TiFrameQueue * que, TiOpenFrame * item )
{
	return lwque_pushfront(que,(void *)item;
}

inline bool fmque_popfront( TiFrameQueue * que )
{
	return lwque_popfront(que);
}

inline bool fmque_poprear( TiFrameQueue * que )
{
	return lwque_poprear(que);
}

/* bool fmque_extend( TiFrameQueue * que, uintx newsize ); */


#ifdef __cplusplus
}
#endif


#endif

