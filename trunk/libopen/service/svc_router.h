//----------------------------------------------------------------------------
// @author zhangwei on 20061010
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// TSinkRouter object
// this object is an node-driven query-based routing protocol. it's very small
// and simple to be implemented on most of hardware device, including from the 
// host computer, sink node to even an general sensor node. 
//
// sink router is running on the sink node.  for a general sensor node, it also 
// has routing functionality. that is handled by TOpenRouter object rather than 
// TSinkRouter object. TSinkRouter is more powerful and complex than TOpenRouter.
// further more, in an entire network, the TSinkRouter needs the cooperation 
// of TOpenRouter or else the whole network can not work.
//
// however, the SinkRouter is not an efficient one. it left a lot of space for
// you to improve it.
//----------------------------------------------------------------------------

typedef struct{
  //TSioComm * sio;
}TSinkRouter;

TSinkRouter * sinkrouter_create();
void sinkrouter_destroy( TSinkRouter * rt );
void sinkrouter_evolve( TSinkRouter * rt );


