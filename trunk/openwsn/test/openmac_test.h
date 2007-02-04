
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

//----------------------------------------------------------------------------
// @author zhangwei on 2006-07-25
// 本程序只测试ARM 7内部的两个UART，它会把每一个收到的字符原封不动的送回去。
// 可通过超级终端、串口调试助手等测试本程序效果
// 
// 本程序测试通过后，将保证两个UART可以同时工作，不发生冲突
//----------------------------------------------------------------------------


#ifndef _OPENMAC_TEST_H_
#define _OPENMAC_TEST_H_
//----------------------------------------------------------------------------

#include "..\foundation.h"
#include "..\global.h"

void openmac_run( void );

//----------------------------------------------------------------------------
#endif