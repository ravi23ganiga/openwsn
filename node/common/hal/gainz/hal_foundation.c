/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 *****************************************************************************/ 

#include "hal_configall.h"
#include <string.h>
#include "hal_foundation.h"

/******************************************************************************
 * @author zhangwei on 20070610
 * @description
 * "hal_foundation" module, contains some general utility functions used frequvently
 * in other modules.
 *  
 * @state
 *  developing
 * 
 * @modified by zhangwei on 20070610
 * 	- add hal_delay()
 * 	- add the interface functions on interrupt management
 * @modified by zhangwei on 20090520
 *	- remove most of the C source code in this file. this file should be as simple
 *    and short as possible.
 *	- add interrupt num-object-handler map table support in this module.
 * @modified by zhangwei on 20090528
 *	- revison
 *  
 *****************************************************************************/ 

uint8 g_atomic_level = 0;
TiFunEventHandler m_listener = NULL;
void * m_listener_owner = NULL;

#ifdef CONFIG_INT2HANDLER_ENABLE
_TiIntHandlerItem m_int2handler[CONFIG_INT2HANDLER_CAPACITY];
#endif



/******************************************************************************
 * software initialization of the hal layer
 *****************************************************************************/

void hal_init( TiFunEventHandler listener, void * object )
{
	g_atomic_level = 0;
	m_listener = NULL;
	m_listener_owner = NULL;
	hal_setlistener( listener, object );
	memset( &(m_int2handler[0]), 0x00, sizeof(m_int2handler) );
}


/******************************************************************************
 * interaction with upper layer using listener mechanism
 *****************************************************************************/


inline void hal_setlistener( TiFunEventHandler listener, void * listener_owner )
{
	m_listener = listener;
	m_listener_owner = listener_owner;
}

inline void hal_notifylistener( TiEvent * e )
{
	if (m_listener != NULL)
		m_listener( m_listener_owner, e );
}

inline void hal_notify_ex( TiEventId eid, void * objectfrom, void * objectto )
{
	TiEvent e;

	/* assert( m_listener != NULL ); */
	if (m_listener != NULL)
	{
		e.id = eid;
		e.objectfrom = objectfrom;
		e.objectto = ((objectto == NULL) ? m_listener_owner : objectto);
		m_listener( m_listener_owner, &e );
	}
}


/******************************************************************************
 * interaction with osx kernel
 *****************************************************************************/

/*
TiFunOsxPostEvent m_osxpost = NULL;

void hal_set_osxpost( TiFunOsxPostEvent osx_post )
{
	m_osxpost = osx_post;
}

void hal_osxpost( TiEventId eid, void * objectfrom, void * objectto )
{
	static TiEvent e;
	//assert( m_osxpost != NULL );
	e.id = eid;
	e.objectfrom = objectfrom;
	e.objectto = objectto;
	m_osxpost( &e );
}
*/

/******************************************************************************
 * interaction with nano os (running on atmegal 128 only currently)
 *
 * the following two function are used with nano os kernel only.
 * if you want to use nano os in your application, you should provide a hal_set_nospost()
 * in this module, and the nano os kernel will call this function during its' 
 * initialization.
 * 
 * since the nano os is built on top of the hal layer, you should not call os_post() 
 * directly in your hal layer module. you should call hal_nospost() instead. 
 *****************************************************************************/
/*
#ifdef CONFIG_NANOS_ENABLE 
static bool (* m_nospost)(hal_nostask_t); 
#endif

#ifdef CONFIG_NANOS_ENABLE 
inline void hal_set_nospost( bool (* nospost)(hal_nostask_t) )
{
	m_nospost = nospost;
}
#endif

#ifdef CONFIG_NANOS_ENABLE 
inline void hal_nospost( void (*task)(void) )
{
	m_nospost(task);
}
#endif
*/

