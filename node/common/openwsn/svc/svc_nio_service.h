/**
 * Q: Introduction of the "nio" architecture?
 * R: "nio" means network I/O architecture. This architecture is designed for memory 
 * highly restricted embedded systems. Each time a new frame arrives, it will be 
 * push into the rxque inside acceptor component, or fetched from the transceiver 
 * into the rxque by the acceptor. Then the io service manages the frame dispatching.
 * All the network components must provide an process like function to be used by 
 * the dispatcher, and all of them are organized as a tree.
 * 
 * Q: The philopsophy of TiNioService (or nio) design?
 * R: TiNioService helps to implement a component based network protocol stack
 * with necessary auxiliary components fastly, so the design of TiNioService itself
 * should simplify the developing of other network components as much as possible.
 * The TiIoService itself maybe complicated but the others should be simple.
 * 
 * For example, the other comoponents only needs to know rxque and txque to receive
 * and send frames. All the other details should be done by the nio service. 
 * 
 * On the other hand, in order to simplify the implementation of the nio service
 * itself, we agree that each time the other network compnent can send/recv frames 
 * to/from txque/rxque only 1 frame. This is used to leave the chance for nio service
 * itself to run or else the nio service havn't chance to process the frames.
 */
 
/** 
 * @reference
 * - Apache MINA Introduction (with figures), http://mina.apache.org/mina-based-application-architecture.html 
 * - 顾锋磊, 软件工程师, IBM, 使用事件驱动模型实现高效稳定的网络服务器程序--几
 *   种网络服务器模型的介绍与比较 , 2010.10, 
 *   http://www.ibm.com/developerworks/cn/linux/l-cn-edntwk/index.html?ca=drs-
 * - Linux 网络栈剖析 -- 从 socket 到设备驱动程序, 
 *   http://blog.chinaunix.net/u1/43045/showart_373164.html
 * - windows网络模型学习--WINDOWS和LINUX开发模型比较, 2009, 
 *   http://lin-style.javaeye.com/blog/343477
 * - LINUX网络协议栈实现分析（－）SKBUFF的实现, 
 *   http://www.kernelchina.org/linuxkernel/sk_buff.pdf
 * - Linux内核协议栈源码分析, 
 *   http://wenku.baidu.com/view/5ca649ec0975f46527d3e131.html (299 Pages)
 *   or http://ishare.iask.sina.com.cn/f/10220414.html
 */

/**
 * TiNioService
 * TiNioService denotes Network I/O Service, which provides the high level interface 
 * of networking functions in the architecture. 
 * 
 * - Other modules can operate the network through the TiNioService component. It's 
 *   the abstraction of the wireless network. NioService contains the network protocol 
 *   and service implementation inside.
 * - TiNioService performs network initialization and startup, so that the network
 *   based developing can be simplified.
 * - The TiNioService can be reconfigured through macros to adapte to some typical 
 *   network applications.
 * - Manages the dispatchers.
 * - Manages the topology data structure and channel data.
 */

typedef struct{
  TiNioAcceptor * nac;
  TiNioDispatcher * dispatcher;
  //TiNioFmqueRwAdapter, TiNioRwQueue 
  TiNioFrameQueue4Rw * rw;
}TiNioService;



application: addon endpoint

nioservice:  port

nioservice: channel

nio_service_open
nio_service_close
nio_service_evolve
nio_service_register( endpoint, component/addon );

init
	curlayer
	
extract_dispatch_id( frame ) = endpoint
encapsulate
assemble
parse
	
application
{
	nios_register( addon1 )
	nios_register( addon1 )
	nios_register( addon1 )
	nios_register( addon1 )

	you can also use nio_handler to move frame into an new queue for application layer
	so that you can use read/write interface
	
	nios_evolve( nio );
}




