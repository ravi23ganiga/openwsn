#include "apl_ieee802frame154_dump.h"
#include "../../common/openwsn/rtl/rtl_ieee802frame154.h"

/* attention: You can replace dbc_xxx with sio_write. so that we can identify the frame 
 * in the byte stream in the PC side.
 */
#ifdef CONFIG_ASCII_OUTPUT
void ieee802frame154_dump( TiFrame * f )
{
    int8 len;
    TiIEEE802Frame154Descriptor meta;

    len = frame_length(f);
	if (len > 0)
	{   
		dbc_putchar( '>' );
	 	dbc_n8toa( len );
		 ieee802frame154_open( &meta );
        if (ieee802frame154_parse(&meta, frame_startptr(f), frame_length(f)))
        {
			dbc_putchar( ':' );
			dbc_write_n8toa( frame_startptr(f), len );
			dbc_putchar( 0xFC );
			}
		else{
            // if parsing failed, then we also output the data
            dbc_putchar( 0xFC );
			dbc_write_n8toa( frame_startptr(f), len );
			}
		}
		 else{
        // If the f doesn't contain any data, then still output some flag for indication.
        dbc_putchar( 0xFB );
        dbc_putchar( 0xFB );
    }
	//uint8 i;
/*
    // if the f structure contains an frame, then output it.
	if (opf_datalen(f) > 0)
	{   
		dbc_putchar( '>' );
	 	dbc_n8toa( f->datalen );

		if (opf_parse(f, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbc_n8toa( *f->sequence );
			dbc_putchar( ':' );
			dbc_write_n8toa( (char*)&(f->buf[0]), f->buf[0]+1 );
		}
		else{
	        dbc_putchar( 'X' );
			dbc_putchar( ':' );
			dbc_write_n8toa( (char*)&(f->buf[0]), f->datalen );
		}
		dbc_putchar( '\r' );
		dbc_putchar( '\n' );
	}
    else{
        // If the f structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbc_putchar( '.' );
    }
    */
}
#endif

#ifndef CONFIG_ASCII_OUTPUT
void ieee802frame154_dump( TiFrame * f )
{
    int8 len;
    TiIEEE802Frame154Descriptor meta;

    len = frame_length(f);
	if (len > 0)
	{   
		dbc_putchar( 0xFA );
		dbc_putchar( 0xFA );
	    dbc_putchar( frame_curlayer(f) );
	 	dbc_putchar( len );

        // if the frame received is parsed successfully, then output it to the
        // computer through debugging channel
        //
        ieee802frame154_open( &meta );
        if (ieee802frame154_parse(&meta, frame_startptr(f), frame_length(f)))
        {
            // todo: you can output more
            // reference frame_dump() in rtl_frame.c

			dbc_putchar( 0xFB );
			dbc_write( frame_startptr(f), len );
			dbc_putchar( 0xFC );
		}
		else{
            // if parsing failed, then we also output the data
            dbc_putchar( 0xFC );
			dbc_write( frame_startptr(f), len );
		}
	}
    else{
        // If the f doesn't contain any data, then still output some flag for indication.
        dbc_putchar( 0xFB );
        dbc_putchar( 0xFB );
    }

    /*
    // if the f structure contains an frame, then output it.
	if (opf_datalen(f) > 0)
	{   
		dbc_putchar( 0x88 );
		dbc_putchar( 0x88 );
	 	dbc_putchar( f->datalen );

		if (opf_parse(f, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbc_putchar( *f->sequence );
			dbc_putchar( 0x88 );
			dbc_putchar( 0x88 );
			dbc_write( (char*)&(f->buf[0]), f->buf[0]+1 );
		}
		else{
            dbc_putchar( 0x00 );
			dbc_putchar( 0x88 );
			dbc_putchar( 0x88 );
			dbc_write( (char*)&(f->buf[0]), f->datalen );
		}
		dbc_putchar( 0x99 );
		dbc_putchar( 0x99 );
	}
    else{
        // If the f structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbc_putchar( 0x88 );
        dbc_putchar( 0x88 );
		dbc_putchar( 0x99 );
		dbc_putchar( 0x99 );
    }
    */
}
#endif
