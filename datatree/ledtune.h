#ifndef _LEDTUNE_H_4698_
#define _LEDTUNE_H_4698_

/* TiLedTune
 * This service emulate the light tuning process. An hardware timer will drive this
 * service to run. The LED will flash according to a specified frequency. The frequency 
 * will change according to the inputed "luminance" parameter.
 */

#include "../common/svc/svc_foundation.h"
#include "../common/svc/svc_timer.h"

typedef struct{
	uint16 luminance;
	TiTimer * timer;
}TiLedTune;

TiLedTune * ledtune_construct( void * mem, uint16 memsize, TiTimer * timer );
void ledtune_destroy( TiLedTune * ledtune );
TiLedTune * ledtune_open( TiLedTune * ledtune );
void ledtune_close( TiLedTune * ledtune );
void ledtune_write( TiLedTune * ledtune, uint16 lum );

#endif
