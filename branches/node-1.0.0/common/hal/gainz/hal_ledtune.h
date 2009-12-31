
/* TiLedTune
 * This service emulate the light tuning process. An hardware timer will drive this
 * service to run. The LED will flash according to a specified frequency. The frequency 
 * will change according to the inputed "luminance" parameter.
 */

typedef struct{
	uint16 luminance;
	TiTimerAdapter timer;
}TiLedTune;

TiLedTune * ledtune_construct( void * mem, uint16 memsize );
void ledtune_destroy( TiLedTune * ledtune );
TiLedTune * ledtune_open( TiLedTune * ledtune, uint16 freq );
void ledtune_close( TiLedTune * ledtune );
void ledtune_write( TiLedTune * ledtune, uint16 lum );