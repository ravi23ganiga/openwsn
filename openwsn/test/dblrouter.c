
#include "..\foundation.h"
#include "..\global.h"
#include "dblrouter.h"

#define BUFFER_CAPACITY 128

typedef struct{
  uint8 addrfrom;
  uint8 addrto;
}_TRouteTableItem;

typedef _TRouteTableItem TRouteTable;

static TRouteTable g_routetable[2] = {{1, 3}, {3, 1}};

// rxqueue: the frame to this node
// txqueue: the frame received to be forwarded to other nodes
// ackqueue: the frame sent which are waiting for ACK/NAK frame
//
static void * rxqueue;
static void * txqueue;
static void * ackqueue;

int16 dblrouter_run( void )
{
	// initialize the router table.
	// the frame received from node 1 will be forwarded to node 3
	// and the frame received from node 3 will be forwarded to node 1
	//
	g_routetable[0].addrfrom = 1;
	g_routetable[0].addrto = 3;
	g_routetable[1].addrfrom = 3;
	g_routetable[1].addrto = 1;

	char buf[MAX_FRAME_LENGTH];

	while (true)
	{
		// check whether there's some frame pending in the txqueue wait for being
		// sent. if this's true, then try to sent them out. for collision avoidance
		// reasons, you'd better send one frame only each time. This will give
		// the local node the chance to RX mode and to wait for possible response.
		//
		if (!rque_empty(txqueue))
		{
			// get one frame from the txqueue and try to send it out.
			// after sending successfully, you should check whether this frame
			// is a ACK/NAK frame or data frame. if this is a data frame, then
			// you should move it into the ackqueue and waiting for the ACK from
			// it destination node.
			//
			frame = rque_front(txqueue);

			if (phy_send(frame, len)
			{
				if (frame->isdata)
				{
					// move the frame from txqueue to ackqueue
					frame->retry = 0;
					frame->timeout = 1000;
					rque_moveto( ackqueue, frame, txqueue )
				}
				rque_pop();
			}
		}

		// visit each frame in the ackqueue and check their "retry" flag and
		// "timeout" flag. if livetime equals 0, then move this frame into txqueue
		// and try to send it again.
		// two cases will be lead the frame deleted from the ackqueue:
		// - received an corresponding ACK frame
		// - when time out or received an NAK frame, the frame will be sent again
		//  and "retry" also increase. if retry equals 3, then the system will
		//  giveup sending again and simply remove this frame from the ackqueue
		//
		frame = rque_front(ackqueue);
		while (frame != NULL)
		{
			frame->timeout --;
			if (frame->timeout <= 0)
			{
				rque_transfer( txqueue, frame, ackqueue );
				frame->retry ++;
			}
		}

		// receiving
		// if the received frame is ACK, then try to find the corresponding frame
		// in the ackqueue and delete it from queue. if cannot find it, then simply
		// ignore the ACK frame.
		//
		// no NAK frame support now. the retry will depend on timeout mechanism
		// only in this implementation
		//
		len = phy_recv( buf, MAX_FRAME_LENGTH );
		frame = create a new frame structure from the frame data in the buffer
		if (len > 0)
		{
			if (frame->type = ACK FRAME)
			{
				searching for the frame in the ackqueue.
				if (find) {
					rque_delete( this frame from queue )
				}
			}
			else if (frame->type == NAK)
			{
				searching for the frame in the ackqueue.
				if (find) {
					rque_transfer( txqueue, frame, ackqueue );
				}
			}
			else{
				if (frame->addto = this node)
				{
					rque_push( rxqueue, frame );
				}
				else{
					// set new destination address of this frame
					frame->addrto = g_routetable[frame->addrframe]->addrto;
					rque_push( txqueue, frame );
					create a ACK frame and push into the txqueue
				}
			}
		}

		// do processing with the frame to this node
		if (!rque->empty())
		{
			frame = rque_front(rxqueue);
			// deal with the frame received by this node
			// you can add your own processing here

			// delete this frame from the
			rque_pop(rxqueue);
		}
	}





while
	// sending logic
	if txqueue has frames
		send frame
		if this is data frame,
			then move it to ackqueue
		end
	end

	visit ackqueue and dec their lifetime
	if lifetime < 0 
		if repeat count still > 0
			insert this pakcet into the queue
			send count ++
		else
			delete this frame
		end
	end
	
	// receiving logic
	check received
	if no packet received then skip the following and continue
	if ack
		delete responsing frame in ackqueue 
		if cannot find it, then ignore this ack
	else
		if this item is for current node self
			move this node in the rxqueue
			notify upper level
		else
			place a ack into the txqueue at the head position
			place the packet in the txqueue and changed the address by searching for the router table
		end
	end
end




allocator = arrayqueue

txqueue
rxqueue
ackqueue

multique_alloc(id)
         push
		 pull
multique_insert


typedef struct{
	txqueue
	rxqueue
	ackqueue
	freeque
		}TMultiQueue


pool->newitem
pool->freeitem

arque->pushfront
arque->pushback = push
arque->popfront = pop
arque->poptail 
arque->front
arque->back/tail


arque->next
arque->prev
arque->insert

// this is the basic unit in rque

typedef struct{

}TFrameQueueItem;

typedef struct{

}TPtrQueue;

TCharQueue
TIntQueue
TGenQueue
TBlockQueue

TBlockAllocator
block_allocator_t
blkalloc_acquire/alloc
blkalloc_release/free

blkpool_create
blkpool_free
blkpool_construct
blkpool_destroy
blkpool_apply
blkpool_release
blkpool_count
blkpool_item[idx]
blkpool_address(id)

// AppPacket will be the queue item content
typedef struct{
  id
  length
  type
  data
  retry
  timeout
}TAppPacket;

createfrom( TOpenFrame)
create_ack
create_nak
phyframe

因为只需要记录索引，所以只要利用TCharQueue即可
然后调用blkpool->addr(id)即可得到memory address
这样，队列操作都是基于char/int，效率非常高
但这个pool就要好好做

typedef TBlockQueue TCharQueue

TQueueItem
}  includes an TOpenFrame and a sereis of function
   used in openwsn only

rframe_create
rframe_buffer/getdata/

rframe->addrfrom, addrto, type, data, length

rque_push/pop/front/next
rque
