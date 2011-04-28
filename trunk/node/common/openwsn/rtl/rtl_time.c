should move to rtl_time

Ã·π©time
svc_time is based on rtl_time, hal_timer

rtl_time: provides an data structure to operate the time information
svc_time: provides the device time and system time. The device time is maintained 
by the hardware, which is hardware dependent. The system time is maintained by 
this module, which is hardware independent. Since the system time is usually converted
from the device time, it's often less accurate than the device time, but it's 
convenient to compare with other devices.


typedef TiTime64 TiDeviceTime;
typedef TiTime64 TiSystemTime;

TiDeviceTime g_devtime;
TiSystemTime g_systime;

