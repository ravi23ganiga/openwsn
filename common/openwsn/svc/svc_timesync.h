#include "hal_timer.h"

/**
 * This module implements a short duration high resolutioin time synchronization service.
 * This service is used mainly for wireless time-spatial measurements. 
 * 
 * If you want long term low resolution synchronization, you should use svc_clocksync 
 * instead.
 * 
 * For performance and portable issues, the timesync module uses uint16 as timestamp.
 */

timesync_construct
timesync_destroy
timesync_open( perod, protocol_id )
timesync_close
timesync_evolve
timesync_stop
timesync_pause
timesync_resume
timesync_current
timesync_set/get
timesync_forward
timesync_backward
timesync_adjust




ʵ�ָ߾��ȼ�ʱ��ͬ���������м�ʱ��ӵ��һ����ͬ�Ŀ�ʼʱ���׼��ͬʱ�ܹ�����һ��ʱ��
���ϣ����ʱ��ͬ��������clocksync

ͬ��svc_clock


uses one hardware timers capture function


timer_counter

backward
forward
scale_factor
shift_factor

y = x * a + b

construct
destroy
open( TiCc2420Transceiver * cc, TiTimerAdapter * tm )
close
evolve( tmsynptr, TiEvent * e );







