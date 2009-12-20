/******************************************************************************
 * rtl_random
 * This module implements a random generator.
 *
 * @author zhangwei(TongJi University) on 20070130
 *	- first created.
 * 
 * @history
 * @modified by zhangwei on 20091102
 *	- revision
 * @modified by Sun Qiang on 20091112
 *  - revision
 * @modified by Sun Qiang on 20091123
 *  - remove higer statistic functions to new module rtl_stochastic
 *
 *****************************************************************************/ 
#ifndef _RTL_RANDOM_H_3425_
#define _RTL_RANDOM_H_3425_

#include "rtl_foundation.h"
#include <stdlib.h>

/* this is the default initial seed. You can change it by calling rand_init(newseed)  */
#define RANDOM_INITIAL_SEED 0xB6A3

/* attention:
 * assume the target device supports 16-bit operations.
 */

#define rand_init(n) rand_open(n)
#define rand_next() rand_read()

/***************************************************************************** 
 * Basic functions of generating random nums
 *****************************************************************************/
void rand_open( uint16 n );
uint16 rand_read();
void rand_close();

/* Generate 8-bit,16-bit,32-bit random nums
 */
uint8 rand_uint8( uint8 n );
uint16 rand_uint16( uint16 n );
uint32 rand_uint32( uint32 n );

/* Generate a random float/double random num in [0,1) 
 */
float rand_float();
double rand_double();

#endif

