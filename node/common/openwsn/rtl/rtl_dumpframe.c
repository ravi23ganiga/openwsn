
#include "../hal/hal_configall.h"
#include <stdlib.h>
#include <string.h>
#include <avr/wdt.h>
#include "../hal/hal_foundation.h"
#include "../rtl/rtl_frame.h"
#include "../rtl/rtl_debugio.h"
#include "../rtl/rtl_ieee802frame154.h"
#include "../hal/hal_assert.h"
#include "../hal/hal_debugio.h"
#include "../svc/svc_simplealoha.h"
#include "rtl_dumpframe.h"

TiIEEE802Frame154Descriptor m_meta;

void ieee802frame154_dump( TiFrame * frame, TiUartAdapter * uart )
{
    static TiIEEE802Frame154Descriptor m_desc;
    TiIEEE802Frame154Descriptor * desc;

	if (frame_length(frame) > 0)
	{   
		dbo_putchar( '>' );
	 	dbo_n8toa( frame_length(frame) );

        desc = ieee802frame154_open( &m_desc );
        if (ieee802frame154_parse(desc, frame_startptr(frame), frame_length(frame)))
        {
            // if the frame received is parsed successfully, then output it to the
            // computer through debugging channel

            //ieee802frame154_set_sequence( desc, seqid ++ );
		    //ieee802frame154_set_panto( desc, CONFIG_ALOHA_DEFAULT_PANID );
		    //ieee802frame154_set_shortaddrto( desc, CONFIG_ALOHA_REMOTE_ADDRESS );
		    //ieee802frame154_set_panfrom( desc, CONFIG_ALOHA_PANID);
		    //ieee802frame154_set_shortaddrfrom( desc, CONFIG_ALOHA_LOCAL_ADDRESS );

            // todo: you can output more
            // reference frame_dump() in rtl_frame.c

            dbo_n8toa( ieee802frame154_sequence(desc) );
			dbo_putchar( ':' );
			dbo_write( frame_startptr(frame), frame_length(frame) );
		}
		else{
	        // if the frame received is parsed failed, then output the error frame
            // to the computer through debugging channel

	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			dbo_write( frame_startptr(frame), frame_length(frame) );
		}
		dbo_putchar( '\n' );
	}
}


