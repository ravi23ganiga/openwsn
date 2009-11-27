/*
 * rtl_stack.h
 * this version of stack data structure is implemented by using TiVector
 *
 * @author zhangwei in 200803
 * @modified by zhangwei in 200803
 */

#ifndef _RTL_STACK_H_5998_
#define _RTL_STACK_H_5998_

#include "rtl_vector.h"

#define TiStack TiVector

#define stk_construct(buf,size,itemsize) vec_construct((buf),(size),(itemlen))
#define stk_destroy(stk) vec_destroy(stk)
#define stk_push(stk) vec_pushfront(stk)
#define stk_pop(stk) vec_popfront(stk)
#define stk_top(stk) ((stk->count>0) ? vec_getbuf(stk,stk->count) : NULL)
#define stk_empty(s) vec_empty(s)
#define stk_full(s) vec_full(s)

#endif /* _RTL_STACK_H_5998_ */
