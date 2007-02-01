#ifndef _CONFIGALL_H_4237_
#define _CONFIGALL_H_4237_

/*******************************************************************************
 * @author zhangwei on 2006-07-20
 * this is the configure file of the whole project
 * you can #define or #undef the macros in this file to enable/disable some 
 * software modules.
 * 
 * every module including re-configure functions SHOULD include this file as 
 * their first one. this is implemented by including "foundation.h". the "foundation.h"
 * has already included the "configall.h"
 * 
 * @modified by zhangwei on 2006-07-20
 * @modified by zhangwei on 20061015
 * add macro CONFIG_MAX_FRAME_LENGTH
 * 
 * @modified by zhangwei on 20061-26
 * add macros to support different hardware platforms
 ******************************************************************************/
#define FOR_2420_TEST 
 
#define MAJOR_VERSION 1
#define MINOR_VERSION 2
 
/* a macro to enable the debug source code 
 * for release version, you should undef this macro
 */
#undef GDEBUG
#define GDEBUG

/* Hardware Platform Choosing Configuration
 * now we have four hardware platforms:
 * - OpenNODE version 1.0
 * - OpenNODE version 2.0
 * - WlsModem version 1.1
 * - GAINS
 */
#define TARGET_OPENNODE_10
#undef TARGET_OPENNODE_10

#undef TARGET_OPENNODE_20
#define TARGET_OPENNODE_20



#define TARGET_WLSMODEM_11
#undef TARGET_WLSMODEM_11

#define TARGET_GAINS 
#undef TARGET_GAINS 


 

/* FlashStore的开始地址和大小
 * FlashStore用于系统掉电期间存储各种设置参数,它是MCU芯片E2PROM或者Flash地址空间中的一片
 * 区域. 这两个参数用于初始化TConfigure对象(service\configure.*)
 * @warning
 * - 由于系统程序也被烧制在Flash中,所以用于保存系统配置信息的FlashStore区域必须避开程序
 * 所在区域.
 * - FlashStore的size必须比系统配置数据实际占用空间大,否则将发生越界错误
 */
#define CONFIG_FLASHSTORE_ADDR 0x4215
#define CONFIG_FLASHSTORE_SIZE sizeof(TConfigStore)

/* 该宏控制系统多长时间必须喂狗一次
 * 该数值如何解释,取决于watchdog硬件和软件要求
 */
#define CONFIG_WATCHDOG_INTERVAL  50000

/* 串口SIO Frame的开始标志字节
 * 对流式Modem而言，不使用该标志
 */
#define CONFIG_START_OF_FRAME 27

/* UART匹配采用RS232还是485
 */
#undef CONFIG_UART_RS232
#define CONFIG_UART_RS232

#define CONFIG_UART_RS485
#undef CONFIG_UART_RS485

#ifdef CONFIG_UART_RS232
  #undef CONFIG_UART_RS485
#endif  

/* If you want the application to support RTOS like uCOS, you should define the following 
 * macro. the default setting is RTOS enabled */

#define CONFIG_USE_RTOS  
#undef CONFIG_USE_RTOS

/* maximum frame length. no more than 127*/
#define CONFIG_MAX_PHY_FRAME_LENGTH 0x7F
#define CONFIG_MAX_MAC_FRAME_LENGTH (CONFIG_MAX_PHY_FRAME_LENGTH-9)
#define CONFIG_MAX_UART_FRAME_LENGTH 0x7F

/* UART frame identification byte */
#define CONFIG_DEFAULT_ESCAPE_CHAR 27
#define CONFIG_DEFAULT_FRAME_PREFIX CONFIG_DEFAULT_ESCAPE_CHAR

#endif