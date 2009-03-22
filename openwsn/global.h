/*****************************************************************************
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
 ****************************************************************************/ 

#ifndef _GLOBAL_H_6382_
#define _GLOBAL_H_6382_


//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// Global Variables with initialization and finalization
//
// 	This module is very important in the whole application. It does not only 
// declares and initializes all the global objects, but also create the 
// relationships between different objects.
// 	In TinyOS and nesC systems, these relationships are constructed by a separate
// Configurator object. As you have seem here, this is not necessary. You should
// understand and adopt the OOP programming model and use it in such conditions.
//  
//----------------------------------------------------------------------------

#include "configall.h"
#include "foundation.h"
#include ".\hal\hal.h"
#include ".\service\svc_actsche.h"
#include ".\service\svc_siocomm.h"
#include ".\service\svc_wlscomm.h"
#include ".\service\svc_console.h"
#include ".\service\svc_debugio.h"
#include ".\service\svc_configure.h"
#include ".\service\svc_openmac.h"
#include ".\service\svc_opennet.h"
#include ".\service\svc_location.h"
#include ".\service\svc_sensor.h"


#define CONFIG_SIOCOMM_ENABLE
#define CONFIG_CONSOLE_ENABLE
#define CONFIG_DEBUGIO_ENABLE

extern TiActionScheduler * 	g_actsche;
extern TiSioComm *			g_sio;
extern TiWlsComm *			g_wls;
extern TiConsole *			g_console;
extern TiConfigure *		g_config;

#ifdef CONFIG_DEBUGIO_ENABLE
extern TiDebugIo * g_debugio;
#endif

extern TiOpenMAC * 			g_mac;
extern TiOpenNET *			g_net;
extern TiSensorService *	g_sensors;
extern TiLocationService *	g_lcs;

// construct all the necessary global object instances. The relationship between
// all these objects are also created.
// 
// @return 
//	0			success
//	negative	failed
//
int8 global_construct( void );

// destroy all the global object instances and their relationships.
// 
// @return 
//	0			success
//	negative	failed
//
int8 global_destroy( void );

#endif