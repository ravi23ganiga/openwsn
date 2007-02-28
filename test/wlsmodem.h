//----------------------------------------------------------------------------
// @author zhangwei on 2006-10-10
// @note rfmodem V 1.0
//
// 一个实用的wireless modem / rfmodem / wlsmodem 测试程序
// 演示如何使用wlscomm，同时也测试wlscomm
//----------------------------------------------------------------------------

#ifndef _WLSMODEM_H_6753_
#define _WLSMODEM_H_6753_

#include "..\foundation.h"
#include "..\global.h"

/* PLC <==> UART frame format(PHY Layer):
 * [ESC Header Flag 1][Length 1][Frame Control 1][Address 2][Data n]
 * 
 * ESC header flag byte and Length byte belong to the PHY layer. 
 * SioComm service functions like the Data Link layer. It will analyze the PHY 
 * frame format and discard some un-necessary information. the application layer
 * will got the following format in the end: 
 * [Frame Control 1][Address 2]
 * 
 * In the two byte of address, the first one is the PanId/Network Id, while the 
 * next byte is the Address in that network. 
 *
 * Frame Control Byte
 * 		0x80	data frame or command frame. 0x80 means command frame
 * 		0x40	...
 * 		0x20	...
 * 		0x01	standard data frame or sio-encapsulated data frame
 * 				0x01 means this frame is encapsulated by SioComm. so you cannot
 * 				trust the frame control byte and the address byte. 	
 *  
 * @modified by zhangwei on 20061024
 * the following PANID and Address are reserved for future use or for special use.
 * 		0x00
 * 		0x01	
 * 		0xFF
 * 		0xFE
 * 		127
 * 		128
 * 
 * you shoul NOT use the above number as your PANID or device address.
 */

void wlsmodem_start( void );

#endif /* _WLSMODEM_H_6753_ */
