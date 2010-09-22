#ifndef _APL_IEEE802FRAME154_DUMP_H_4873_
#define _APL_IEEE802FRAME154_DUMP_H_4873_

/* application layer's frame dump */

#include "../../common/openwsn/rtl/rtl_configall.h"
#include "../../common/openwsn/rtl/rtl_foundation.h"
#include "../../common/openwsn/rtl/rtl_frame.h"
#include "../../common/openwsn/rtl/rtl_debugio.h"

#undef CONFIG_ASCII_OUTPUT
void ieee802frame154_dump( TiFrame * f );

// todo: should be 
// void _output_frame( TiFrame * f );

#endif
