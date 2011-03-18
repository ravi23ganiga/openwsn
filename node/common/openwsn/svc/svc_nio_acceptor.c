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

#include "svc_configall.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_framequeue.h"
#include "../rtl/rtl_debugio.h"
#include "../hal/hal_cc2420.h"
#include "../hal/hal_led.h"
#include "../hal/hal_debugio.h"
#include "svc_foundation.h"
#include "svc_nio_acceptor.h"
#include "../hal/gainz/hpl_cpu.h"

#ifdef CONFIG_DEBUG
#include "../rtl/rtl_dumpframe.h"
#endif

#ifdef CONFIG_DYNA_MEMORY
TiNioAcceptor * nac_create( void * mem, TiFrameTxRxInterface * rxtx, uint8 rxque_capacity, 
	uint8 txque_capacity )
{
	char * buf;
	uint16 size = NIOACCEPTOR_HOPESIZE(rxque_capacity, txque_capacity);
	TiNioAcceptor * nac;
	
	buf = (char *)malloc( size );
	if (buf != NULL)
	{
		nac = nac_construct( buf, size );
		nac = nac_open( nac, rxtx, rxque_capacity, txque_capacity );
	}
	else
		nac = NULL;
		
	return nac;	
}
#endif

#ifdef CONFIG_DYNA_MEMORY
void nac_free( TiNioAcceptor * nac)
{
	nac_close( nac );
	free( nac );
}
#endif

TiNioAcceptor * nac_construct( void * mem, uint16 size )
{
	TiNioAcceptor * nac = (TiNioAcceptor *)mem;
	memset( mem, 0x00, size );
	nac->memsize = size;
	return nac;
}

void nac_detroy( TiNioAcceptor * nac )
{
	nac_close( nac);
}

/**
 * @pre You have already initialized the rxque and txque before open the network 
 * 		acceptor object.
 * 
 * @warning: You must guarantee the memory block size is large enough to hold rxque
 *      and txque! Or else the application will access inviolate memory space and 
 *      the whole device may probably rebooted when accessing the TiNioAcceptor object.
 */
TiNioAcceptor * nac_open( TiNioAcceptor * nac, TiFrameTxRxInterface * rxtx, 
	uint8 rxque_capacity, uint8 txque_capacity )
{
	char * buf;
	
// #ifdef CONFIG_DEBUG
	svc_assert( nac->memsize >= NIOACCEPTOR_HOPESIZE(rxque_capacity,txque_capacity) );
// #endif	

	nac->state = 0;
	memmove( &(nac->rxtx), rxtx, sizeof(TiFrameTxRxInterface) );
	
	buf = (char*)nac + sizeof(TiNioAcceptor);
	nac->rxque = fmque_construct( buf, FRAMEQUEUE_HOPESIZE(rxque_capacity) );
	buf += FRAMEQUEUE_HOPESIZE(rxque_capacity);
	nac->txque = fmque_construct( buf, FRAMEQUEUE_HOPESIZE(txque_capacity) );
	buf += FRAMEQUEUE_HOPESIZE(txque_capacity);
    nac->rxframe = frame_open( buf, FRAMEQUEUE_ITEMSIZE, 0, 0, 0 ); 

	// If you encounter problems here, please check whether you have input enough
	// memory block to hold the full frame.
	hal_assert((nac->rxframe != NULL) && (nac->txque != NULL) && (nac->rxque != NULL));
	return nac;
}

void nac_close( TiNioAcceptor * nac )
{
	fmque_destroy( nac->rxque );
	fmque_destroy( nac->txque );
	frame_close( nac->rxframe );
}

TiFrameTxRxInterface * nac_rxtx_interface( TiNioAcceptor * nac )
{
	return &(nac->rxtx);
}

/** Return the sending queue(txque) inside acceptor */
TiFrameQueue * nac_txque( TiNioAcceptor * nac )
{
	return nac->txque;
}

/** Return the receiving queue(rxque) inside acceptor */
TiFrameQueue * nac_rxque( TiNioAcceptor * nac )
{
	return nac->rxque;
}

/**
 * Push the frame object into the sending queue(txque) and wait for the acceptor
 * to send it out. If the txque is full, then this function will return false.
 * 
 * @param nac TiNioAcceptor object
 * @param item TiFrame object to be sent
 * @return true if success and false is failed.
 */
intx nac_send( TiNioAcceptor * nac, TiFrame * item, uint8 option )
{   
	
	if (fmque_pushback( nac->txque, item ) > 0)
	{  
		
		nac_evolve( nac,NULL );
		return frame_capacity(item);
	}
	else
		return 0;
}

/**
 * Pop the front frame from the receiving queue (rxque) and place it into the
 * item.
 */
intx nac_recv( TiNioAcceptor * nac, TiFrame * item , uint8 option )
{
	TiFrame * front;
	
	nac_evolve( nac,NULL );
	front = fmque_front( nac->rxque );
	if (front != NULL)
	{
		frame_totalcopyfrom( item, front );
		fmque_popfront( nac->rxque );
		return frame_capacity(front);
	}
	else
		return 0;
}

/** 
 * Check whether there's incoming frame pending in the transceiver object. If there
 * has, then get it back and place it at the tail of the rxque.  Also check whether
 * there's a frame in txque wait for sent. If has, then send it.
 */
void nac_evolve ( TiNioAcceptor * nac, TiEvent * event )
{   
	TiFrameRxTxInterface * rxtx = &(nac->rxtx);
	uint8 count, first;
	TiFrame * f;

    if (!fmque_empty(nac->txque))
	{   
		f =  fmque_front( nac->txque );
        
		// @attention: The developer often forget to call frame_setlength(f) after
		// copy the data into the frame. It's the developer's responsibility to set
		// correct length value of the frame, or else frame_length(f) may probably 
		// return 0, which lead to sending failure.
		// 
		// the following line is only for testing by replacing frame_length(f)
		// with frame_capacity(f). 
		//
		// @todo: 2011.03
		first = frame_firstlayer(f);
		
		//todo
		//���ͺ����е�frame_layerstartptr(f,first)��������ģ����У�first = f->firstlayer,
		
		count = rxtx->send( rxtx->provider, frame_layerstartptr(f,first), frame_layercapacity(f,first), f->option );
		// count = rxtx.send( rxtx.provider, frame_startptr(f), frame_capacity(f), frame->option );
		// count = rxtx.send( rxtx.provider, frame_startptr(f), frame_length(f), frame->option );
		
		// the following is for debug only
		/*#ifdef CONFIG_DEBUG
		ieee802frame154_dump(f);
		#endif*/
		
		if (count > 0)
		{   
			
			fmque_popfront( nac->txque );
			
		}
	}
	
	if (!fmque_full(nac->rxque ))
	{
		#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
		hal_enter_critical();
		#endif
        
		// @pre nac->rxframe must be initialized correctly.
		f = nac->rxframe;
		hal_assert( f != NULL );
	    frame_reset( f, 0, 0, 0 );    		
		count = rxtx->recv( rxtx->provider, frame_startptr(f), frame_capacity(f), f->option );
		if (count > 0)
		{  
            

            frame_setlength( f, count );
            frame_setcapacity( f, count );
			fmque_pushback( nac->rxque, f );
		}
		
		#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
		hal_leave_critical();
		#endif
	}
}


TiNioSession * nac_getcursession( TiNioAcceptor * nac, TiNioSession * session )
{
	session->rxque = nac->rxque;
	session->txque = nac->txque;
	session->option = 0x00;
	return session;
}

/*
 * This listener can be called by the transceiver component when a new frame arrives.
 * So you can move the frame received by transceiver into the rxque inside TiNioAcceptor.
 * 
 * @attention: This function isn't mandatory. You can also call nac_evolve() to 
 * do this. The only difference between nac_evolve() and nac_on_frame_arrived_listener()
 * is that: the listener can be executed in interrupt mode, while the evolve()
 * cannot do this.
 */
#ifdef CONFIG_NIOACCEPTOR_LISTENER_ENABLE
void nac_on_frame_arrived_listener( TiNioAcceptor * nac, TiEvent * e )
{
	TiFrameRxTxInterface * rxtx = nac->rxtx;
	TiFrame * f = nac->rxframe;
	
	hal_atomic_begin();
	count = rxtx.recv( rxtx.provider, frame_startptr(f), frame_capacity(f), 0x00 );
	if (count > 0)
	{
		frame_setlength( f, count );
        frame_setcapacity( f, count );
		fmque_pushback( nac->rxque, f );
	}
	hal_atomic_end();
}
#endif

