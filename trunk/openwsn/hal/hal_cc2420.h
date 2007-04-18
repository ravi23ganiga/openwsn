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
#ifndef _HAL_CC2420_H_1278_
#define _HAL_CC2420_H_1278_

/*******************************************************************************
 * @author zhangwei on 2006-07-20
 * TCc2420Driver
 * This is an software abstraction of the cc2420 transceiver hardware. you can 
 * fully manipulate the cc2420 hardware through this object. 
 * 
 * @attention
 * 	- TCc2420Driver is different to TSpiDriver. TSpiDriver is only
 * the encapsulation of SPI communication. It shouldn't has relationships with 
 * cc2420. 
 *  - this module is a software abstraction and encapsulation of cc2420 hardware. 
 * it should be as simple as possible.
 * 	- some one may regards this module as the PHY layer of the whole system. 
 * however, it may contain some MAC functions. This is due to the reason that 
 * the chip "cc2420" is 802.15.4 MAC supported.
 * 
 * @TODO
 * 	try to implement standard PHY functions based on this module in the future.
 * 
 * @modified by huanghuan on 2008-08-10
 * - modified the interface function. 
 * - changes a lot data type from uint16 to uint8, because cc2420 support maximum
 * 	frame length of 256.
 * - add interface function: cc2420_configure, cc2420_sleep, cc2420_wakeup
 * 
 * @modified by huanghuan on 2008-08-01
 * porting the old source code successfully
 * testing passed. 
 * 
 * @modified by zhangwei on 2006-09-06
 * update the comment. add a lot of descriptions.
 * revise code logic and the interface. 
 * 请黄欢按照本文件提供的接口对原代码进行调整，并注意以下问题
 * 
 * @modified by zhangwei on 2006-09-11
 * add support to sleep in the state machine
 * revised the state machine and other source code.
 * 
 * @modified by zhangwei on 2006-10-14
 * integrated with huanghuan's code. and do much more format revisions.
 *
 * @modified by zhangwei on 2006-11-19
 * formulate the I/O interface of cc2420 drvier
 *
 * uint8 cc2420_rawread( TCc2420Driver * cc, char * buf, uint8 size, uint8 opt );
 * 从2420 driver内部buffer中读走一个frame, size表示参数buf所指缓冲区的大小
 * return the byte count moved to parameter "frame" buffer.
 * 返回实际move的字节数
 * 如果size偏小放不下，那么只最多size个字符。我们认为主程序有责任开辟足够大的buffer来调用rawread
 * 参数buf所指的buffer中将包含一个完整的PHY frame，或者内容无变动
 * 如果没有数据包可读，则返回0
 * opt参数备用
 * 
 * uint8 cc2420_rawwrite( TCc2420Driver * cc, char * buf, uint8 len, uint8 opt );
 * 将char * buf中的frame发送出去
 * 这里参数buf所指的内存中已经包含了一个完整的cc2420 phy frame，所有数据都已经在内存中排好了
 * 返回实际发送成功的字节数，一般情况下应该等于len，表示全部发送成功
 * 我们以一个完整的frame作为发送和接收单位，不支持发送或者接收半个frame
 *
 * int8 cc2420_sendframe( TCc2420Driver * cc, TCc2420Frame * frame, uint8 opt );
 * 类似于cc2420_rawwrite 但是frame信息是通过一个结构体TCc2420Frame传递进来的，不是在内存中排好再传进来的
 * 
 * int8 cc2420_recvframe( TCc2420Driver * cc, TCc2420Frame * frame, uint8 opt );
 * 类似于cc2420_rawread, 接收一个frame，但是接收到的数据不是排列在内存中，而是放在一个结构体中
 * opt中的某一位指明是否是broadcast还是普通的发送，是否需要ACK
 * opt中的设置将覆盖参数frame中的设置
 * 
 * #define cc2420_rawbroadcast(cc,frame,len,opt) cc2420_rawwrite(cc,frame,len,opt|0x01)
 * #define cc2420_broadcast(cc,frame,opt) cc2420_sendframe(cc,frame,opt|0x01)
 * 广播一个frame出去，其实就是调用rawwrite或者sendframe
 * broadcast frame是不需要ACK的
 * 
 * 删除read_stream和write_stream
 * 可以不支持cc2420_read()和cc2420_write()
 * 
 * @modified by zhangwei on 20070324
 * make some revisions today. two main modifications today:
 * - change the old member variable "txbuffer" and "rxbuffer" to their new name
 * "txframe" and "rxframe". furthremore, change txframe[] to txframe, and rxframe[]
 * to rxframe. it's unnecessary to keep so many buffers in HAL layer. if you want
 * to allocate queuing buffer, you should do it in higher layer such as MAC, NET 
 * or even APP layer.
 * - eliminate the rfsettings member variable.  
 * 但是在消除rfsettings变量的过程中，合并了cc2420结构和rfsettings结构中的panid, seqid
 * 和rssi三个变量，应该没关系吧？
 * 
 * @TODO: i think pRxInfo is also obsolete.
 *
 ******************************************************************************/

#include "hal_foundation.h"
#include "hal_configall.h"
#include "hal_spi.h"
#include "hal_cc2420chip.h"
#include "hal_openframe.h"
 
/*******************************************************************************
 * IEEE 802.15.4 PPDU format
 * [4B Preamble][1B SFD][7b Framelength, 1b Reserved][nB PSDU/Payload]
 * 
 * IEEE 802.15.4 MAC DATA format (the payload of PHY frame)
 * Beacon Frame
 * [2B Frame Control] [1B Sequence Number][4 or 10 Address][2 Superframe Specification]
 * 		[k GTS fields][m Padding address fields] [n Beacon payload][2 FCS]
 * 
 * Data Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 Address][n Data Payload][2 FCS]
 * 
 * ACK Frame
 * [2B Frame Control] [1B Sequence Number][2 FCS]
 * 
 * MAC Control Frame
 * [2B Frame Control] [1B Sequence Number][4 or 20 ADdress][1 Command Type][n Command Payload][2 FCS]
 * 
 ******************************************************************************/


//#define CC2420_BUF_CAPACITY 1 

#ifdef TARGET_OPENNODE_10
#define FIFO            8  // P0.8  - Input: FIFO from CC2420
#define FIFOP           9  // P0.9  - Input: FIFOP from CC2420
#define CCA            10  // p0.10 - Input:  CCA from CC2420
#define RESET_N        12  // P0.12 - Output: RESET_N to CC2420
#define VREG_EN        13  // P0.13 - Output: VREG_EN to CC2420
#define SFD            16  // P0.16 - Input:  SFD from CC2420
#define CSN            21  // P1.21 - Output: SPI Chip Select (CS_N)  

#define SFD_PORT       0
#define CCA_PORT       0     
#define FIFO_PORT      0    
#define FIFOP_PORT     0     
#define VREG_EN_PORT   0       
#define RESET_N_PORT   0
#define CSN_PORT       1  
#endif

#ifdef TARGET_OPENNODE_20
#define FIFO           22  
#define FIFOP          15  
#define CCA            13  
#define RESET_N        12 
#define VREG_EN        10  
#define SFD            16 
#define CSN            21  

#define SFD_PORT       0
#define CCA_PORT       0     
#define FIFO_PORT      1    
#define FIFOP_PORT     0     
#define VREG_EN_PORT   0       
#define RESET_N_PORT   0
#define CSN_PORT       1  
#endif

#ifdef TARGET_WLSMODEM_11
#define FIFO           16  // P0.16 - Input: FIFO from CC2420
#define FIFOP          15  // P0.15 - Input: FIFOP from CC2420
#define CCA            12  // p0.12 - Input: CCA from CC2420
#define RESET_N        23  // P1.23 - Output: RESET_N to CC2420
#define VREG_EN        10  // P0.10 - Output: VREG_EN to CC2420
#define SFD            11  // P0.11 - Input: SFD from CC2420
#define CSN            17  // P0.17 - Output: SPI Chip Select (CS_N)  

#define SFD_PORT       0
#define CCA_PORT       0     
#define FIFO_PORT      0    
#define FIFOP_PORT     0     
#define VREG_EN_PORT   0       
#define RESET_N_PORT   1
#define CSN_PORT       0  
#endif


#define CC2420_DEFAULT_CHANNEL  19         //channel is 11 - 26

/* cc2420 state constants */
enum { CC_STATE_IDLE=0, CC_STATE_SLEEP, CC_STATE_POWERDOWN };
    
/* config control code */    
#define CC2420_CONFIG_PANID 			0x01 
#define CC2420_CONFIG_LOCALADDRESS		0x02 
#define CC2420_CONFIG_TUNNING_POWER 	0x03
#define CC2420_CONFIG_CHANNEL			0x04
#define CC2420_BASIC_INIT               0x05
#define CC2420_XTAL_SWITCH              0x06
#define CC2420_CONFIG_APPLY				0x07
#define CC2420_SET_ACKREQUEST           0x08


/*power level*/
//POWER_1 is the highest,POWER_8 is the lowest 
#define CC2420_POWER_1  0x01       //  0dBm   17.4mA
#define CC2420_POWER_2  0x02       // -1dBm   16.5mA
#define CC2420_POWER_3  0x03       // -3dBm   15.2mA
#define CC2420_POWER_4  0x04       // -5dBm   13.9mA
#define CC2420_POWER_5  0x05       // -7dBm   12.5mA
#define CC2420_POWER_6  0x06       //-10dBm   11.2mA
#define CC2420_POWER_7  0x07       //-15dBm    8.9mA
#define CC2420_POWER_8  0x08       //-25dBm    8.5mA

/* Packet includes : frame control field(2B), sequence number(1B), PAN ID(2B), destination and source(4B),payload(nB),footer(2B)
 * This is an integrated MAC Frame Format
 */
/* 
typedef struct {
    WORD   frame_control;
    BYTE   seqNumber;
    WORD   PanId;
    WORD   destAddr;
    WORD   srcAddr;
    BYTE   Payload[120];
    WORD   footer;
} TCc2420Frame;
*/
#define TCc2420Frame TOpenFrame

/* @attention
 *	if a variable will be accessed by both the master program and interrupt routine, 
 * then it should be endorsed by keyword "volatile".
 */ 
typedef struct {
    TCc2420Frame pRxInfo;
    uint8 payload_length;
    UINT8 seqid;
    volatile BOOL ackReceived;
    WORD panid;
    WORD myAddr;
    BOOL receiveOn;
    uint8 rssi;
}BASIC_RF_SETTINGS;

// 要重点清查receivepayload_len和payload_length是否一致，是否应归并
typedef struct{
  uint8 state;
  uint8 nextstate;
  TSpiDriver * spi; 
  //uint16 panid;
  uint16 address;
  uint8 channel; // frequency, channel varies from 11 to 26. f = 2405 + 5*(channel - 11) MHz;
  uint8 txlen;
  uint8 rxlen;
  TCc2420Frame txframe;
  volatile TCc2420Frame rxframe;
  //char * txbuf;
  //char * rxbuf;
  //volatile BASIC_RF_SETTINGS rfSettings;
  uint8 sleeprequest;
  uint8 power;
  uint8 ackrequest;             
  //uint8 rssi;                 //最近一次接收到的信息的信号强度
  volatile uint8 receivepacket_len;      //最近一次接收到的包的总长度
  volatile uint8 receivepayload_len;     //最近一次接收到的包的payload的长度
  uint8 sendpayload_len;        //最近一次发送的包的payload长度
  uint8 if_read;                //接收缓冲被读后，变为1， 接收到新信息后，变为0
  
  // @TODO 200703 for huanghuan
  // the following is a map of volatile BASIC_RF_SETTINGS rfSettings
  // this structure should be replaced by cc2420 structure itself
  // rfsettings will be eliminated from the source code now
  volatile TCc2420Frame pRxInfo;
  volatile uint8 payload_length;
  volatile UINT8 seqid;
  volatile BOOL ackReceived;
  WORD panid;
  volatile WORD myAddr;
  volatile BOOL receiveOn;
  volatile uint8 rssi; 		//最近一次接收到的信息的信号强度
}TCc2420Driver;

/* The following variable is declared usually in "global.c". However, it is used
 * in the interrupt service routine of this module. Be sure the variable name 
 * cannot be modified! it must be "g_cc2420"!
 */
extern TCc2420Driver * g_cc2420;
 
/*******************************************************************************
 * construct a TCc2420 driver object in the memory.
 * @param
 * 	buf			memory started address
 * 	size		memory size allocated for this object. this size must be large
 * 				or equal to the object size. 
 * 	spi			TSpiDriver object. the MCU uses this driver to communicate with 
 * 				"cc2420" hardware chip.
 ******************************************************************************/ 
TCc2420Driver * cc2420_construct( char * buf, uint16 size, TSpiDriver * spi );

/*******************************************************************************
 * destroy a TCc2420Driver object 
 * release all necessary resouces.
 ******************************************************************************/ 
void cc2420_destroy( TCc2420Driver * cc );

/*******************************************************************************
 * configure the cc2420 driver object.
 * you may add more parameters in the functions. 
 * different to the parameters in the construct function, you can use this function
 * to modify parameters without rebooting the system to make them take effect.
 * 
 * @param
 * idx 
 * 	1	设置地址
 * 	2	调整发射功率
 * 	3	调整发射频率
 * pvalue	the buffer pointers to the configure data. it is also the buffer
 * 			to receive the results.
 * size		buffer size.
 ******************************************************************************/ 
 
 /* @modified by zhangwei on 20061108
  * 我还是觉得不要用uint16, 而用void * buf或者 void * pvalue比较好，它和后面的
  * size合在一起表示配置数据。用uint16看似代码方便，但是使用者会没有观念，不清楚
  * 配置数据要多大比较合适,而且代码也要做强制类型转换。pvalue方式强制用户要设置size. 
  * 写代码的原则是接口原型应尽量做到满足未来需求，而不是眼前需要。函数的实现可以
  * 改，但函数原型一旦确定，就不宜轻易变动，否则日后维护容易出错
  */
void cc2420_configure( TCc2420Driver * cc, uint8 ctrlcode, uint16 value, uint8 size );

/*******************************************************************************
 * cc2420 chip must be initialized successfully before it can be used for sending
 * or receive 
 ******************************************************************************/
void cc2420_init(TCc2420Driver * cc);

/* return the cc2420 driver state.
 * attention that state is used for other modules. it is not the same as "mode".
 */
uint8 cc2420_state( TCc2420Driver * cc );

void cc2420_set_power(TCc2420Driver * cc,uint8 power);

/* return the last I/O operation's result.
 * result & 0x01		still has data sending
 * result & 0x02		still has data in the internal buffer and to be read
 * 
 * Though this function gives you the ability to check the state of this module, 
 * we hope you do not use this function. 
 */
uint8 cc2420_ioresult( TCc2420Driver * cc );

/*******************************************************************************
 * read data from cc2420 driver
 * read data from the driver's internal buffer. the interrupt service routine 
 * is responsible to place the received data into the internal buffer.
 * you can always call this function to check whether there has data in the internal 
 * buffer no matter what state the wireless chip "cc2420" is. 
 * 
 * @attention
 * you may lost the input data when you cannot call this function fast enough.
 * 
 * @param
 * 	buf			the memory to receive the data
 * 	size		buffer size
 * 	opt			option settings.
 * @return
 * 	the character count copied successfully to the buffer
 ******************************************************************************/ 
//uint8 cc2420_read( TCc2420Driver * cc, char * buf, uint8 size, uint8 opt );

/*******************************************************************************
 * return the received frame entirely to the frame buffer. 
 * different to xxx_read() function, this function will not return when a frame
 * received entirely. For "cc2420" chip, the frame is a standard 802.15.4 frame.
 * 
 * @attention
 * 	- similar to cc_read(), you'll lost the received frame if you cannot call this 
 * function quickly enough.
 * 	- when this function is effective, you must guarantee the internal buffer is 
 * large enough to hold a frame.
 * 
 * @warning 
 * 	problems may be encountered when you switch between "cc2420_read()" and 
 * "cc2420_rawread()". rawread will always assume the second byte in the internal 
 * buffer as frame length and use it to check whether the whole frame received.
 * you'll encounter problems when there's still some data left in the buffer 
 * when you call cc2420_rawread(). you'd better identify the frame start position
 * using another member variable to improve reliability in the future.
 * 
 * @TODO: not finished yet!
 ******************************************************************************/ 
uint8 cc2420_rawread( TCc2420Driver * cc, char * frame, uint8 size, uint8 opt );




/*******************************************************************************
 * @attention
 * 	you must guarantee the internal buffer size is larger than frame length
 * or you may encounter unexpected errors.
 * 
 * @TODO NOT finished Yet!
 ******************************************************************************/ 
#define cc2420_broadcast(cc,frame,len,opt) cc_2420_rawwrite(cc,frame,len,opt|0x01)
//uint8 cc2420_rawwrite( TCc2420Driver * cc, char * frame, uint8 length, uint8 opt );
int8 cc2420_rawwrite( TCc2420Driver * cc, char * frame, int8 length,uint8 opt );

//int8 cc2420_sendframe( TCc2420Driver * cc, TCc2420Frame * frame );
//int8 cc2420_recvframe( TCc2420Driver * cc, TCc2420Frame * frame );
/*******************************************************************************
 * this function is used mainly by the driver itself.
 * it will check whether there are some data to be sent. if there's data in the 
 * txbuf, then it starts the sending process. 
 * 
 * this function doesn't check whether there are some data received like some other
 * drivers, because the data receiving process is interrupt driven.
 * 
 * generally, you needn't to call this function in your source code.
 ******************************************************************************/ 
int8 cc2420_evolve( TCc2420Driver * cc );

/* 启动cc2420硬件设备
 * 用于从Power Off / Power Down状态恢复到正常接收数据状态
 */
void cc2420_startup( TCc2420Driver * cc );

/* 关闭cc2420硬件设备
 * 从正常接收发送数据状态进入Power off/Power down状态
 * 注意同时需要更改driver内部的状态标志
 */
void cc2420_shutdown( TCc2420Driver * cc );

/* 休眠 */
void cc2420_sleep( TCc2420Driver * cc );

/* 从休眠中唤醒 */
void cc2420_wakeup( TCc2420Driver * cc );

// @TODO: 20061026
// huanghuan: you should use the following setchannel
//void cc2420_setchannel( TCc2420Driver * cc, uint8 channel );
// not this one:
void cc2420_setchannel( TCc2420Driver * cc);

int8 cc2420_write( TCc2420Driver * cc,TCc2420Frame frame, int8 length,uint8 opt);
uint8 cc2420_read( TCc2420Driver * cc,TCc2420Frame * frame,uint8 len,uint8 opt);

void cc2420_receive_on(TCc2420Driver * cc);



void cc2420_receive_off(TCc2420Driver * cc);


//void cc2420_interrupt_init( void );
//void cc2420_event_handler(void);

#endif /* _HAL_CC2420_H_ */
