#ifndef _CONFIGALL_H_4237_
#define _CONFIGALL_H_4237_

/******************************************************************************
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
 * @modified by zhangwei on 200601-26
 * add macros to support different hardware platforms
 * 
 * @modified by zhangwei on 20060510
 * replace TARGET_XXX with CONFIG_TARGET_XXX
 * 
 *****************************************************************************/

/******************************************************************************
 * @attention
 *                  IMPORTANT ISSUE ON HOW TO USE THIS FILE
 *
 * OPENWSN_XXX are constant macro definitions. you should NOT change them. while, 
 * CONFIG_XXX are configure setting macros. you can change them to reflect your own 
 * settings
 *
 * if you want to cancel some macro, pls use #undef macro. you can just place
 * the #undef after the previous macro. if you want it to take effective, you 
 * can place the #undef XXX macro before the configuration macro.
 * 
 * NOT to use "//" to comment these macros. because future developes may think 
 * they are really comments and deleted them! 
 *****************************************************************************/

/******************************************************************************
 * User Changable Configurations
 *****************************************************************************/
 
#define FOR_2420_TEST 
 
#define MAJOR_VERSION 1
#define MINOR_VERSION 2
 
/* a macro to enable the debug source code 
 * for release version, you should undef this macro
 */
#define CONFIG_DEBUG

/* Hardware Platform Choosing Configuration
 * now we have four hardware platforms:
 * - CONFIG_TARGET_OPENNODE_10  for OpenNODE version 1.0
 * - CONFIG_TARGET_OPENNODE_20  for OpenNODE version 2.0
 * - CONFIG_TARGET_OPENNODE_30  for OpenNODE version 3.0
 * - CONFIG_TARGET_WLSMODEM_11 for WlsModem version 1.1
 * - CONFIG_TARGET_GAINS for ICT'sGAINS hardware
 * 
 * @attention: there're only one above macro allowed in the system!
 * currently, openwsn only support OPENNODE_10, 20, 30
 */
#undef CONFIG_TARGET_OPENNODE_10
//#define CONFIG_TARGET_OPENNODE_20
#undef CONFIG_TARGET_WLSMODEM_11
#undef CONFIG_TARGET_GAINS
#define CONFIG_TARGET_OPENNODE_30



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

/* "openwsn" is designed to be integrated with existed mature OS. currently, it
 * only support uCOS-II. you can change the value of macro CONFIG_OS
 * to select the OS.  the default setting is OS_NONE 
 */
#define OPENWSN_OS_NONE 0  
#define OPENWSN_OS_TINYOS 1
#define OPENWSN_OS_UCOSII 2
#define OPENWSN_OS_EMBEDDEDLINUX 3
#define OPENWSN_OS_FREERTOS 4
#define OPENWSN_OS_DEFAULT OPENWSN_OS_NONE 

#define CONFIG_OS OPENWSN_OS_UCOSII


/* maximum MAC layer frame length. no more than 127.  
 * attention the length value in 802.15.4's PHY frame does NOT include itself. 
 * according to cc2420's definition, the length of PHY frame 
 *  = 4B preamble + 1B SFD(0xA7) + 1B frame length
 * the 1B frame length is essentially the length of MAC frame. 
 */
//#define CONFIG_MAX_PHY_FRAME_LENGTH 0x7F
//#define CONFIG_MAX_MAC_FRAME_LENGTH (CONFIG_MAX_PHY_FRAME_LENGTH-9)
//#define CONFIG_MAX_UART_FRAME_LENGTH 0x7F
#define OPENWSN_MAX_MAC_FRAME_LENGTH 0x7F
#define OPENWSN_MAX_PHY_FRAME_LENGTH (OPENWSN_MAX_MAC_FRAME_LENGTH+1)
#define OPENWSN_MAX_UART_FRAME_LENGTH (OPENWSN_MAX_MAC_FRAME_LENGTH+1)

/* UART frame identification byte */
#define CONFIG_DEFAULT_ESCAPE_CHAR 27
#define CONFIG_DEFAULT_FRAME_PREFIX CONFIG_DEFAULT_ESCAPE_CHAR

/*******************************************************************************
 * User Un-Changable Configurations
 * these macros may induced from above macros, so you shouldn't modify them.
 ******************************************************************************/

#ifdef CONFIG_DEBUG
#define GDEBUG
#endif


#ifndef NULL
  #define NULL 0
#endif

#ifndef __cplusplus
  #define bool char
  #define true  1
  #define false 0
  #define BOOL bool
  #define TRUE 1
  #define FALSE 0
#else
  #define BOOL char
  #define TRUE 1
  #define FALSE 0
#endif  

typedef signed char 	int8;
typedef signed short 	int16;
typedef signed int 		int32;
typedef long long 		int64;
typedef unsigned char 	uint8;
typedef unsigned short 	uint16;
typedef unsigned long 	uint32;
typedef unsigned long long uint64;
typedef unsigned char 	boolean;
typedef float          	fp32;             /* single precision floating point variable (32bits)  */
typedef double         	fp64;             /* double precision floating point variable (64bits)  */

#endif
