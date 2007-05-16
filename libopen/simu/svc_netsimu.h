/*****************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007,2008 zhangwei (openwsn@gmail.com)
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

#ifndef _SVC_NETSIMU_H_7388_
#define _SVC_NETSIMU_H_7388_

//----------------------------------------------------------------------------
// @author zhangwei on 20070515
// @note
// 该模块用于提供一系列接口，使得host上的service/ui程序可以从网络
// 读取数据或者通过网络发送数据。但本模块提供的不是对真实网络的read/write，而是
// 虚拟了一个网络。读写的数据通过随机方式生成。
//
// @modified by zhangwei on 20070515
//	first created
//----------------------------------------------------------------------------

#include "..\configall.h"
#include "..\foundation.h"

// 模拟一个网络结点
// x,y,z		该结点的位置
// timestamp	当前时刻
// sen_temprature	当前温度值
//
typedef struct{
  uint16 id;
  uint8  state;
  uint32 x,y,z;
  uint32 timestamp;
  uint8  txlen;
  uint8  rxlen;
  char   txbuf[64];
  char   rxbuf[64];
  uint32 sen_temprature;
  uint32 sen_strain; 
  uint16 history_length;
  uint32 history_timestamp[256];
  uint32 history_tempvalue[256];
  uint32 history_strainvalue[256];
}TOpenNode;

// 模拟一个网络
//
typedef struct{
  uint16 root;
  uint32 senseing_radius;
  uint32 comm_radius;
  TOpenNode nodes[100];
}TOpenNetwork;

// 创建一个TOpenNetwork对象并执行必须的初始化
TOpenNetwork * net_create();

// 释放一个网络对象
void net_free( TOpenNetwork * net );

// 打开一个网络对象，准备读写
int net_open( TOpenNetwork * net );
net_close( TOpenNetwork * net );

// 从网络中读一批数据上来，每次read调用要么返回0，表示没有读到数据，要么通过buf返回一个数据包
// 返回值表示该数据包的长度。网络对象不管数据包的格式和解释。你需要自己解释。
int net_read( TOpenNetwork * net, char * buf, uint8 capacity );

// 写一个数据包给网络，也就是发送数据包出去
int net_write( TOpenNetwork * net, char * buf, uint8 len );

// 暂时不用
void net_evolve( TOpenNetwork * net );

// 暂时不用
void net_probe( TOpenNetwork * net )

// 获取网络中结点个数，常与get_node_desc配套使用，完成网络结点的循环遍历
int net_get_node_count( TOpenNetwork * net )
TOpenNode * net_get_node_desc( TOpenNetwork * net, uint16 id )

// 获取某结点的邻结点id序列
// 暂时不用
int net_get_neighbor_nodes( TOpenNetwork * net, uint16 id, uint16 * buf, uint16 capacity );

// 在仿真中生成一个网络，包括生成网络中的所有结点，他们的位置数据等
int net_generate( TOpenNetwork * net );

// 将网络信息保存到文件中
int net_load( TOpenNetwork * net, char * filename );
int net_save( TOpenNetwork * net, char * filename );

// 暂时不用
int net_sleep( TOpenNetwork * net );

// 暂时不用
int net_wakeup( TOpenNetwork * net );

//----------------------------------------------------------------------------
#endif
