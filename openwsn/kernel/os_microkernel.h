#ifndef OS_MICROKERNEL_H_
#define OS_MICROKERNEL_H_

/*
 */

typedef uint8 TMknEvent;
typedef int8 (* TMicroKenelHandler)(void * owner, void * source);

typedef struct {
	TMknEvent eventid;
	uint8 next;
	TMicroKernelHandler handler;
	void * owner;
} TMknItem;

typedef struct {
	TMknItem relations[10];
	TMknEvent eventid;
	void * eventsource;
} TMicroKernel;

static TMicroKernel g_mkn;

#define mkn_trigger(e,source) mkn_execute(e,source)
#define mkn_post(e,source) mkn_publish(e,source)

void mkn_init() {

}

void mkn_trigger( TMknEvent e, void * source ) {

}

void mkn_publish(TMknEvent e, void * source) {

}

void mkn_subscribe(TMknEvent e, handler, void * handler_owner) {

}

void mkn_execute(TMknEvent * e, void * source) {

}

void mkn_process() {
	if (g_mkn.e != INVALID) {
		mkn_execute(g_mkn.eventid, g_mkn.eventparam);
	}
}

#endif /*OS_MICROKERNEL_H_*/
