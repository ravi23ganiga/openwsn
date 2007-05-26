
#ifndef _SVC_NETDATA_H_8989_
#define _SVC_NETDATA_H_8989_

//----------------------------------------------------------------------------
// @author zhangwei on 20070515
// @note
//	规定了网络数据包的数据格式
//  提供了在结构类型数据和buf类型数据之间转换的工具函数
//
// @modified by zhangwei on 20070515
//	first created
//----------------------------------------------------------------------------
#include "svc_configall.h"
#include "svc_foundation.h"

#ifndef DLLAPI
#error "please define macro DLLAPI first"
#endif
/*
void test2();


#define DLLAPI extern "C" _declspec(dllexport)



// TNetAppData是net_read()/net_write()中数据的格式
// net_read()/net_write()并不管数据的格式，本结构来解释数据格式以及每个成员变量的含义
//
typedef struct{
  //unsigned short nodeid;
  //unsigned int x,y,z;
  //unsigned int timestamp;
  //unsigned int sen_temprature;
  //unsigned int sen_strain;
  unsigned char reserved[4];
}TNetAppData;  


// 在TNetAppData结构和read/write所要求的{buf,capacity}缓冲区之间进行格式转换
// assemble用于将struct中的数据逐个字节memmove到buf缓冲区中紧密排放
// disassemble执行反向操作

DLLAPI int __stdcall net_assemble33332( void  * payload, char * buf, uint16 capacity );

DLLAPI int _stdcall net_assemble( TNetAppData * payload, char * buf, uint16 capacity );
DLLAPI int _stdcall net_disassemble( char * buf, uint16 len, TNetAppData * payload );
*/
//----------------------------------------------------------------------------
#endif
