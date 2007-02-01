#ifndef _HAL_INTERRUPT_H_4236_
#define _HAL_INTERRUPT_H_4236_

/*******************************************************************************
 * @author zhangwei on 2006-09-12
 * Interrupt Management
 * 
 ******************************************************************************/

void interrupt_init( void );
void interrupt_enable( uint8 id );
void interrupt_disable( uint8 id );
void interrupt_enable_global( void );
void interrupt_disable_global( void );

#endif