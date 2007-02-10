/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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

using System;
using System.Collections.Generic;
using System.Text;

namespace WorldView
{
    class DllService
    {
        // @TODO for zhousongli
        // in the future, the LocalService class will inheritate from DllService
        // DllService encapsulate the C language libsink.dll, and LocalService is intended to 
        // provide rich API for GUI uses.
        
        //[DllImport("libsink", EntryPoint = "svc_read", CharSet = CharSet.Auto)]
        //public unsafe static extern byte svc_read(void* svc, [In, Out]/* out*/ byte* buf, byte capacity, ushort opt);

        //[DllImport("libsink", EntryPoint = "svc_write")]
        //public unsafe static extern byte svc_write(void* svc, byte[] buf, byte len, ushort opt);

        //[DllImport("libsink", EntryPoint = "svc_create")]
        //public unsafe static extern void* svc_create(ushort id, ushort opt);

        //[DllImport("libsink", EntryPoint = "svc_start")]
        //public unsafe static extern void* svc_start(void* svc);

        //[DllImport("libsink", EntryPoint = "svc_stop")]
        //public unsafe static extern void* svc_stop(void* svc);

        //[DllImport("libsink", EntryPoint = "svc_uart_open")]
        //public unsafe static extern sbyte svc_uart_open(void* svc, byte[] name);

        //[DllImport("libsink", EntryPoint = "svc_uart_close")]
        //public unsafe static extern sbyte svc_uart_close(void* svc);

        //[DllImport("libsink", EntryPoint = "svc_uart_configure")]
        //public unsafe static extern sbyte svc_uart_configure(void* svc, uint baudrate, byte databits, byte stopbits, byte parity, byte optflag);
        
    }
}
