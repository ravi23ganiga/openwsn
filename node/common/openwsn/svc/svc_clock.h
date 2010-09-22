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

#include "../rtl/rtl_timer.h"
#include "../hal/hal_timer.h"


一类是以time_t结构为参数的，共十个字节，一类是出于效率起见，以uint16整型为参数的

TiClock * clock_construct
clock_destroy
clock_open
clock_close
clock_pause
clcok_resume
clock_forward
clock_backward
clock_adjust
clock_current
clock_set/get
clock_disable
clock_enable



sysclock


TiHpmClock

TiNetSyncTimer
TiNetSyncClock
nsync_timer_
nsync_clock

TiClock

clock_

shtimer
hpmtimer
longtimer

longtimer





svc_powerclock

TiPowerClock



TiLongClock
TiUsClock



TiRtcAdapter

TiClock


clock_forward
clock_backward
clock_adjust
clock_run
clock_step
clock_tune
clock_capture
clock_snapshot
clock_lastsnapshot



TiTimerAdapter *