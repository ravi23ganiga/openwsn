/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/
 
 apc_init
 apc_attach( addon )
 apc_detach( addon )
 apc_evolve()
 
 /**
  * TiApplication
  * The application component maintains an component tree and data frame dispatching 
  * rules. 
  */
  
apc_init()
{
	tree root is always the network acceptor
	and the application component itself will do frame dispatching between mac component
		and time sync component
	mac child is frame dispatching 
	and this dispatching dispatch frames to other components
	and ontop of net layer, also an dispatching
	
	so totally 3 dispatching frame possible. but the above two may be one or less.
}
 
tree�εĽṹ�ƺ����ǲ��ܺܺõĽ��network protocol stack�������
treeӦ������osx��ά������Ϻ�, tree��event routing path���Ժܺý��

TiFrameDispatcher����
���е�mac,net,localization,high level time sync����framedispatcherǰ����ƽ�ȵģ�û�в��֮��
������������ݵĶ�̬��������У������Ǿ�̬�ṹ��

����Ҳ���Ա�����ߵ������
ϵͳ�����ṩ���ָ�����FrameDispatcher,��Ϊ���������һЩ�ػ��Ĺ��ܡ���Ȼ������ͨ�û�������
TiMacBasedFrameDispatcher
TiNetBasedFrameDispatcher
TiNetAcceptorBasedFrameDispatcher
���Ǹо��е㸴�ӣ���Щ����Ӧ���Ǿ��������������Ҫ��Ϊһ�����������ͳ��֣���ά��TiFrameDispatcher��ͨ����

TiFrameDispatcher

open( choice_offset )
attach
detach
evolve

��ΪЧ�ʲ�����ؼ��ģ��������ǿ��Թ涨��
ÿ�ζ����봦����rxque�е�front�󣬲ſ�������һ���Ķ�����������session����ִ�С�

application_evolve()
{
	dispatcher d[3] = {TiNetAcceptorBasedFrameDispatcher, TiMacBasedFrameDispatcher, TiNetBasedFrameDispatcher }
	f = acceptor.front of rxque;
	do {
		d = dispatcher[i];
		d.dispatch( f );
		if f is not the front frame then 
			break;
		endif
		i++ until reach the last dispatcher
	}
}

����������ƹ̶�����ֻ����������ʵ����Ҳ���ã��������Բ����

apc_attach( layer=0,1,2, component )
dispatcher_attach / dpt_attach

 
 
rxque������ܺõĽ����
����txque�������أ���Ҫ����װframe�� ��ô�죿ÿ��component������ȷ֪���Լ���һ�����ĸ������ڸ�����
�����Ǹ߶�������ġ����ĸ�������ɵײ㴫���ϲ�Ӧ�ò�Ҫȥ��

=> ��ȫ����txque.tail

apc���txque.tail item, �������frame�ĵ�ǰlayer������ײ�---Ӧ����acceptor��frame�������жϣ�
��ô��������ͨ�������ֱ����acceptor��ȫ���ܡ�������������ÿ��ֻ�ܷ���1��frame
����ʵ�����ǲ��еģ���Ϊ����һ�η���Ҫ�󷢶��������ĳЩ�ӽ�����broadcast����Ϊ

�ǲ�����Ҫ��Ӧ�ò㵥��������ʱ��txbuf? 

���У�������������޷�����һ��frame����������Ϣ�������������������
dispatcher����dispatch֮�⣬��Ҫ�����������ķ���ֵ���Ƿ���Ҫ��component list�м������move
������dispatchһ��֮��ͽ��� 
 

/* TiSystem is the highest level object in an application. it has the 
 * following functions:
 * - receive messages from other objects and environment and call corresponding 
 *	 message handlers. the message source include other objects, interrupt service
 *	 handler, RTC, Timer, bytes arrived from UART, packets arrived from wireless,
 *   scheduler object.
 * - call the evolve() function of other objects. this will drive the state 
 *   transfer of other objects.
 * - call the extern function/class, which is the entrance of user defined functions.
 *	 (like the main() function which is the entry function of the whole application)
 *
 * the openwsn developer must provide it's own app class. this class is named as 
 * TiApplication, and it should provide three interface functions:
 * - construct();
 * - destroy();
 * - evolve();
 */

typedef struct{
}TiSystem;

TiApplication

TiPFunConstruct
TiPFunDestroy
TiPFunEvolve
TiPFunFilterExecute

/* g_system: pre-defined global object */
g_tsys.attach( &startobject )


svc_construct()
svc_evolve()
svc_destroy()

typedef struct
}TiObjectInterface;

run()
{
	g_system.construct
	g_system.put(udo);
}

g_system.registerservice( udo_create, udo_construct, udo_evolve, udo_destroy, udo_free );

udo_create
udo_construct
udo_evolve
udo_destroy
udo_free




/* @attention
 * in OOP design and implementation, every object shuold be able to contain their 
 * message queue and has message handlers. however, in openwsn project, only the 
 * TiSystem object has message queue. this is intended to simplify the system 
 * design and data route.
 */ 

/* @attention
 * msgsend() is elaborately designed and it can be called inside ISR functions 
 * and any other objects. 
 */ 
tsys_create()
tsys_construct()
tsys_destroy()
tsys_free()
tsys_run()
tsys_trigger(msg)
tsys_msghandler()
tsys_msgsend
tsys_msgrecv

int main()
{
	g_tsystem.construct();
	g_tapp.construct();
	g_tsystem.run();
	g_tapp.destroy();
	g_tsystem.destroy();
}
