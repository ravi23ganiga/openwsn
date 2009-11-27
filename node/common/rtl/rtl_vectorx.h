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
 * TiVectorX(vex) �� TiVector(vec)��ͬ�������˶�ÿ��item��state��֧�֣��ɸ���state
 * �жϸ�item�Ƿ��Ѿ��������ݡ���һ���Կɷ�����߼���hashmap, tree, list, heap��
 * ���ݽṹ��ʵ��
 */
typedef struct{
	uint16 size;
	uint16 itemlen;
	uint16 capacity;
	uint16 count;
	uint16 offset;
}TiVectorX;

TiVectorX *	vex_construct( void * buf, uint16 size, uint16 itemlen );
void        vex_destroy( TiVectorX * vex );

uint16       vex_capacity( TiVectorX * vex );
uint16       vex_count( TiVectorX * vex );
bool        vex_empty( TiVectorX * vex );
bool        vex_full( TiVectorX * vex );

bool        vex_apply( TiVectorX * vex, uint16 * idx );
void        vex_release( TiVectorX * vex, uint16 idx );
bool        vex_isavailable( TiVectorX * vex, uint16 idx );	

//uint8 *     vex_states( TiVectorX * vex );
uint8       vex_states( TiVectorX * vex, uint16 idx );
void *      vex_items( TiVectorX * vex );
void *      vex_getbuf( TiVectorX * vex, uint16 idx );
bool        vex_get( TiVectorX * vex, uint16 idx, void * buf );
bool        vex_set( TiVectorX * vex, uint16 idx, void * item );
bool        vex_put( TiVectorX * vex, void * item );
bool        vex_remove( TiVectorX * vex, uint16 idx );
void        vex_clear( TiVectorX * vex );
void        vex_swap( TiVectorX * vex, uint16 i, uint16 j );

#ifdef __cplusplus
}
#endif

#endif

