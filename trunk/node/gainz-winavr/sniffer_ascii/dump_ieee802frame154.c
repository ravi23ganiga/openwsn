#include "dump_ieee802frame154.h"

/* todo: you should replace dbo_xxx with sio_write. so that we can identify the frame 
 * in the byte stream in the PC side.
 */
#ifdef CONFIG_ASCII_OUTPUT
void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
	//uint8 i;

    // if the opf structure contains an frame, then output it.
	if (opf_datalen(opf) > 0)
	{   
		dbo_putchar( '>' );
	 	dbo_n8toa( opf->datalen );

		if (opf_parse(opf, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbo_n8toa( *opf->sequence );
			dbo_putchar( ':' );
			_dbo_write_n8toa( (char*)&(opf->buf[0]), opf->buf[0]+1 );
		}
		else{
	        dbo_putchar( 'X' );
			dbo_putchar( ':' );
			_dbo_write_n8toa( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( '\r' );
		dbo_putchar( '\n' );
	}
    else{
        // If the opf structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbo_putchar( '.' );
    }
}
#endif

#ifndef CONFIG_ASCII_OUTPUT
void _output_openframe( TiOpenFrame * opf, TiUartAdapter * uart )
{
    // if the opf structure contains an frame, then output it.
	if (opf_datalen(opf) > 0)
	{   
		dbo_putchar( 0x88 );
		dbo_putchar( 0x88 );
	 	dbo_putchar( opf->datalen );

		if (opf_parse(opf, 0))
		{
            // if the frame parsing succeed, then output the whole frame.
	        dbo_putchar( *opf->sequence );
			dbo_putchar( 0x88 );
			dbo_putchar( 0x88 );
			dbo_write( (char*)&(opf->buf[0]), opf->buf[0]+1 );
		}
		else{
            dbo_putchar( 0x00 );
			dbo_putchar( 0x88 );
			dbo_putchar( 0x88 );
			dbo_write( (char*)&(opf->buf[0]), opf->datalen );
		}
		dbo_putchar( 0x99 );
		dbo_putchar( 0x99 );
	}
    else{
        // If the opf structure doesn't contain frames, then output a '.' to indicate 
        // the call of this function. However, this case rarely happens.
        dbo_putchar( 0x88 );
        dbo_putchar( 0x88 );
		dbo_putchar( 0x99 );
		dbo_putchar( 0x99 );
    }
}
#endif
