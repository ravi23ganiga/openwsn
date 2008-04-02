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

#ifndef _CONFIGALL_H_4237_
#define _CONFIGALL_H_4237_

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
 
#define OPENWSN_MAJOR_VERSION 0
#define OPENWSN_MINOR_VERSION 8
 
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
#define CONFIG_TARGET_OPENNODE_10
#undef  CONFIG_TARGET_OPENNODE_10

#define CONFIG_TARGET_OPENNODE_20
#undef  CONFIG_TARGET_OPENNODE_20

#define CONFIG_TARGET_OPENNODE_30
//#undef  CONFIG_TARGET_OPENNODE_30

#define CONFIG_TARGET_WLSMODEM_11
#undef  CONFIG_TARGET_WLSMODEM_11

#define CONFIG_TARGET_GAINS
#undef  CONFIG_TARGET_GAINS

/* Compiler used to Compile the Source code
 */
#define CONFIG_COMPILER_ADS
//#undef  CONFIG_COMPILER_ADS

#define CONFIG_COMPILER_GNU
#undef  CONFIG_COMPILER_GNU
 
/* FlashStore�Ŀ�ʼ��ַ�ʹ�С
 * FlashStore����ϵͳ�����ڼ�洢�������ò���,����MCUоƬE2PROM����Flash��ַ�ռ��е�һƬ
 * ����. �������������ڳ�ʼ��TConfigure����(service\configure.*)
 * @warning
 * - ����ϵͳ����Ҳ��������Flash��,�������ڱ���ϵͳ������Ϣ��FlashStore�������ܿ�����
 * ��������.
 * - FlashStore��size�����ϵͳ��������ʵ��ռ�ÿռ��,���򽫷���Խ�����
 */
#define CONFIG_FLASHSTORE_ADDR 0x4215
#define CONFIG_FLASHSTORE_SIZE sizeof(TConfigStore)

/* �ú����ϵͳ�೤ʱ�����ι��һ��
 * ����ֵ��ν���,ȡ����watchdogӲ�������Ҫ��
 */
#define CONFIG_WATCHDOG_INTERVAL  50000

/* ����SIO Frame�Ŀ�ʼ��־�ֽ�
 * ����ʽModem���ԣ���ʹ�øñ�־
 */
#define CONFIG_START_OF_FRAME 27

/* UARTƥ�����RS232����485
 */
#undef CONFIG_UART_RS232
#define CONFIG_UART_RS232

#define CONFIG_UART_RS485
#undef CONFIG_UART_RS485

#ifdef CONFIG_UART_RS232
  #undef CONFIG_UART_RS485
#endif  

#define CONFIG_UART0_STARTUP_BAUDRATE 115200
#define CONFIG_UART_BAUDRATE 9600

/* "openwsn" is designed to be integrated with existed mature OS. currently, it
 * only support uCOS-II. you can change the following macro to configure the OS
 * to be integrated. 
 * currently, it only support uCOS. it's also the default settings. 
 * in the future, the default setting will be OS_NONE
 */
#undef  CONFIG_OS_NONE 
#define CONFIG_OS_TINYOS 
#undef  CONFIG_OS_UCOSII 
#undef  CONFIG_OS_EMBEDDEDLINUX 
#undef  CONFIG_OS_FREERTOS 


/* maximum MAC layer frame length. no more than 127.  
 * attention the length value in 802.15.4's PHY frame does NOT include itself. 
 * according to cc2420's definition, the length of PHY frame 
 *  = 4B preamble + 1B SFD(0xA7) + 1B frame length
 * the 1B frame length is essentially the length of MAC frame. 
 */
#define OPENWSN_MAX_MAC_FRAME_LENGTH 0x7F
#define OPENWSN_MAX_PHY_FRAME_LENGTH (OPENWSN_MAX_MAC_FRAME_LENGTH+1)
#define OPENWSN_MAX_UART_FRAME_LENGTH (OPENWSN_MAX_MAC_FRAME_LENGTH+1)

/* UART frame identification byte */
#define CONFIG_DEFAULT_ESCAPE_CHAR 27
#define CONFIG_DEFAULT_FRAME_PREFIX CONFIG_DEFAULT_ESCAPE_CHAR


/* System wide configuration for lpc213x.
 * Fosc��Fcclk��Fcco��Fpclk must be defined or you may encounter data failure
 * when using UART, especially when the communication data rate is high. 
 *
 * Fosc:	Crystal frequence,10MHz~25MHz��should be the same as actual status. 
 *          Ӧ����ʵ��һ������Ƶ��,10MHz~25MHz��Ӧ����ʵ��һ��
 * Fcclk:   System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz. 
 *          ϵͳƵ�ʣ�����ΪFosc��������(1~32)����<=60MHZ
 * Fcco:    CCO frequence,should be 2��4��8��16 multiples of Fcclk, ranged from 156MHz to 320MHz. 
 *          CCOƵ�ʣ�����ΪFcclk��2��4��8��16������ΧΪ156MHz~320MHz
 * Fpclk:   VPB clock frequence , must be 1��2��4 multiples of (Fcclk / 4).
 *          VPBʱ��Ƶ�ʣ�ֻ��Ϊ(Fcclk / 4)��1��2��4��
 */
#if (defined(CONFIG_TARGET_OPENNODE_10) || defined(CONFIG_TARGET_OPENNODE_20) || defined(CONFIG_TARGET_WLSMODEM_11))
  #define Fosc            11059200                  
  #define Fcclk           (Fosc * 4)                  
  #define Fcco            (Fcclk * 4)                 
  #define Fpclk           (Fcclk / 4) * 1             
#endif

#ifdef CONFIG_TARGET_OPENNODE_30
  #define Fosc            16000000                
  #define Fcclk           (Fosc * 4)              
  #define Fcco            (Fcclk * 4)             
  #define Fpclk           (Fcclk / 4) * 1         
#endif	    


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
