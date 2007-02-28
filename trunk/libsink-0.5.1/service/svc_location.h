
#ifndef _SVC_LOCATION_H_4656_
#define _SVC_LOCATION_H_4656_


#include "svc_foundation.h"

/* state & LOC_TYPE_ARCHOR == 1, this is a archor node
 * state & LOC_TYPE_ARCHOR == 0, this is a non-archor node
 */
#define LOC_TYPE_ARCHOR 0x80

#define LENGTH_UNIT 0.59 * 2      //0.59m

#define L_1_RSSI      244
#define L_2_RSSI      231
#define L_3_RSSI      227
#define L_4_RSSI      224
#define L_5_RSSI      222
#define L_6_RSSI      220
#define L_7_RSSI      219
#define L_8_RSSI      218
#define L_9_RSSI      215
#define L_10_RSSI     210
#define L_11_RSSI    1
#define L_12_RSSI    2
#define L_13_RSSI    3
#define L_14_RSSI    4
#define L_15_RSSI  5
#define L_16_RSSI   11
#define L_17_RSSI   6
#define L_18_RSSI   7
#define L_19_RSSI   8
#define L_20_RSSI    9
#define L_21_RSSI  10
#define L_22_RSSI   11 
#define L_23_RSSI    12 
#define L_24_RSSI    13 
#define L_25_RSSI  14
#define L_26_RSSI   15
#define L_27_RSSI   16
#define L_28_RSSI    17
#define L_29_RSSI    18
#define L_30_RSSI   19

#define L_1      1  * LENGTH_UNIT
#define L_2      2  * LENGTH_UNIT
#define L_3      3  * LENGTH_UNIT
#define L_4      4  * LENGTH_UNIT
#define L_5      5  * LENGTH_UNIT
#define L_6      6  * LENGTH_UNIT
#define L_7      7  * LENGTH_UNIT
#define L_8      8  * LENGTH_UNIT
#define L_9      9  * LENGTH_UNIT
#define L_10     10 * LENGTH_UNIT
#define L_11     11 * LENGTH_UNIT
#define L_12     12 * LENGTH_UNIT
#define L_13     13 * LENGTH_UNIT
#define L_14     14 * LENGTH_UNIT
#define L_15     15 * LENGTH_UNIT
#define L_16     16 * LENGTH_UNIT
#define L_17     17 * LENGTH_UNIT
#define L_18     18 * LENGTH_UNIT
#define L_19     19 * LENGTH_UNIT
#define L_20     20 * LENGTH_UNIT
#define L_21     21 * LENGTH_UNIT
#define L_22     22 * LENGTH_UNIT
#define L_23     23 * LENGTH_UNIT
#define L_24     24 * LENGTH_UNIT
#define L_25     25 * LENGTH_UNIT
#define L_26     26 * LENGTH_UNIT
#define L_27     27 * LENGTH_UNIT
#define L_28     28 * LENGTH_UNIT
#define L_29     29 * LENGTH_UNIT
#define L_30     30 * LENGTH_UNIT


typedef struct {
  int16 x;
  int16 y;
  int16 z;
} TLocation;

typedef struct {
  uint8 state;
  uint16 nodeid;
  //uint16 neighbors[8];
} TLocationService;

/* "loc_read()" and "loc_write()" are used to get and set the location of the
 * node itself.
 *
 * "loc_evolve()" is used to performe location estimation.
 * one call to "loc_evolve" only performs one time of estimation. you can call
 * it multi times to get a more occurate resullt.
 */
TLocationService * lcs_construct(char * buf, uint16 size);
void lcs_destroy( TLocationService * lcs );
void lcs_rssi2dist( TLocationService * lcs, uint8 rssi, uint16 * dis );
void lcs_init(TLocationService * lcs );
int8 lcs_read( TLocationService * lcs, uint16 nodeid, TLocation * loc, uint8 opt );
int8 lcs_write( TLocationService * lcs, uint16 nodeid, TLocation * loc, uint8 opt );
int8 lcs_evolve( TLocationService * lcs );

#endif

