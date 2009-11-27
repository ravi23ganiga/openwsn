#ifndef _CACHE_H_4638_
#define _CACHE_H_4638_

/*******************************************************************************
 * rtl_cache
 * This module implements a simple but very useful object -- TiCache. The eldest
 * item will be replaced according to the current algorithm.
 *
 * @todo
 * performance tuning in the future
 *
 * @author zhangwei(TongJi University) in 200911
 *	- first created.
 ******************************************************************************/ 

#define CONFIG_CACHE_CAPACITY 8
#define CONFIG_CACHE_MAX_LIFETIME 0xFF

#define CACHE_HOPESIZE(itemsize,capacity) (sizeof(TiCache) + itemsize*capacity)

/* TiCache
 * An simple cache. The eldest item will be displaced first. 
 *
 * lifetime = 0 means this is an empty item in the cache. The bigger the lifetime, 
 * the newer the lifetime.
 */
typedef struct{
	uint16 itemsize;
	uintx  capacity;
	uint8  lifetime[CONFIG_CACHE_CAPACITY];				
} TiCache;

TiCache * cache_construct( void * mem, uint16 memsize, uint16 itemsize, uintx capacity );
void cache_destroy( TiCache * cache );
bool cache_hit( TiCache * cache, char * item, uint16 itemlen, uintx * idx );
uintx cache_visit( TiCache * cache, char * item, uint16 itemlen );

#endif
