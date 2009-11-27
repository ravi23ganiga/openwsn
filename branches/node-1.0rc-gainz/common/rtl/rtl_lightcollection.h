#ifndef _RTL_vexTORX_H_1869_
#define _RTL_vexTORX_H_1869_

/* rtl_vectorx
 * an extension of the standard vector (TiVector).
 *
 * @state
 *	finished. not fully tested
 *
 * @author zhangwei in 200904 
 *	- first created. 
 */ 

#define VECTORX_HOPESIZE(itemsize,capacity) (sizeof(TiVectorX) + itemsize + itemsize*capacity)
#define VECTORX_STATE_AVAILABLE 0
#define VECTORX_STATE_ACTIVE 1

#define vex_isavail(vex,idx) vex_isavailable(vex,idx)
#define vex_isused(vex,idx) (!vex_isavailable(vex,idx))

/* reference
 * The Collections Framework,
 * http://java.sun.com/j2se/1.4.2/docs/guide/collections/index.html
 */

#ifdef __cplusplus
extern "C" {
#endif

/* TiVectorX
 * memory map:
 *  ---------------------
 *  Header
 *  ---------------------
 *  Item State Table      (each state occupies only 1 byte)
 *  ---------------------
 *  Item Data Table
 *  ---------------------
 *
 * TiVectorX(vex) 与 TiVector(vec)不同，增加了对每个item的state的支持，可根据state
 * 判断该item是否已经放了数据。这一特性可方便更高级的hashmap, tree, list, heap等
 * 数据结构的实现
 */
typedef struct{
	uintx size;
	uintx itemlen;
	uintx capacity;
	uintx count;
	uintx offset;
}TiVectorX;

TiVectorX *	vex_construct( void * buf, uintx size, uintx itemlen );
void        vex_destroy( TiVectorX * vex );

uintx       vex_capacity( TiVectorX * vex );
uintx       vex_count( TiVectorX * vex );
bool        vex_empty( TiVectorX * vex );
bool        vex_full( TiVectorX * vex );

bool        vex_apply( TiVectorX * vex, uintx * idx );
void        vex_release( TiVectorX * vex, uintx idx );
bool        vex_isavailable( TiVectorX * vex, uintx idx );	

//uint8 *     vex_states( TiVectorX * vex );
uint8       vex_states( TiVectorX * vex, uintx idx );
void *      vex_items( TiVectorX * vex );
void *      vex_getbuf( TiVectorX * vex, uintx idx );
bool        vex_get( TiVectorX * vex, uintx idx, void * buf );
bool        vex_set( TiVectorX * vex, uintx idx, void * item );
bool        vex_put( TiVectorX * vex, void * item );
bool        vex_remove( TiVectorX * vex, uintx idx );
void        vex_clear( TiVectorX * vex );
void        vex_swap( TiVectorX * vex, uintx i, uintx j );

#ifdef __cplusplus
}
#endif

#endif

