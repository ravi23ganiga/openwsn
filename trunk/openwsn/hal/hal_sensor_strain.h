/*****************************************************************************
 * @note
 * this module is based on hal_mcp6s26 and hal_ad
 * it provide a high level support to strain sensor
 ****************************************************************************/
 
#include "hal_mcp6s.h"

typedef struct{
	TMcp6s26 * mcp;
}TStrainSensor;

TStrainSensor * strain_construct( TStrainSensor * strain );
void strain_destroy( TStrainSensor * strain );  
uint8 strain_read( TStrainSensor * strain, char * buf, uint8 size, uint8 opt );

