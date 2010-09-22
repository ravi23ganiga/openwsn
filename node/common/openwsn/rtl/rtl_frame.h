#ifndef _RTL_FRAME_H_5768_
#define _RTL_FRAME_H_5768_
/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

#include "rtl_configall.h"
#include "rtl_foundation.h"

/*******************************************************************************
 * rtl_frame
 * TiFrame object to help manipulating nested layer frame structures.
 *
 * @author zhangwei in 2005
 * @modified by zhangwei in 200604
 *  - revision
 * @modified by openwsn in 20100706
 *  - full revised. compile passed. not fully tested yet.
 * @modified by zhangwei on 2010.08.07
 *  - revision
 ******************************************************************************/

/** 
 * CONFIG_FRAME_LAYER_CAPACITY 
 * defines the maximum layers the frame support. currently, the most complicate frame 
 * may be the following:
 * 
 *  PHY Frame[ MAC Frame [LLC Frame [NET Frame [TCP Frame [Middleware Frame [App Frame]]]]]]
 * 
 * so this macro is defined to 8 by default. you can decrease value to save memory space.
 */

#define CONFIG_FRAME_LAYER_CAPACITY 8

/* atention: if you use Dynamic C compiler from rabbit semiconductor to compile 
 * this module, you should attention that the "size" is an keyword in dyanmic C.
 * so I had to replace size with "capacity". however, you should check your other
 * source codes to avoid possible side affect about this macro.
 */

/* #ifdef CONFIG_COMPILER_DYNAMICC
 * ##define size capacity
 * #endif
 */

#define FRAME_HOPESIZE(capacity) (sizeof(TiFrame)+(capacity))

/** 
 * TiFrame
 * TiMioBuffer contains multiple sections. each section can be regarded as a separate 
 * buffer. After initialization, it's default to section 0. In this case, it can simply
 * replace the TiIoBuf. All the input/output (R/W) operations are on current section.
 * 
 * frame layered structure
 * 
 *                          [App Frame .......................................]
 *                      [App Support Frame such as one2many ..................]
 *                  [Middleware such as TimeSync Frame, Localization Frame ...]
 *              [NET Frame ...................................................]
 *          [LLC Frame........................................................]
 *      [MAC Frame ...........................................................]
 *  [PHY Frame ...............................................................]
 * 
 * each higher layer item is also the payload of the lower item.
 * 
 * member variables
 *  memsize: the memory block size the current frame object occupied
 *  curlayer: current item
 *  layercount: active items currently.
 *  bottomlayer: outermost layer.
 *  origin: the initial start position in the buffer. skipinner() and skipouter()
 *      will adjust the index according to this origin.
 *  item_start[idx]: offset of each item. item size is estimated by item_start[current+1]
 *      - item_start[current]
 *  item_length[idx]: data length of the specified layer's interior data.
 */
typedef struct{
    uintx memsize;
    uintx firstlayer;
    uintx curlayer;
    uintx layercount;
    uintx layerstart[CONFIG_FRAME_LAYER_CAPACITY];
    uintx layerlength[CONFIG_FRAME_LAYER_CAPACITY];
    uintx layercapacity[CONFIG_FRAME_LAYER_CAPACITY];
}TiFrame;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * create a frame
 */
#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_create( uintx capacity );
#endif

/**
 * free a frame created by frame_create()
 */
#ifdef CONFIG_DYNA_MEMORY
void frame_free( TiFrame * frame );
#endif

TiFrame * frame_open( char * buf, uintx memsize, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity );
void frame_close( TiFrame * frame );

#ifdef CONFIG_DYNA_MEMORY
TiFrame * frame_duplicate( TiFrame * frame );
#endif

void frame_reset( TiFrame * frame, uintx init_layerindex, uintx init_layerstart, uintx init_layercapacity );

/**
 * clear the TiFrame object. after the clear, there's only one item inside the frame
 * and it occupies all the memory available.
 */
void frame_bufferclear( TiFrame * frame );
uintx frame_buffercapacity( TiFrame * frame );

#define frame_totalstart(frame) frame_layerstart(frame,(frame)->firstlayer)
#define frame_totalstartptr(frame) frame_layerstartptr(frame,(frame)->firstlayer)
#define frame_totalend(frame) frame_layerend(frame,(frame)->firstlayer)
#define frame_totallength(frame) frame_layerlength(frame,(frame)->firstlayer)
#define frame_settotallength(frame,count) frame_setlayerlength(frame,(frame)->firstlayer,count)
#define frame_totalcapacity(frame) frame_layercapacity(frame,(frame)->firstlayer)
#define frame_settotalcapacity(frame,count) frame_setlayerlength(frame,(frame)->firstlayer,count)

#define frame_totalcopyfrom(frame,from) {memmove((frame),(from),(from->memsize));}

uintx frame_layerstart( TiFrame * frame, uint8 layer );
char * frame_layerstartptr( TiFrame * frame, uint8 layer );
uintx frame_layerend( TiFrame * frame, uint8 layer );
uintx frame_layerlength( TiFrame * frame, uint8 layer );
void frame_setlayerlength( TiFrame * frame, uint8 layer, uintx count );
uintx frame_layercapacity( TiFrame * frame, uint8 layer );
void frame_setlayercapacity( TiFrame * frame, uint8 layer, uintx count );

/**
 * return the current layer index in the byte buffer. the outer most layer is with 
 * index 0. the TiFrame currently support at most 8 layers. this can be reconfigured
 * by macro CONFIG_FRAME_LAYER_CAPACITY.
 */
uintx frame_curlayer( TiFrame * frame );

/**
 * change the current item
 * if failed, then nothing changed. 
 */
bool frame_setcurlayer( TiFrame * frame, uintx cur );

/** 
 * set the inner item as the current one
 */
bool frame_moveinner( TiFrame * frame );

/** 
 * set the inner item as the current one
 */
bool frame_moveouter( TiFrame * frame );

#define frame_movefirst(f) frame_moveoutermost(f)
#define frame_movelast(f) frame_moveinnermost(f)

bool frame_moveinnermost( TiFrame * frame );
bool frame_moveoutermost( TiFrame * frame );

#define frame_firstlayer(f) frame_outermost(f)
#define frame_lastlayer(f) frame_innermost(f)

uintx frame_innermost( TiFrame * frame );
uintx frame_outermost( TiFrame * frame );

uintx frame_initlayer( TiFrame * frame, uint8 layerindex, uintx skiplen, uintx capacity );

/** 
 * add inner item and also change the current to it
 * 
 * @param skiplen: header size of the current item
 * @param left: tail size of the current item
 */
bool frame_skipinner( TiFrame * frame, uintx skiplen, uintx left );

bool frame_skipouter( TiFrame * frame, uintx skiplen, uintx left );

/**
 * @section all the following functions are operated on current layer.
 */

void frame_clear( TiFrame * frame );

/**
 * resize the current layer frame to specified new size. this function is usually
 * called after an frame object is constructed because the default frame will occupy
 * all the memmory available in the buffer.
 * 
 * @return the new size of the current layer frame is succeed. 0 if failed.
 */
uintx frame_resize( TiFrame * frame, uintx newsize );

uintx frame_start( TiFrame * frame );

uintx frame_end( TiFrame * frame );

uintx frame_length( TiFrame * frame );

uintx frame_capacity( TiFrame * frame );

/**
 * return an memory pointer to the first byte of the current item's data. if there's
 * no data in the current item, then return NULL. it's almost the same as frame_startptr()
 * except it will return NULL when the item is empty.
 */
char * frame_dataptr( TiFrame * frame );

char * frame_startptr( TiFrame * frame );
char * frame_endptr( TiFrame * frame );

char * frame_header( TiFrame * frame, uintx * psize );
char * frame_interior( TiFrame * frame, uintx * psize );
char * frame_tail( TiFrame * frame, uintx * psize );

bool frame_full( TiFrame * frame );
bool frame_empty( TiFrame * frame );
uintx frame_available( TiFrame * frame );
uintx frame_read( TiFrame * frame, char * buf, uintx size );
uintx frame_write( TiFrame * frame, char * data, uintx len );

/* frame_pushback
 * append the data at the end of the original data. If there's not enough space,
 * then only the (size-length) characters will be padded.
 */
uintx frame_pushback( TiFrame * frame, char * data, uintx len );

 /* frame_pushbyte
 * append a single byte at the end of the original data
 */
uintx frame_pushbyte( TiFrame * frame, unsigned char value );

uintx frame_front( TiFrame * frame, char * buf, uintx len );
void frame_popfront( TiFrame * frame, uintx count );

bool frame_set( TiFrame * frame, uintx idx, char c );
bool frame_get( TiFrame * frame, uintx idx, char * c );

uintx frame_copyfrom( TiFrame * frame1, TiFrame * frame2 );
uintx frame_copyto( TiFrame * frame1, TiFrame * frame2 );
uintx frame_movefrom( TiFrame * frame1, TiFrame * frame2 );

uintx frame_moveto( TiFrame * frame1, TiFrame * frame2 );
uintx frame_append( TiFrame * frame1, TiFrame * frame2 );

void frame_setlength( TiFrame * frame, uintx count );
void frame_adjustlength( TiFrame * frame, int delta );

#define frame_setcapacity(f,capacity) frame_setlayercapacity((f),(f)->curlayer,(capacity))

/* Dump the internal state of the TiFrame object. This function is used to help debugging.
 * Requires <link rtl_debugio.h>
 */
#ifdef CONFIG_DEBUG
void frame_dump( TiFrame * frame );
#endif


#ifdef __cplusplus
}
#endif

#endif /* _RTL_FRAME_H_5768_ */

