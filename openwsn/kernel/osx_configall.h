#ifndef _OSX_CONFIGALL_7898_
#define _OSX_CONFIGALL_7898_

#include "../configall.h"


/* controls whether to use malloc() to allocate memory to hold the osx kernel */
#define CONFIG_OSX_DYNAMIC_MEMORY 
#undef CONFIG_OSX_DYNAMIC_MEMORY 

/* configure the maximum event count in the system event queue */
#define CONFIG_OSX_QUEUE_CAPACITY 8

/* configure how many objects can be placed in the dispatcher list */
#define CONFIG_OSX_DISPATCHER_CAPACITY 8

/* enable debug agent */
#define CONFIG_DBA_ENABLE

#endif
