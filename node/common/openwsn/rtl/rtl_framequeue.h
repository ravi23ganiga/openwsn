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
#ifndef _RTL_FRAMEQUEUE_H_6788_
#ifndef _RTL_FRAMEQUEUE_H_6788_

/*******************************************************************************
 * @author zhangwei on 2007.03.02
 * @description
 *  Frame queue. Each item in the queue is an TiFrame object.
 * 
 * @status
 *  - Finished developing. Not fully tested yet. 
 * 
 * @history
 * @modified by zhangwei on 2007.03.02
 * 	- first created
 * @modified by zhangwei on 2010.08.24
 *  - replace TiOpenFrame with TiFrame.  The TiFrame can be regarded as an upgraded
 *    version of TiOpenFrame. The most significant feature of TiFrame is that it
 *    support multilayer frame/packet operations.
 ******************************************************************************/
  
#include "rtl_foundation.h"
//#include "rtl_openframe.h"   
#include "rtl_frame.h"   
#include "rtl_lightqueue.h"

//#define TiOpfQueue TiFrameQueue
//#define OPFQUEUE_HOPESIZE(capa) FRAMEQUEUE_HOPESIZE(capa) 

#define FRAMEQUEUE_HOPESIZE(capacity) (sizeof(TiFrameQueue) + sizeof(TiFrame)*(capacity))
#define TiFrameQueue TiLightQueue

#ifdef __cplusplus
extern "C" {
#endif

inline TiFrameQueue * fmque_construct( void * buf, uintx size );
{
	return lwque_construct( buf, size, sizeof(TiFrame) );
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

inline bool fmque_pushback( TiFrameQueue * que, TiFrame * item )
{
	return lwque_pushback(que,(void *)item);
}

inline bool fmque_pushfront( TiFrameQueue * que, TiFrame * item )
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

/* The TiFrameQueue and its ancester object TiLightQueue doesn't support memory extending.
 * It's almost useless in memory highly reconstraint embedded systems. If you want this
 * feature, you should use TiDynaQueue instead.
 */
/* bool fmque_extend( TiFrameQueue * que, uintx newsize ); */


#ifdef __cplusplus
}
#endif


#endif /* _RTL_FRAMEQUEUE_H_6788_ */

