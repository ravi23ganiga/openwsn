#include "rtl_configall.h"
#include "rtl_foundation.h"
#include "rtl_cache.h"
#include "..\hal\hal_debugio.h"
#include <string.h>

static inline uintx _cache_displace( TiCache * cache, char * item, uint16 itemlen, uintx idx );


TiCache * cache_construct( void * mem, uint16 memsize, uint16 itemsize, uintx capacity )
{
	uintx i;
	TiCache * cache;

	memset( mem, 0x00, memsize );
	cache = (TiCache *)mem;
	cache->itemsize = itemsize;
	cache->capacity = capacity;
	for (i=0; i<capacity; i++)
		cache->lifetime[i] = 0;

	return cache;
}

void cache_destroy( TiCache * cache )
{
	return;
}

/* _cache_hit()
 * Check whether the input item is hitted in the cache. Return true if hitted.
 * 
 * @return
 *	true                     when hitted. (found the item in the cache). *idx is 
 *                           the item index in the cache.
 *  false                    not hitted. *idx is undefined in this case. 
 */
bool cache_hit( TiCache * cache, char * item, uint16 itemlen, uintx * idx )
{
	bool found, same;
	uintx i;
	uint16 j;
    char * curitem;

	// searching for the specified item in the cache
  
	found = false;
	curitem = (char*)cache + sizeof(TiCache);
	for (i=0; i<cache->capacity; i++)
	{    
		same = true;
		for (j=0; j<itemlen; j++)
		{
			if (curitem[j] != item[j])
			{
				same = false;
				break;
			}
		}

		/* If the item already inside the cache, then refresh the lifetime value and reset
		 * it to the youngest */
		if (same)
		{   
	
			found = true;
			if (idx != NULL)
				*idx = i;
			cache->lifetime[i] = CONFIG_CACHE_MAX_LIFETIME; 
			break;
		}

		curitem += cache->itemsize;
	}

	return found;
}

/* Put the specified item into the cache. If the cache is full, then the eldest 
 * item will be displaced with the new one. The eldest frame is with the smallest
 * lifetime value.
 * 
 * @return 
 *	the index of the new item in the cache.
 */
uintx _cache_displace( TiCache * cache, char * item, uint16 itemlen, uintx idx )
{
	uintx replace=0, i;
	char * curitem, * repitem = NULL;

	if (cache->lifetime[0] == 0)
	{
		curitem = (char*)cache + sizeof(TiCache);
		repitem = curitem;
		replace = 0;
	}
	else{
		replace = 0;
		repitem = (char*)cache + sizeof(TiCache);
		curitem = repitem + cache->itemsize;
		for (i=1; i<cache->capacity; i++)
		{
			if (cache->lifetime[i] == 0)
			{
				repitem = curitem;
				replace = i;
				break;
			}
			if (cache->lifetime[i] < cache->lifetime[replace])
			{
				repitem = curitem;
				replace = i;
			}
			curitem += cache->itemsize;
		}
	}

	memmove( repitem, item, itemlen );
	cache->lifetime[replace] = CONFIG_CACHE_MAX_LIFETIME;

	return replace;
}

/* Searching for the specified frame in the cache. It hitted/found, then returns 1.
 * Return 0 when not hitted. 
 * 
 * If not hitted, then this function will also put the new item  into the cache.
 *
 * @return
 *	true					when successfully found the item in the cache (hit the cache)
 *  false                   
 */
uintx cache_visit( TiCache * cache, char * item, uint16 itemlen )
{
	uintx i, idx=0;
	char * curitem;

	rtl_assert( itemlen <= cache->itemsize );
	if (cache_hit(cache, item, itemlen, &idx))
	{
		return true;
	
     }
	_cache_displace( cache, item, itemlen, idx );
    
	curitem = (char*)cache + sizeof(TiCache);
	for (i=0; i<cache->capacity; i++)
	{
		if (cache->lifetime[i] > 0)
			cache->lifetime[i] --;
		curitem += cache->itemsize;
	}

	return false;
}
