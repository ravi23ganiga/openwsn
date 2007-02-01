/*************************************************************
 * @author zhangwei on 2005-07-19
 * @note: Interface file CRC check module.
 *	thanks Chen Ruijie to give the original source code (first version). 
 * 
 * @history
 * 2005-07-19 first created by zhangwei
 * @modified by zhangwei on 20061030
 * revised and released as part of RTL
 ************************************************************/

#ifndef _RTL_CRC_H_4278_
#define _RTL_CRC_H_4278_

/*  CRCProduce
*********************************************************************************************************
* Description: 产生CRC校验字段.
* Arguments  : buf,  帧缓冲区的首指针，指向第一个待校验的字符
*                     number, 待校验帧的字符个数
* Returns    :   fcsOut, CRC校验码
*                    另外，本函数在输入buf缓冲区的末尾添加了CRC校验码  
* Complete date  : 2005.5.20
                   Modified by xxx at xxxx.xx.xx:
*********************************************************************************************************
*/
extern unsigned short crc_produce(unsigned char *buf, unsigned short number);

/*  CRCCheck
*********************************************************************************************************
* Description: CRC校验函数.
* Arguments  : buf,  帧缓冲区的首指针，指向第一个已校验的字符
*                     number, 已校验帧的字符个数，可以包括CRC校验码
* Returns    :   fcsOut, CRC校验码
*                    另外，当检验帧包含CRC校验码时，CRC校验正确时输出fcsOut为0  
* Complete date  : 2005.5.20
                   Modified by xxx at xxxx.xx.xx:
*********************************************************************************************************
*/
extern unsigned short crc_check(unsigned char *buf, unsigned short number);

#endif