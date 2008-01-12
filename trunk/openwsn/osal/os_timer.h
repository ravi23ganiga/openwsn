/* the MCU's hardware timers are usually very powerful. furthermore, they maybe varies from chip suppliers.
 * the OS timer is relatively simple and easy to use. but it's designed to satisfy the OS requirement only. 
 * "openwsn" doesn't want to make the timer is too heavy. it should be simple and lightweight. 
 * if the application need a more complicated timer, it can use the HAL's timer directory rather than OS timer.
 */
 
@attention:
considering the application can often access the HAL timer, there's only one OS timer.
=> no! you should support multiple OS timer.

// only one hardware timer used by "osal", but it will generate multiple software timers

int os_gettimer( int i );  
int os_settimer( int i );
int os_elapse( int i, int timepassed );
int os_expired( int i );
