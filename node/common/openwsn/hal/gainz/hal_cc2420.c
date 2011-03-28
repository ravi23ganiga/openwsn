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

/******************************************************************************
 * hal_cc2420.c
 * This module implements a cc2420 adapter service. It's an simple wrapper of the
 * cc2420 low power wireless transceiver from Chipcon/TI. It helps to implement 
 * the MAC network layer. 
 *
 * You can regard the adapter as the PHY layer in layered communication architecture.
 * However. cc2420 provides more functions than a simple PHY.
 * 
 * Reference
 * - cc2420's startup and initialization, http://blog.sina.com.cn/s/blog_5e194f5e0100heex.html;
 * 
 * @state
 *	compile passed. tested. released.
 * 
 * @history
 * @author zhangwei on 200609
 * @modified by zhangwei on 20090718
 *	- revision. 
 * @modified by yan-shixing(TongJi University) in 200907
 *	- tested
 * @modified by zhangwei on 20090808
 *	- bugfix: cc2420_setrxmode(cc) in cc2420_open()
 *  - add automatic mode transition in cc2420_recv()/cc2420_write()
 *  - add cc2420_sfd_handler(). not tested.
 * @modified by zhangwei(TongJi University) on 20090919
 *	- add cc2420_rssi() and cc2420_lqi(). not tested.
 * @modified by zhangwei, yanshixing 
 *  - improved cc2420_recv add crc校验
 * @modified by Xu-Fuzhen in TongJi University(xufz0726@126.com) in 2010.10
 *  - improved interface and cc2420_ischannelclear()
 *
 *****************************************************************************/

#include "../hal_configall.h"
#include <string.h>
#include <stdio.h>
#include "../hal_foundation.h"
#include "../hal_cpu.h"
#include "../hal_interrupt.h"
#include "../hal_assert.h"
#include "../hal_targetboard.h"
#include "../hal_led.h"
#include "../hal_uart.h"
#include "../hal_cc2420const.h"
#include "../hal_cc2420.h"
#include "../hal_debugio.h"
#include "../hal_cc2420inc.h"

/* @attention
 * If you want to disable all the assertions in this macro, you should undef CONFIG_DEBUG.
 * You should do this in release version */

#undef  CONFIG_DEBUG
#define CONFIG_DEBUG


/* In "hal_cc2420inc.h", we implement the most fundamental cc2420 operating functions.
 * If you want to port hal_cc2420 to other platforms, you can simply revise the 
 * hal_cc2420inc.h. The other part inside hal_cc2420.h can keep unchanged.
 */
#include "../hal_cc2420inc.h"


// Current Parameter Arrray Positions
enum{
 CP_MAIN = 0,
 CP_MDMCTRL0,
 CP_MDMCTRL1,
 CP_RSSI,
 CP_SYNCWORD,
 CP_TXCTRL,
 CP_RXCTRL0,
 CP_RXCTRL1,
 CP_FSCTRL,
 CP_SECCTRL0,
 CP_SECCTRL1,
 CP_BATTMON,
 CP_IOCFG0,
 CP_IOCFG1
} ;

static inline int8 _cc2420_pin_init( TiCc2420Adapter * cc );
static inline int8 _cc2420_reg_init( TiCc2420Adapter * cc );
static inline bool _cc2420_setreg( TiCc2420Adapter * cc );
void _cc2420_waitfor_crystal_oscillator( TiCc2420Adapter * cc );
static uint8       _cc2420_writetxfifo( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 option );
static uint8       _cc2420_readrxfifo( TiCc2420Adapter * cc, char * buf, uint8 size, uint8 option );
static void        _cc2420_fifop_handler( void * ccptr, TiEvent * e );

#ifdef CONFIG_CC2420_SFD
static void        _cc2420_sfd_handler( void * ccptr, TiEvent * e );
#endif

/******************************************************************************
 * cc2420 service management 
 * construct(), destroy(), open() and close()
 *****************************************************************************/

TiCc2420Adapter * cc2420_construct( void * mem, uint16 size )
{
	hal_assert( sizeof(TiCc2420Adapter) <= size );
	memset( mem, 0x00, size );
	return (TiCc2420Adapter *)mem;
}

void cc2420_destroy( TiCc2420Adapter * cc )
{
	cc2420_close( cc );
}

TiCc2420Adapter * cc2420_open( TiCc2420Adapter * cc, uint8 id, TiFunEventHandler listener, 
	void * lisowner, uint8 option )
{
    cc->id = 0;
    cc->state = CC2420_STATE_RECVING;
    cc->listener = listener;
    cc->lisowner = lisowner;
    cc->option = option;
	cc->rssi = 0;
	cc->lqi = 0;
	cc->spistatus = 0;
	cc->rxlen = 0;

	_cc2420_pin_init( cc );
	_cc2420_spi_open();

	// enable the cc2420's internal voltage regulator through the VREG_EN pin of cc2420.
	// assert( the voltage regulation work correctly ). 
	cc2420_vrefon( cc );	

	// reset the chip by giving a pulse and level signal.
	HAL_CLR_CC_RSTN_PIN();
	// the old delay value is 10, seems also ok
	hal_delayus(100);  

	HAL_SET_CC_RSTN_PIN();
	// old value is 10, seems also ok
	hal_delayus(100);
	
	// start the oscillator of cc2420. 
	// assert( the oscillator must be started successfully );
	#ifdef CONFIG_DEBUG
	if (cc2420_oscon(cc))
	{
	
		led_on(LED_GREEN); hal_delay(100);
		led_off(LED_GREEN); hal_delay(1000);
	}
	else{

		led_on(LED_YELLOW); hal_delay(100);
		led_off(LED_YELLOW); hal_delay(1000);
	}
	#endif

	#ifndef CONFIG_DEBUG
	cc2420_oscon( cc );
	#endif

	// default settings attention:
	// - MDMCTRL0.AUTOCRC is always enabled for both the sender and receiver. 
	// - MDMCTRL0.AUTOACK is by default disabled. But you can call cc2420_enable_autoack()
	//	 to enable it.
	// - security is by default disabled.
	//
	_cc2420_reg_init( cc );
	
	// the default setting is to disable address recognition and set threhold of FIFOP
	#ifdef CONFIG_DEBUG
	if (_cc2420_setreg( cc ))
	{
		led_on(LED_GREEN); hal_delay(100);
		led_off(LED_GREEN); hal_delay(100);
	}
	else
	{
		led_on(LED_YELLOW); hal_delay(1000);
		led_off(LED_YELLOW); hal_delay(1000);
	} 
	#else
	_cc2420_setreg( cc );
	#endif

	cc2420_setchannel( cc, CC2420_DEF_CHANNEL );            // default communication channel
	cc2420_enable_addrdecode( cc );                         // enable hardware address recognition
	cc2420_setpanid( cc, CC2420_DEF_PANID );                // network id
	cc2420_setshortaddress( cc, CC2420_DEF_LOCAL_ADDRESS ); // node id inside the network
	cc2420_disable_autoack( cc );                           // auto ack is disabled by default

	cc2420_setrxmode( cc );  
	                               // switch to receiving mode
	_cc2420_waitfor_crystal_oscillator( cc );

	/* Here only enable the FIFOP interrupt request, but you must enable the global
	 * interrupt flag somewhere or else the cc2420 adapter still cannot response 
	 * to the FIFOP interrupt.
	 */
    hal_attachhandler( INTNUM_CC2420_FIFOP, _cc2420_fifop_handler, cc );
	#ifdef CONFIG_CC2420_SFD
	/* attention
	 * - Though you attach the SFD interrupt handler here, the interrupt itself 
	 *   hasn't been enabled yet! You should also enable the global interrupt flag 
	 *   in order to make the system really response to the SFD interrupt request.
	 * - SFD interrupt is controlled by the svc_timesync module.
	 * - The default settings for the SFD interrupt is disabled currently.
	 */
    hal_attachhandler( INTNUM_CC2420_SFD, _cc2420_sfd_handler, cc );
	#endif

    cc2420_enable_fifop( cc );
	#ifdef CONFIG_CC2420_SFD
	//cc2420_enable_sfd(cc);
	#endif
    return cc;
} 

void cc2420_close( TiCc2420Adapter * cc )
{
	cc2420_disable_fifop( cc );
    hal_detachhandler( INTNUM_CC2420_FIFOP );
}

uint8 cc2420_state( TiCc2420Adapter * cc )
{
	return cc->state;
}


/* _cc2420_pin_init()
 * This function is used in this file only. It initialize MCU's PIN functions 
 * and directions. If you want to port this adapter to other MCU, you should modify 
 * this function.
 * 
 * Actually, the following pin initialization has nothing to do with cc2420 operations.
 */
inline int8 _cc2420_pin_init( TiCc2420Adapter * cc )
{
	HAL_MAKE_MISO_INPUT();
	HAL_MAKE_MOSI_OUTPUT();
	HAL_MAKE_SPI_SCK_OUTPUT();
	HAL_MAKE_CC_RSTN_OUTPUT();
	HAL_MAKE_CC_VREN_OUTPUT();
	HAL_MAKE_CC_CS_OUTPUT();
	HAL_MAKE_CC_FIFOP_INPUT();
	HAL_MAKE_CC_CCA_INPUT();
	HAL_MAKE_CC_SFD_INPUT();
	HAL_MAKE_CC_FIFO_INPUT();

	// @todo: suggest giving initial values to these pins and wait for a little time
	// to guarantee the configuration is correct. 
  
    return 0;
}


inline static int8 _cc2420_reg_init( TiCc2420Adapter * cc )
{
	cc->param[CP_MAIN] = 0xf800;

	// disable auto adr_decode, cca mode is 3, auto crc is set
	cc->param[CP_MDMCTRL0] = ((0 << CC2420_MDMCTRL0_PANCRD) | (0 << CC2420_MDMCTRL0_ADRDECODE) | 
		(2 << CC2420_MDMCTRL0_CCAHIST) | (3 << CC2420_MDMCTRL0_CCAMODE)  | 
		(1 << CC2420_MDMCTRL0_AUTOCRC) | (2 << CC2420_MDMCTRL0_PREAMBL)); 

	cc->param[CP_MDMCTRL1] = 20 << CC2420_MDMCTRL1_CORRTHRESH; // default
	cc->param[CP_RSSI] = 0xE080; //default 
	cc->param[CP_SYNCWORD] = 0xA70F;

	// CC2420_TXCTRL_BUFCUR:  default is 2-1.16ma,now is 980ua
	// default, turnaround time is 12 symbols
	cc->param[CP_TXCTRL] = ((1 << CC2420_TXCTRL_BUFCUR) | (1 << CC2420_TXCTRL_TURNARND) | 
		(3 << CC2420_TXCTRL_PACUR) | (1 << CC2420_TXCTRL_PADIFF) | (0x1f << CC2420_TXCTRL_PAPWR)); 

	// default is 980ua
	cc->param[CP_RXCTRL0] = ((1 << CC2420_RXCTRL0_BUFCUR) | (2 << CC2420_RXCTRL0_MLNAG) | 
		(3 << CC2420_RXCTRL0_LOLNAG) | (2 << CC2420_RXCTRL0_HICUR) | (1 << CC2420_RXCTRL0_MCUR) | 
		(1 << CC2420_RXCTRL0_LOCUR));

	cc->param[CP_RXCTRL1]  = ((1 << CC2420_RXCTRL1_LOLOGAIN) | (1 << CC2420_RXCTRL1_HIHGM) |
		(1 << CC2420_RXCTRL1_LNACAP) | (1 << CC2420_RXCTRL1_RMIXT) |
		(1 << CC2420_RXCTRL1_RMIXV) | (2 << CC2420_RXCTRL1_RMIXCUR));
							    
	// default to 2405 MHz
	cc->param[CP_FSCTRL] = ((1 << CC2420_FSCTRL_LOCK) | ((357+5*(CC2420_DEF_CHANNEL-11)) << CC2420_FSCTRL_FREQ));

	// disable the security
	cc->param[CP_SECCTRL0] = ((0 << CC2420_SECCTRL0_PROTECT) | (1 << CC2420_SECCTRL0_CBCHEAD) |
		(1 << CC2420_SECCTRL0_SAKEYSEL)  | (1 << CC2420_SECCTRL0_TXKEYSEL) | (1 << CC2420_SECCTRL0_SECM ));

	cc->param[CP_SECCTRL1] = 0;
	cc->param[CP_BATTMON]  = 0;		//battery monitor is disable 

	//cc->param[CP_IOCFG0]   = (((TOSH_DATA_LENGTH + 2) << cc2420_IOCFG0_FIFOTHR) | (1 <<cc2420_IOCFG0_FIFOPPOL)) ;
	//set fifop threshold to greater than size of frame, fifop goes active at end of frame
 
	// todo: modified 20090723
	// set high rise interrupt
	// FIFOP threshold is 127

	// 为什么要用127移位？ 不要用这种写法（颜：其实127就是设置的值，因为CC2420_IOCFG0_FIFOTHR为0，它大概是为了统一格式，所以这么写）
	//cc->param[CP_IOCFG0]   = (((127) << CC2420_IOCFG0_FIFOTHR) | (1 <<CC2420_IOCFG0_FIFOPPOL)) ; //polarity is inverted as compareed to the specification
	cc->param[CP_IOCFG0]   = (((127) << CC2420_IOCFG0_FIFOTHR) | (0 <<CC2420_IOCFG0_FIFOPPOL)) ; //polarity is inverted as compareed to the specification
	cc->param[CP_IOCFG1]   =  0;

	return 0;
}

/* cc2420 set register vlaues. used to set the register values of cc2420 */
inline bool _cc2420_setreg( TiCc2420Adapter * cc )
{
	uint16 data;
  
	/*cc2420_writeregister( cc, CC2420_MAIN, cc->param[CP_MAIN] );
	cc2420_writeregister( cc, CC2420_MDMCTRL0, cc->param[CP_MDMCTRL0] );
	data = cc2420_readregister( cc, CC2420_MDMCTRL0 );

	if (data != cc->param[CP_MDMCTRL0]) 
	{
		return FALSE;
    }
	
	cc2420_writeregister( cc, CC2420_MDMCTRL1, cc->param[CP_MDMCTRL1] );
	cc2420_writeregister( cc, CC2420_RSSI, cc->param[CP_RSSI] );
	cc2420_writeregister( cc, CC2420_SYNCWORD, cc->param[CP_SYNCWORD] );
	cc2420_writeregister( cc, CC2420_TXCTRL, cc->param[CP_TXCTRL] );
	cc2420_writeregister( cc, CC2420_RXCTRL0, cc->param[CP_RXCTRL0] );
	cc2420_writeregister( cc, CC2420_RXCTRL1, cc->param[CP_RXCTRL1] );
	cc2420_writeregister( cc, CC2420_FSCTRL, cc->param[CP_FSCTRL] );

	cc2420_writeregister( cc, CC2420_SECCTRL0, cc->param[CP_SECCTRL0] );
	cc2420_writeregister( cc, CC2420_SECCTRL1, cc->param[CP_SECCTRL1] );
	cc2420_writeregister( cc, CC2420_IOCFG0, cc->param[CP_IOCFG0] );
	cc2420_writeregister( cc, CC2420_IOCFG1, cc->param[CP_IOCFG1] );

	cc2420_sendcmd( cc, CC2420_SFLUSHTX );
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );
	*/
	// todo:
	// 上面的代码是老版本，下面的代码是新版本，请将新老版本merge到一起去
	// new version started from here
	// 20090724


	cc2420_writeregister( cc, CC2420_MAIN, cc->param[CP_MAIN] );
	data = cc2420_readregister( cc, CC2420_MAIN);
	if (data != cc->param[CP_MAIN]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_MDMCTRL0, cc->param[CP_MDMCTRL0]);
	data = cc2420_readregister( cc, CC2420_MDMCTRL0);
	if (data != cc->param[CP_MDMCTRL0]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_MDMCTRL1, cc->param[CP_MDMCTRL1]);
	data = cc2420_readregister( cc, CC2420_MDMCTRL1);
	if (data != cc->param[CP_MDMCTRL1]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_RSSI, cc->param[CP_RSSI] );
	data = cc2420_readregister( cc, CC2420_RSSI);
	if (data != cc->param[CP_RSSI]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_SYNCWORD, cc->param[CP_SYNCWORD] );
	data = cc2420_readregister( cc, CC2420_SYNCWORD);
	if (data != cc->param[CP_SYNCWORD]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_TXCTRL, cc->param[CP_TXCTRL] );
	data = cc2420_readregister( cc, CC2420_TXCTRL);
	if (data != cc->param[CP_TXCTRL]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_RXCTRL0, cc->param[CP_RXCTRL0] );
	data = cc2420_readregister( cc, CC2420_RXCTRL0);
	if (data != cc->param[CP_RXCTRL0]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_RXCTRL1, cc->param[CP_RXCTRL1] );
	data = cc2420_readregister( cc, CC2420_RXCTRL1);
	if (data != cc->param[CP_RXCTRL1]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_FSCTRL, cc->param[CP_FSCTRL] );
	data = cc2420_readregister( cc, CC2420_FSCTRL);
	if (data != cc->param[CP_FSCTRL]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_SECCTRL0, cc->param[CP_SECCTRL0] );
	data = cc2420_readregister( cc, CC2420_SECCTRL0);
	if (data != cc->param[CP_SECCTRL0]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_SECCTRL1, cc->param[CP_SECCTRL1] );
	data = cc2420_readregister( cc, CC2420_SECCTRL1);
	if (data != cc->param[CP_SECCTRL1]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_IOCFG0, cc->param[CP_IOCFG0]);
	data = cc2420_readregister( cc, CC2420_IOCFG0);
	if (data != cc->param[CP_IOCFG0]) 
	{
		return false;
    }

	cc2420_writeregister( cc, CC2420_IOCFG1, cc->param[CP_IOCFG1]);
	data = cc2420_readregister( cc, CC2420_IOCFG1);
	if (data != cc->param[CP_IOCFG1]) 
	{
		return false;
    }

	cc2420_sendcmd( cc, CC2420_SFLUSHTX );
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );

	return true;
}

/*void cc2420_restart( TiCc2420Adapter * cc )
{
	uint8 status;

	status = cc2420_sendcmd( cc, CC2420_SXOSCON );
	while (!(status & 0x40))
	{
		//CC2420_StrobCmd(CC2420_SXOSCOFF);
		status = CC2420_StrobCmd( CC2420_SXOSCON );
	}

	status = cc2420_sendcmd( cc, CC2420_SRXON );
	CC_CLR_CS_PIN();
	_cc2420_spi_put( CC2420_SFLUSHRX );
	_cc2420_spi_put( CC2420_SFLUSHRX );
	CC_SET_CS_PIN();
}*/


/******************************************************************************
 * cc2420 service input/output 
 * _cc2420_writetxfifo(), _cc2420_readrxfifo(), cc2420_recv(), cc2420_send()
 *****************************************************************************/

/* write a frame to cc2420's TXFIFO buffer for sending.
 * 
 * @param
 *	cc				          TiCc2420Adapter service
 *  buf                       the frame buffer inside the memory
 *  len                       length of the frame data inside "buf". The first byte is the mac 
 *                            frame length. it will be overwritten by the value "len-1".
 *  option			          if option & 0x01 == 1, then this frame request ACK.
 * 
 * @return
 *  >0                        byte count actually written to TXFIFO.
 *  0                         when failed
 * 
 * attention: 
 *  - TXFIFO underflow is issued if too few bytes are written to the TXFIFO. transmission
 * is then automatically stopped. the underflow is indicated by the TX_UNDERFLOW status
 * bit. (bit 5)
 * 
 *	- bit5(TX_UNDERFLOW) indicates whether an FIFO underflow has occured during 
 * transmission. attention this must be cleared manually with a SFLUSHTX command strobe.
 */ 
uint8 _cc2420_writetxfifo( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 option )
{
	uint8 i=0;

	// @attention
	// The minimal input buf is [Length 1B] [Frame Control 2B] [AddressTo 2B] [AddressFrom 0B] [Potential Checksum 2B]
	// We don't support sending frames longer than 128 bytes(including 128 bytes) or 
	// less than 5 bytes. 
	// 
	if ((len < CC2420_MIN_FRAME_LENGTH) || (len >= CC2420_TXFIFO_SIZE))
		return 0;
	
	// Check whether the SFD pin is low. High SFD indicates the last frame is still 
	// in sending state. It's safe to wait for the SFD change to low. 
	//while (HAL_READ_CC_SFD_PIN() == 1) {};//最后再做

	cpu_atomic_t cpu_status = _cpu_atomic_begin();
	{
		HAL_CLR_CC_CS_PIN();
	    
		// attention
		// - the crystal oscillator must be running for writing the TXFIFO(0x3E) 
		// - reading TXFIFO is only possible using RAM read (according to cc2420 datasheet)

		cc->spistatus = _cc2420_spi_put( CC2420_TXFIFO );

		_cc2420_spi_put( len-1 );

		for (i=1; i<len; i++) 
		{
			_cc2420_spi_put( buf[i] );
		}
		
		HAL_SET_CC_CS_PIN();
	}
	_cpu_atomic_end(cpu_status);

	// before sending the STXON, it is necessary to check the CCA pin to see whether 
	// the channel is clear. If it is clear, the CCA pin might be 1.  referring to the graph 
	// in the checkpoint3.pdf, page 7 and the cc2420 datasheet, page 63, the CCA_MODE.

	while (!HAL_READ_CC_CCA_PIN()) {};
	// while(!(PIND & (1<<6))) {};

	cc2420_sendcmd( cc, CC2420_STXON );
	while (!(HAL_READ_CC_SFD_PIN() == 1)) {};
	while (HAL_READ_CC_SFD_PIN() == 1) {};
	//hal_delay(60);
	// @attention
	// If you change cc2420 to receiving mode immediately, you may encounter frame 
	// loss because the last sending may not complete in time.
	// Suggest to wait some time in the transition between sending and receiving. 

	return len;
}

/* cc2420_readrxfifo
 * reads the frame from cc2420 transceiver and place it into the "buf"
 * this function can be called in both interrupt mode or non-interrupt mode. 
 *
 * @param
 *	cc			TiCc2420Adapter service
 *  buf         the memory buffer to hold the frame
 *  size        size of the buffer. for cc2420, it should always larger than the 
 *              maximum frame length + 1. Currently, the default settins is 128
 *              (defined by macro CONFIG_CC2420_RXBUF_SIZE)
 *
 * @return
 *	0			no frame received inside the buffer
 *	>0          frame received. the return value indicates how many bytes in the buf. 
 *              the first byte in the buf is the MAC frame length read out from 
 *              cc2420 transceiver. so the returned value is always equal to buf[0]+1.
 */
uint8 _cc2420_readrxfifo( TiCc2420Adapter * cc, char * buf, uint8 size, uint8 option )
{
	cpu_atomic_t cpu_status;
	uint8 i, len, count=0;

	// if the oscillator isn't stable, then restart the transceiver chip
/*
	status = cc2420_sendcmd( CC2420_SNOP );
	if (status < 0x40)
	{
		cc2420_restart( cc );
		return 0;
	}
*/

	/* todo since the readrxfifo is called in both non-interrupt mode and interrupt
	 * node, I suggest to develope two version of readrxfifo to satisfy different
	 * contexts.
	 *
	 * modified by zhangwei on 200912
	 *	- Bug fix. Since the execution of this function maybe intervened by the interrupts, 
	 * so we should place _cpu_atomic_begin() at the very begging of the implementation.
	 */
	cpu_status = _cpu_atomic_begin();
	_cc2420_waitfor_crystal_oscillator( cc );

	uint8 status;

	do{	   
		status = cc2420_sendcmd( cc, CC2420_SNOP );
	}while (!(status & (1 << CC2420_XOSC16M_STABLE)));

	{
		HAL_CLR_CC_CS_PIN();
		// attention: the crystal oscillator must be running for access the RXFIFO(0x3F) 
		//
		// SPDR bit7 (SPIF): SPI interrupt flag. this bit will be set when a serial 
		// transfer complete. it's automatically clearly by ISR or by first reading 
		// the SPI status register with SPIF set, then accessing the SPI data register.
		// reference: SPSR register in atmega128L datasheet. 

		cc->spistatus = _cc2420_spi_put( CC2420_RXFIFO | 0x40 );	// read RXFIFO register					

		/* Read the length byte from cc2420 transceiver. Attention the value of the 
		 * length byte doesn't include itself. */
		len = _cc2420_spi_get();						
	                             
		/* modified by zhangwei on 200909
		 *	- Bug fix. You cannot enable the following assertion in real applications. 
		 * If two frames encounter collision during their transmission, the len byte 
		 * read out may longer than size. This is the feature of wireless communication
		 * and not our own fault. 
		 * 
		 * hal_assert( len < size );
		 */

		/* @attention
		 * Though cc2420 datasheet tells us this transceiver supports frames longer
		 * than 128 bytes(including the length byte itself), the TiCc2420Adapter doesn't
		 * plan to support this feature. 
		 * 
		 * The frames which are very short and very long(larger than 128) are all
		 * discarded by TiCc2420Adapter. 
		 */
		if (len < CC2420_MIN_FRAME_LENGTH)
		{
			//for (i=0; i<len; i++);
			//{
			//	_cc2420_spi_get();
			//}
			cc2420_sendcmd( cc, 0x08 );
			hal_delayus(5);
			cc2420_sendcmd( cc, 0x08 );
			hal_delayus(5);
			count=0;
		}
		else if (len > 127)
		{
			// if the incomming frame length is too long (RXFIFO buffer overflow occured), 
			// then this function will simply drop it.
			cc2420_sendcmd( cc, 0x08 );
			cc2420_sendcmd( cc, 0x08 );
			count=0;

			// todo
			// However, the cc2420 transceiver support frames longer than 127. So
			// you should consider the logic in this case.
		}
		else{
			// len &= 0x7F;

			// @attention
			// Actually, you needn't do with the overlength frame here. because the cc2420 transceiver's
			// internal frame buffer is only 128 bytes, the longer frame will trigger RXFIFO
			// overflow. this has already been processed inside the interrupt handler. furthermore, 
			// you must manually send RXFIFO FLUSH command to 2420 to restart it if overflow
			// really occurs.

			hal_assert( len+1 <= size );

			if (len < size)
			{
				buf[0] = len;
				for (i=1; i<=len; i++)
				{
					buf[i] = _cc2420_spi_get();
				}
				count = len+1;
			}
			else{
				cc2420_sendcmd( cc, 0x08 );
				cc2420_sendcmd( cc, 0x08 );
				count = 0;
			}

/*			count = min( len, size );
			buf[0] = len;
			for (i=1; i<=count; i++)
			{

				buf[i] = _cc2420_spi_get();
			}

			if (len >= size)
			{
			cc2420_sendcmd( cc, 0x08 );
			cc2420_sendcmd( cc, 0x08 );
			}

			// todo 
			// If the input buffer cannot hold the entire frame, we still need to read all 
			// the left data in cc2420's RXFIFO. Or else these data will lead to unexpected
			// RXFIFO overflow. --> However, this should never happens because the master 
			// program usually allocate a buffer large enough to hold any frames. 
			// 
			for (i=count; i<=len; i++);
			{
				SPDR = 0;
				while (!(SPSR & 0x80)) {};
				SPDR; 
			}*/

			/* According to cc2420's datasheet, the last two bytes in the frame are 
			 * RSSI(Received Signal Strength Indicator) and LQI(link Quality Indicator).
			 * The value of "len" already includes them. You needn't do anything special 
			 * to read them out. they've already been read out and put into the "buf".
			 */
		}
		HAL_SET_CC_CS_PIN();
    }
    _cpu_atomic_end(cpu_status); 
	return count;
}

/* cc2420_send()
 * send a frame at once through cc2420 transceiver.
 *
 * @param
 *	len                       is the length of the data in the "buf". it's equal to 
 *                            "frame length + 1".
 *  option                    if bit 0 of option is 1, then this function will request 
 *                            ACK from the receiver.
 *
 * @return
 *	0			              failed. you should retry again.
 *  >0                        byte count successfully send. It should equal to parameter 
 *                            "len" when success.
 */
uint8 cc2420_send( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 option )
{
	uint16 count, rxlen, loopcount;

	hal_assert( len > 0 );

	// @attention
	// @todo
	// The hardware transceiver will transfer to RX mode automatically after sending
	// the whole frame. However, there maybe a little delay for the transceiver to establish 
	// an stable RX mode. Or you may wait for the oscillator stable.
	
	cc->state = CC2420_STATE_SENDING;

	/* If the "option" parameter indicates the frame needs an ACK response, then we 
	 * should modify the frame control field of this frame and set the ACK REQUEST bit
	 * in the frame control. 
	 *
	 * If b5 in the frame control byte is set to 1, then it indicates ACK request
	 * according to cc2420 and 802.15.4 specification
	 */

	if (option & 0x01)
	{
		buf[1] |= (0x01 << 5);
	}
	else{
		buf[1] &= (~(0x01 << 5));
	}

	/* @todo:
	 * we should check whether the last sending is really finished or not. If the MCU 
	 * runs very fast, we should add this check. Though it's not apparent on the slow
	 * MCU such as Atmega128.
	 */
	// while (HAL_READ_CC_SFD_PIN() == 1) {};

	count = _cc2420_writetxfifo( cc, (char*)&(buf[0]), len, option );
	cc->state = CC2420_STATE_RECVING;

	// If the frame was succcesfully sent and it requires ACK response, then we should
	// check for the ACK frame. Theoretically speaking, the frame received next to the 
	// last sending frame may be not the ACK, but we can assume it should be ACK by 
	// the help of cc2420 transceiver. 

	if ((count > 0) && (buf[1] & 0x20))
	{
		// wait for the sending really finished.
		//while (HAL_READ_CC_SFD_PIN() == 1) {};
		//原本打算改为这一句，等到发送结束就做下一步，比上一句自己估算时间好。
		//但是实测发现不行。
		//hal_delayus( 950);


		//cc2420_setrxmode( cc );
		//cc->state = CC2420_STATE_RECVING;
		// todo: _cc2420_waitfor_crystal_oscillator( cc );

		// @warning
		// @todo important!
		// you should allocate a separate buffer for ack frame to avoid overwritten!!!
		// 
		// If there's already a frame in the cc->rxbuf, it will be overwrite by the
		// the ACK frame!!!  Suggest you always read the frame out from this object
		// as soon as possible. 

		// @attention
		// According to cc2420 and 802.15.4 datasheet, the receiver should start to 
		// send ACK frame after Tack and 12 symbol time. The Sender should check ACK
		// frame after this time plus additional ACK frame receiving time. 

		// @warning
		// assert( global interrupts is enabled here )
		// You should guarantee the global interrupt is enabled at this point, or else
		// you're imporssible to got the ACK frame returned. In the last main version 
		// two years ago, hal_enable_interrupts() is called here. But in this version,
		// we use hal_atomic_begin() and hal_atomic_end(). And hal_enable_interrupts()
		// hal_disable_interrupts() are suggested not to be used anymore.

		// todo suggest cc2420_readrxfifo here

		// @todo
		// QQQ
		// ???
		// why cc->rxlen = 0 here? 
		// rxlen = 0 seems ok
		// this will cause unnecessary frame loss during transmission because the rxbuf may 
		// has some frame

		// If this frame needs ACK response, then we can check for ACK here.
		// If there's no ACK frame received, then we can assume the frame just sent
		// was lost. This transmission is failed.

		cc->rxlen = 0;

		rxlen = 0; 
		loopcount = 0;
		// ???: I'm not sure whether it's enough to loop 0xFF times to call cc2420_recv?
		// shall we try it more times?
		// todo: you should use timer instead of loopcount to control the checking period
		while ((rxlen == 0) && (loopcount < (0x3FF-1)))
		{	
			hal_delayus( 10 );
			rxlen = cc2420_recv( cc, (char*)&(cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0 );
			loopcount ++;
		}

		/* Based on the above analysis and cc2420 transceiver's help, we can assume the 
		 * frame just received is ACK frame. This can simplify the software. 
		 */

		// todo: 
		// ???
		// 如果收到ACK,还必须比较ACK帧中的sequence number和txbuf中的sequence number
		// 两个只有一致才能说明ack确认的是txbuf中的frame

		// if hasn't received the ACK frame, we had to return 0 to indicate the sending
		// is failed.
		if (rxlen == 0)
		{
			count=0;
		}
		else 
		{
			// if (buf[seqid_idx] != cc->rxbuf[]) 
			//	count = 0;
		}
	}
	return count;
}

/* cc2420_recv()
 * Read a frame from cc2420 adapter's internal buffer or from cc2420 transceiver 
 * directly. Attention this function should only called outside of ISR in non-Interrupt
 * mode.
 *
 * This function will also parse the frame to get the RSSI and LQI value, and save 
 * them temporarily into cc->rssi and cc->lqi. Then you can call cc2420_rssi(...)
 * and cc2420_lqi(...) to retrieve them out. 
 * 
 * @return
 *	0			no frame received in the buffer
 *	>0          frame received in the buffer. The returned value is the data length. 
 *              The first byte in the "buf" is the frame length. The returned length
 *              should euqla to "frame length + 1".
 *
 * @todo
 * If the cc2420 is working in "manual ack" mode, then the cc2420_recv() must send a ACK
 * when the ACK REQUEST bit in the frame control field is set. However, currently, 
 * the cc2420 transceiver is set to AUTO ACK mode, so the software part is simplified.
 */
uint8 cc2420_recv( TiCc2420Adapter * cc, char * buf, uint8 size, uint8 option )
{
	cpu_atomic_t cpu_status;
	uint8 ret = 0;

	if (cc->state == CC2420_STATE_SENDING)
	{
		/* @todo
		 * You shouldn't check whether the last sending is complete or not. If the 
		 * program runs much faster than the cc2420 transmission speed, then it may lead 
		 * to unecessary frame loss at the sender. 
		 * In order to avoid such unecessary frame loss, you should read FIFO pin to 
		 * check the transceiver's state. 
		 * 
		 * Another idea is simply delay here. Though this will decreasing the performance.
		 * hal_delayus( 950 );
		 */

		//cc2420_setrxmode( cc );
		//_cc2420_waitfor_crystal_oscillator( cc );
		cc->state = CC2420_STATE_RECVING;
	}
	else if (cc->state != CC2420_STATE_RECVING)
	{
		return 0;
	}

	// @assert: the cc2420 adapter is in RECVING state now.

	// @attention
	// @warning
	// You should set the cc2420's FIFOP threshold to the largest possible size of 
	// the frame. Because this function will call _cc2420_readrxfifo() when both 
	// FIFO and FIFOP are high! This assume the two pin indicate the entire frame 
	// has already been received by cc2420
	cpu_status = _cpu_atomic_begin();
	{
		/* If there's already a frame pending inside the rxbuf, then move this frame into
		 * parameter buf. and try to pull the new incoming frames from the transceiver's 
		 * hardware. 
		 */
		if (cc->rxlen > 0)
		{	
			hal_assert( cc->rxlen <= size );
			memmove( (void *)buf, (void *)(&(cc->rxbuf[0])), cc->rxlen );
			ret = cc->rxlen;
			cc->rxlen = 0;

			// we should move the frame inside cc2420 transceiver to MCU's RAM as fast as 
			// possible. they will reduce the possibility for frame dropping.
			//
			// if there's still some data pending inside the cc2420's RXFIFO buffer, then 
			// read them from cc2420 to mcu's ram. this often occurs when the sending 
			// rate is too high and exceeds the receiver's processing rate. 
			//
			if (CC_READ_FIFOP_PIN() && HAL_READ_CC_FIFO_PIN()) 
			{
				cc->rxlen = _cc2420_readrxfifo( cc, (char*)(&cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00 );

				/* @attention
				 * If the frame has an ACK request, then the transceiver cc2420's hardware will
				 * send ACK back automatically. However, you must enable the AUTOACK feature 
				 * of cc2420 during its initialization. 
				 * 
				 * Since cc2420 can send ACK automatically, we needn't to send ACK manually
				 * in the source code. 
				 * 
				 * If you want to send ACK manually, you can generate your own ACK frame similar
				 * to a normal data frame, or else send command SACK or SACKPEND to cc2420. 
				 * cc2420 can recognize the two commands and send ACK. 
				 */

				/* attention here cc->rxlen should be large enough or else this frame must 
				 * be an invalid frame, and it should be discarded. 
				 * attention cc->rxlen includes the first length byte.
				 */
				hal_assert( (cc->rxlen == 0) || (cc->rxlen >= CC2420_MIN_FRAME_LENGTH) );
				if (cc->rxlen >= CC2420_MIN_FRAME_LENGTH)
				{
					cc->rssi = cc->rxbuf[cc->rxlen-2];
					cc->lqi = cc->rxbuf[cc->rxlen-1];

					// If the crc checksum failed, then simply drop this frame.
					if(((cc->lqi >> 7)==0x00))
					{
						cc->rxlen = 0;
					}
				}
			}
			else{
				// todo: clear cc2420 rxbuf?
				//cc2420_sendcmd( cc, 0x08 );
				//cc2420_sendcmd( cc, 0x08 );
			}
		}

		/* If there's no frame pending inside the rxbuf, then try to pull a frame from 
		 * the transceiver into parameter buf directly. 
		 */
		else{
			if (CC_READ_FIFOP_PIN() && HAL_READ_CC_FIFO_PIN()) 
			{
				ret = _cc2420_readrxfifo( cc, buf, size, 0x00 );

				/* @attention
				 * If the frame has an ACK request, then the transceiver cc2420's hardware will
				 * send ACK back automatically. However, you must enable the AUTOACK feature 
				 * of cc2420 during its initialization. 
				 * 
				 * Since cc2420 can send ACK automatically, we needn't to send ACK manually
				 * in the source code. 
				 * 
				 * If you want to send ACK manually, you can generate your own ACK frame similar
				 * to a normal data frame, or else send command SACK or SACKPEND to cc2420. 
				 * cc2420 can recognize the two commands and send ACK. 
				 */

				/* attention here ret value should be large enough or else this frame must 
				 * be an invalid frame, and it should be discarded. 
				 */
				hal_assert( (ret == 0) || (ret >= CC2420_MIN_FRAME_LENGTH) );
				if (ret >= CC2420_MIN_FRAME_LENGTH)
				{
					cc->rssi = buf[ret-2];
					cc->lqi = buf[ret-1];

					// If the crc checksum failed, then simply drop this frame.
					if(((cc->lqi>>7)==0x00))
					{
						ret = 0;
					}
				}
			}
			else{
				// todo: clear cc2420 rxbuf?
				//cc2420_sendcmd( cc, 0x08 );
				//cc2420_sendcmd( cc, 0x08 );
			}
		}
	}
    _cpu_atomic_end(cpu_status); 

	return ret;
}
/*
uint8 cc2420_iobsend( TiCc2420Adapter * cc, TiIoBuf * iobuf, uint8 option )
{
    return cc2420_send( cc, iobuf_ptr(iobuf), iobuf_length(iobuf), option );
}

uint8 cc2420_iobrecv( TiCc2420Adapter * cc, TiIoBuf * iobuf, uint8 option )
{
    uint8 count = cc2420_recv( cc, iobuf_ptr(iobuf), iobuf_size(iobuf), option );
    iobuf_setlength( iobuf, count );
    return count;
}
*/
void cc2420_evolve( TiCc2420Adapter * cc )
{
	// the following section will check flag variable and try to read data from 
	// cc2420 hardware to MCU's memory when there's incomming frame pending inside
	// cc2420
	//
	//if ((m_fifop_request > 0) && (m_rxbuf_len == 0))
	{
		//cc2420_readrxfifo( cc );
		//m_fifop_request --;
		//m_fifop_request = 0;
	}
}


/* set the cc2420 transceiver into receiving mode
 */
inline uint8 cc2420_setrxmode( TiCc2420Adapter * cc )
{
	cc2420_sendcmd( cc, CC2420_SRXON );
	return SUCCESS;
}

/* set the cc2420 transceiver into sending mode
 */
inline uint8 cc2420_settxmode( TiCc2420Adapter * cc )
{
	cc2420_sendcmd( cc, CC2420_STXON );
	return SUCCESS;
}

/* cc2420 transceiver's mode in its datasheet
 *  - voltage regulation disabled (1uA)
 *  - power down (PD, voltage regulator enabled)  20uA
 *  - idle (XOSC running. Voltage regulator must be enabled in this case) 426uA
 *    attention some times the idle mode equals to power down mode in the datasheet.
 *  - receive mode 19.7 mA
 *  - transmit mode 8.5-17.4 mA
 * 
 * The "Voltage Regulator" can be enabled through VREG_EN pin.
 * The oscillator can be start/stop by issuing a command to cc2420 throught SPI interface.
 * 
 * sleep() and wakeup() are two standard interface functions which should be supported
 * by all hardware adapters. They're used to help implementing low power programming.
 */

inline uint8 cc2420_sleep( TiCc2420Adapter * cc )
{
	/* Assert the voltage regulator is on now. Actually, the voltage regulator is on
	 * since the device is powered on and never be turned off in this version. */
	return cc2420_oscoff( cc );
}

inline uint8 cc2420_wakeup( TiCc2420Adapter * cc )
{
	/* Assert the voltage regulator is already on now */
	return cc2420_oscon( cc );
}

/*******************************************************************************
 * cc2420 PIN based operations
 * These functions can be called in any case.
 ******************************************************************************/

/* attention: according to the description of cc2420 datasheet, the CCA pin is only
 * valid when the receiver has been enabled for at least 8 symbol periods. each symbol
 * periods is about 125us
 */
uint8 cc2420_ischannelclear( TiCc2420Adapter * cc )
{
    //return HAL_READ_CC_CCA_PIN();
	// @todo
    return true;
}

/* turn on the cc2420 VREF
 * attention you should wait long enough to guarantee the voltage is stable and ok 
 */
inline uint8 cc2420_vrefon( TiCc2420Adapter * cc )
{
    HAL_SET_CC_VREN_PIN();                    //turn-on  
    hal_delayus( 1800 );    
	return SUCCESS;
}

/* turn off the cc2420 VREF */
inline uint8 cc2420_vrefoff( TiCc2420Adapter * cc )
{
	HAL_CLR_CC_VREN_PIN();                    //turn-off  
    hal_delayus( 1800 );  
	return SUCCESS;
}

inline uint8 cc2420_powerdown( TiCc2420Adapter * cc )
{
	return cc2420_vrefoff( cc );
}

/* Reference
 *	- cc2420 datasheet: state machine of cc2420.
 */
inline uint8 cc2420_powerup( TiCc2420Adapter * cc )
{
	return cc2420_vrefon( cc );
}

/*******************************************************************************
 * cc2420 command operations
 ******************************************************************************/

/* send a command to cc2420. This is function is very important because it's the 
 * foundation of all the other command functions.
 *
 * @return 
 *	command execution status
 */
uint8 cc2420_sendcmd( TiCc2420Adapter * cc, uint8 addr )
{
	uint8 status;

	cpu_atomic_t cpu_status = _cpu_atomic_begin();
	HAL_CLR_CC_CS_PIN(); 
	status = _cc2420_spi_put( addr );
    HAL_SET_CC_CS_PIN();
    _cpu_atomic_end(cpu_status); 
	
	return status;
}

uint8 cc2420_getcmdstatus( TiCc2420Adapter * cc) 
{
    return cc2420_sendcmd(cc,CC2420_SNOP);
}

/* No operation. Has no other effects than reading out cc2420's status-bit */
inline uint8 cc2420_snop( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_STXON );
}

/* start the 16MHz crystal oscllator required by cc2420 
 * @return 
 *	0		success
 *
 * @modified by zhangwei on 20090720
 *	- revision
 */
inline uint8 cc2420_oscon( TiCc2420Adapter * cc )
{
	uint8 i;
	uint8 status;
	bool osx_stable = FALSE;
	
	i = 0;
	cc2420_sendcmd( cc, CC2420_SXOSCON );
	while ((i < 200) && (osx_stable == FALSE))
	{
		hal_delayus(100);
		status = cc2420_sendcmd( cc, CC2420_SNOP );
		
		// bit6 of status: XOSC16M_STABLE bit. 1 means the 16MHz crystal oscllator is 
		// running successfully

		if (status & (1 << 6))
		{
	        osx_stable = TRUE;
			break;
        }

		i++;
    }
  
	if (!osx_stable) 
	{
		// attention: when there's problems, you can add the following assert() in
		// the source code and check whether the oscillator is really started or not.
		//hal_assert( false );

		return false;
    }
  
	return true;
}

/* turn-off cc2420's 16MHz crystal oscillator */
inline uint8 cc2420_oscoff( TiCc2420Adapter * cc ) 
{
	cc2420_sendcmd( cc, CC2420_SXOSCOFF );   
    return SUCCESS;
}

/* Waits for the crystal oscillator to become stable by polling the SPI status byte. 
 * You must wait until it is stable before doing further sending and receiving. 
 * 
 * @attention
 * Note that this function will lock up if the SXOSCON command strobe has not been 
 * given before the function call.
 */
void _cc2420_waitfor_crystal_oscillator( TiCc2420Adapter * cc ) 
{
	uint8 status;

	do{	   
		status = cc2420_sendcmd( cc, CC2420_SNOP );
	}while (!(status & (1 << CC2420_XOSC16M_STABLE)));
}

inline uint8 cc2420_calibrate( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_STXCAL );
}

inline uint8 cc2420_rxon( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_SRXON );
}

inline uint8 cc2420_txon( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_STXON );
}

/* similar to STXON. send a frame out. nothing happens if the channel is busy.
 * refer to: cc2420 datasheet on STXONCCA, radio control status machine, CCA
 *
 * - transmission is enabled by issuing a STXON or STXONCCA command strobe. see the 
 * radio control status machine section in cc2420 datasheet on Page42 for an illusion
 * of how the transmit command strobe affect the state of cc2420. 
 * - the STXONCCA strobe is ignored if the channel is busy. see the clear channel 
 * assessment section in cc2420 datasheet 49 for detail.
 */
inline uint8 cc2420_txoncca( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_STXONCCA );
}

inline uint8 cc2420_rfoff( TiCc2420Adapter * cc )
{
	cc2420_sendcmd( cc, CC2420_SRFOFF );
	return SUCCESS;
}

void cc2420_switchtomode( TiCc2420Adapter * cc, uint8 mode )
{
    // todo
}

inline uint8 cc2420_flushrx( TiCc2420Adapter * cc )
{
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );
	cc2420_sendcmd( cc, CC2420_SFLUSHRX );
	return SUCCESS;
}

inline uint8 cc2420_flushtx( TiCc2420Adapter * cc )
{
	return cc2420_sendcmd( cc, CC2420_SFLUSHTX );
}

inline uint8 cc2420_ack( TiCc2420Adapter * cc )              
{
	return cc2420_sendcmd( cc, CC2420_SACK );
}

inline uint8 cc2420_rxdec( TiCc2420Adapter * cc )               
{
	return cc2420_sendcmd( cc, CC2420_SRXDEC );
}

inline uint8 cc2420_txenc( TiCc2420Adapter * cc )              
{
	return cc2420_sendcmd( cc, CC2420_STXENC );
}


/*******************************************************************************
 * cc2420 register/memory access functions and configuration functions.
 ******************************************************************************/

/* write specific data on the specified address of cc2420 */
uint8 cc2420_writeregister( TiCc2420Adapter * cc, uint8 addr, uint16 data)
{
	uint8 status;
	
	cpu_atomic_t cpu_status = _cpu_atomic_begin();

	HAL_CLR_CC_CS_PIN();
	status = _cc2420_spi_put( addr );
	if (addr > 0x0E) 
	{
		_cc2420_spi_put( data >> 8 );
		_cc2420_spi_put((uint8)(data & 0xff));
	}
	_cpu_atomic_end(cpu_status); 
	HAL_SET_CC_CS_PIN();

	return status;
}

/* read data from specified address of cc2420 */
uint16 cc2420_readregister( TiCc2420Adapter * cc, uint8 addr )
{
	uint16 data = 0;
	uint8 status;

	cpu_atomic_t cpu_status = _cpu_atomic_begin();

	HAL_CLR_CC_CS_PIN();
	status = _cc2420_spi_put( addr | 0x40 );
	data = _cc2420_spi_put( 0 );

	data = (data << 8) | (_cc2420_spi_put( 0 ));
	HAL_SET_CC_CS_PIN();

	_cpu_atomic_end(cpu_status); 
	return data;
}

uint8 cc2420_readlargeram( TiCc2420Adapter * cc, uint16 addr, uint8 length, uint8 *buffer )
{
	uint8 i = 0;
	uint8 status;

	cpu_atomic_t cpu_status = _cpu_atomic_begin();

	HAL_CLR_CC_CS_PIN();
		  
	status =  _cc2420_spi_put((addr & 0x7F) | 0x80);
       
    status = _cc2420_spi_put((addr >> 1) & 0xe0);
    
	for (i = 0; i < length; i++) 
	{
		buffer[i]=_cc2420_spi_get();
    }
	
	HAL_SET_CC_CS_PIN();
    _cpu_atomic_end(cpu_status); 

	return SUCCESS;
}

//---------------------------------------------------------连续写cc2420ram
uint8 cc2420_writelargeram( TiCc2420Adapter * cc, uint16 addr, uint8 length, uint8 *buffer)
{
	uint8 i = 0;
	uint8 status;

	cpu_atomic_t cpu_status = _cpu_atomic_begin();

	HAL_CLR_CC_CS_PIN();
	
	status = _cc2420_spi_put((addr & 0x7F) | 0x80);
	
	status = _cc2420_spi_put((addr >> 1) & 0xC0)	;

	for (i = 0; i < length; i++) 
	{
		_cc2420_spi_put(buffer[i]);
	}

	HAL_SET_CC_CS_PIN();
    _cpu_atomic_end(cpu_status); 

	return SUCCESS;
}



//------------------------------------------------------------------
inline uint8 cc2420_enable_autoack( TiCc2420Adapter * cc )
{
	cc->param[CP_MDMCTRL0] |= (1 << CC2420_MDMCTRL0_AUTOACK);
    return cc2420_writeregister( cc, CC2420_MDMCTRL0,cc->param[CP_MDMCTRL0]);
}

inline uint8 cc2420_disable_autoack( TiCc2420Adapter * cc )
{
	cc->param[CP_MDMCTRL0] &= ~(1 << CC2420_MDMCTRL0_AUTOACK);
    return cc2420_writeregister( cc, CC2420_MDMCTRL0,cc->param[CP_MDMCTRL0]);
}

inline uint8 cc2420_enable_addrdecode( TiCc2420Adapter * cc )
{
    cc->param[CP_MDMCTRL0] |= (1 << CC2420_MDMCTRL0_ADRDECODE);
    return cc2420_writeregister( cc, CC2420_MDMCTRL0,cc->param[CP_MDMCTRL0]);
}

inline uint8 cc2420_disable_addrdecode( TiCc2420Adapter * cc )
{
    cc->param[CP_MDMCTRL0] &= ~(1 << CC2420_MDMCTRL0_ADRDECODE);
    return cc2420_writeregister( cc, CC2420_MDMCTRL0,cc->param[CP_MDMCTRL0]);
}


/*
void CC2420_SetCCAMode(USIGN8 mode)
{
USIGN16 temp;
temp = CC2420_ReadRegister(CC2420_MDMCTRL0);
CC2420_WriteRegister(CC2420_MDMCTRL0, ((temp & 0xff3f) | (mode<<6)));
}
USIGN8 CC2420_GetCCAMode()
{
return (CC2420_ReadRegister(CC2420_MDMCTRL0) & 0x00c0)>>6;
}
*/
void cc2420_setcoordinator( TiCc2420Adapter * cc, bool flag )
{
	uint16 temp;
	temp = cc2420_readregister( cc, CC2420_MDMCTRL0 );
	if (flag)//Coordinator and Auto Ack
		cc2420_writeregister( cc, CC2420_MDMCTRL0, (temp | 0x1010) );
	else    //other device type and Auto Ack
		cc2420_writeregister( cc, CC2420_MDMCTRL0, (temp | 0x0010) );
}

/* select the communication channel of cc2420 */
inline uint8 cc2420_setchannel( TiCc2420Adapter * cc, uint8 chn )
{
	int ctrl;

	hal_assert( (chn >= 11) && (chn <=26));

	ctrl = (uint16) (chn - 11);	             // subtract the base channel 
	ctrl = ctrl + (ctrl << 2);                   // multiply with 5, which is the channel spacing
	ctrl = ctrl + 357 + 0x4000;                  // 357 is 2405-2048, 0x4000 is LOCK_THR = 1

	cc->param[CP_FSCTRL] = ctrl;
	cc2420_writeregister( cc, CC2420_FSCTRL, ctrl );
	return SUCCESS;
}

/*
void CC2420_SetIEEEAddress(USIGN8 *ieeeaddr)
{
CC2420_WriteRam(CC2420_RAM_IEEEADR, 8, ieeeaddr);
}
void CC2420_GetIEEEAddress(USIGN8 *ieeeaddr)
{
CC2420_ReadRam(CC2420_RAM_PANID, 8, ieeeaddr);
}
*/

/* set the cc2420 short address
 * assume cc2420 is using short address with pan id currently 
 */
inline uint8 cc2420_setshortaddress( TiCc2420Adapter * cc, uint16 addr)
{
	addr = toLSB16(addr);
	cc2420_writelargeram( cc, CC2420_RAM_SHORTADDR, 2, (uint8 *)&addr );
	return SUCCESS;
}

inline uint8 cc2420_getshortaddress( TiCc2420Adapter * cc, uint16 * addr )
{
	cc2420_readlargeram( cc, CC2420_RAM_SHORTADDR, 2, (uint8 *)addr );
	return SUCCESS;
}

/* set cc2420 pan id 
 * assume the frame control bytes in the frame have correct settings on pan and 
 * address. 
 */
inline uint8 cc2420_setpanid( TiCc2420Adapter * cc, uint16 id )
{
	id = toLSB16(id);
    cc2420_writelargeram( cc, CC2420_RAM_PANID, 2, (uint8 *)&id );
    return SUCCESS;
}

inline uint8 cc2420_getpanid( TiCc2420Adapter * cc, uint16 * id )
{
	cc2420_readlargeram( cc, CC2420_RAM_PANID, 2, (uint8 *)&id );
	return SUCCESS;
}


inline uint8 cc2420_settxpower( TiCc2420Adapter * cc, uint8 power )
{
    uint16 ctrl;
    
    switch (power)
    {
	case CC2420_POWER_1:  ctrl = 0xa0e3; break;
    case CC2420_POWER_2:  ctrl = 0xa0e7; break;
    case CC2420_POWER_3:  ctrl = 0xa0eb; break;
    case CC2420_POWER_4:  ctrl = 0xa0ef; break;
    case CC2420_POWER_5:  ctrl = 0xa0f3; break;
    case CC2420_POWER_6:  ctrl = 0xa0f7; break;
    case CC2420_POWER_7:  ctrl = 0xa0fb; break;
    case CC2420_POWER_8:  ctrl = 0xa0ff; break;
    default :             ctrl = 0xa0ef; break; 
    }
    
    return cc2420_writeregister(cc, CC2420_TXCTRL, ctrl);     
}




/*******************************************************************************
 * cc2420 adapter enhanced functions
 ******************************************************************************/

/* return the rssi(received signal strength indicator) of the current frame just
 * read out by cc2420_recv().
 * 
 * cc2420_readrxfifo() will read the whole frame into cc->rxbuf. it will also read
 * the extra two bytes and place them into rxbuf. these two bytes are "rssi" and "lqi"
 * according to cc2420 datasheet. 
 * 
 * when the user call cc2420_recv() to retrieve the frame out from cc->rxbuf, then 
 * the cc2420_recv() function will return the data only and assign the last two bytes
 * to cc->rssi and cc->lqi here.  
 * 
 * Q: How cc2420 transceiver deals with RSSI, LQI and CRC result?
 * R: When MODEMCTRL0.AUTOCRC is set the two FCS bytes are replaced by the RSSI 
 * value, average correlation  value (used  for LQI) and CRC OK/not OK. 
 *    The  first FCS byte is replaced by the 8-bit RSSI value. The 7 least significant 
 * bits in the last FCS byte are replaced by the average correlation value. This 
 * correlation value may be used as a basis for calculating the LQI. The most 
 * significant bit in the last byte of each frame is set high if the CRC of the 
 * received frame is correct and low otherwise. 
 */ 
uint8 cc2420_rssi( TiCc2420Adapter * cc )
{
	return cc->rssi;
}

uint8 cc2420_lqi( TiCc2420Adapter * cc )
{
	return (cc->lqi & 0x7F);
}

bool cc2420_crctest( TiCc2420Adapter * cc )
{
	return ((cc->lqi >> 7) == 1);
}

TiFrameTxRxInterface * cc2420_interface( TiCc2420Adapter * cc, TiFrameTxRxInterface * intf )
{
    memset( intf, 0x00, sizeof(TiFrameTxRxInterface) );
    intf->provider = cc;
    intf->send = (TiFunFtrxSend)cc2420_send;
    intf->recv = (TiFunFtrxRecv)cc2420_recv;
    intf->evolve = (TiFunFtrxEvolve)cc2420_evolve;
    intf->switchtomode = (TiFunFtrxSwitchToMode)cc2420_switchtomode;
    intf->ischnclear = (TiFunFtrxIsChannelClear)cc2420_ischannelclear;
    intf->enable_autoack = (TiFunFtrxEnableAutoAck)cc2420_enable_autoack;
    intf->disable_autoack = (TiFunFtrxDisableAutoAck)cc2420_disable_autoack;
    intf->enable_addrdecode = (TiFunFtrxEnableAddrDecode)cc2420_enable_addrdecode;
    intf->disable_addrdecode = (TiFunFtrxDisableAddrDecode)cc2420_disable_addrdecode;
    intf->setchannel = (TiFunFtrxSetChannel)cc2420_setchannel;
    intf->setpanid = (TiFunFtrxSetPanId)cc2420_setpanid;
    intf->getpanid = (TiFunFtrxGetPanId)cc2420_getpanid;
    intf->setshortaddress = (TiFunFtrxSetShortAddress)cc2420_setshortaddress;
    intf->getshortaddress = (TiFunFtrxGetShortAddress)cc2420_getshortaddress;
    intf->settxpower = (TiFunFtrxSetTxPower)cc2420_settxpower;
    intf->getrssi = (TiFunFtrxGetRssi)cc2420_rssi;
    return intf;
}

/*
TiFrameTxRxInterface * cc2420_interface( TiCc2420Adapter * cc )
{
    memset( &(cc->intf), 0x00, sizeof(TiFrameTxRxInterface) );

    cc->intf.send = (TiFunFtrxSend)cc2420_send;
    cc->intf.recv = (TiFunFtrxRecv)cc2420_recv;
    cc->intf.evolve = (TiFunFtrxEvolve)cc2420_evolve;
    cc->intf.switchtomode = (TiFunFtrxSwitchToMode)cc2420_switchtomode;
    cc->intf.ischnclear = NULL; //(TiFunFtrxIsChannelClear)cc2420_ischannelclear;
    cc->intf.enable_autoack = (TiFunFtrxEnableAutoAck)cc2420_enable_autoack;
    cc->intf.disable_autoack = (TiFunFtrxDisableAutoAck)cc2420_disable_autoack;
    cc->intf.enable_addrdecode = (TiFunFtrxEnableAddrDecode)cc2420_enable_addrdecode;
    cc->intf.disable_addrdecode = (TiFunFtrxDisableAddrDecode)cc2420_disable_addrdecode;
    cc->intf.setchannel = (TiFunFtrxSetChannel)cc2420_setchannel;
    cc->intf.setpanid = (TiFunFtrxSetPanId)cc2420_setpanid;
    cc->intf.getpanid = (TiFunFtrxGetPanId)cc2420_getpanid;
    cc->intf.setshortaddress = (TiFunFtrxSetShortAddress)cc2420_setshortaddress;
    cc->intf.getshortaddress = (TiFunFtrxGetShortAddress)cc2420_getshortaddress;
    cc->intf.settxpower = (TiFunFtrxSetTxPower)cc2420_settxpower;
    cc->intf.getrssi = (TiFunFtrxGetRssi)cc2420_rssi;

    return &(cc->intf);
}
*/

/******************************************************************************
 * cc2420_dump
 * Dump cc2420's internal state to the debug output. This function is for debugging
 * use only.
 *****************************************************************************/

void cc2420_dump( TiCc2420Adapter * cc )
{
	#ifdef GDEBUG
    /*{
    FAST2420_GETREG(cc->spi,CC2420_MDMCTRL0, &rereg);   
    uart_putchar(g_uart,(char)rereg);
    uart_putchar(g_uart,(char)(rereg>>8));
    
    FAST2420_GETREG(cc->spi,CC2420_MDMCTRL1, &rereg);    
    uart_putchar(g_uart,(char)rereg);
    uart_putchar(g_uart,(char)(rereg>>8));
    
    FAST2420_GETREG(cc->spi,CC2420_IOCFG0, &rereg);    
    uart_putchar(g_uart,(char)rereg);
    uart_putchar(g_uart,(char)(rereg>>8));
    //FAST2420_GETREG(cc,CC2420_SECCTRL0, &rereg); 
    } */ 
    #endif
}

/*******************************************************************************
 * cc2420 interrupt related functions
 *
 * cc2420_enable_fifop
 * Enable the cc2420's FIFOP interrupt
 * 
 * cc2420_disable_fifop
 * Disable the cc2420's FIFOP interrupt
 *
 * cc2420_enable_sfd
 * Enable the cc2420's SFD interrupt
 * 
 * cc2420_disable_sfd
 * Disable the cc2420's SFD interrupt
 *
 *****************************************************************************/

/* enable cc2420 FIFOP interrupt. 
 * 
 * - according to the FIFOP description in cc2420's datasheet, the FIFOP goes to high
 * when the data length in RXFIFO exceed the threshold IOCFG0.FIFOP_THR or the 
 * frame is completely received in RXFIFO. 
 * 
 * - FIFOP goes to low once one bytes has been read out from RXFIFO.
 */
inline void cc2420_enable_fifop( TiCc2420Adapter * cc )
{
	switch (cc->id)
	{
	case 0:
		//EICRB &= ~(1 << 4);	// up edge of FIFOP to trigger the interrupt
								// register EICRB control the features of INT4,5,6,7
		EICRB |= (1 << 4);
		EICRB |= (1 << 5);
		EIMSK |= (1 << 6);		// = sbi(EIMSK, INT6)
		break;
	case 1:
		break;
	}
	return;
}

inline void cc2420_disable_fifop( TiCc2420Adapter * cc )
{
	switch (cc->id)
	{
	case 0:
	    EIMSK &= (1 << INT6);		// cbi( EIMSK, INT6 );
		break;
	case 1:
		break;
	}
}

inline void cc2420_enable_sfd( TiCc2420Adapter * cc )
{
	// todo
}

inline void cc2420_disable_sfd( TiCc2420Adapter * cc )
{
	// todo
}

/******************************************************************************
 * cc2420_default_listener
 * A callback listerner function can be used when call cc2420_open(). If the master
 * module doesn't provide a propriate listener function, then you can use the 
 * this default implementation and let the osx kernel help you do the left. 
 * The default listener will generate an event and put this event into a system
 * event queue.
 *****************************************************************************/

void cc2420_default_listener( void * ccptr, TiEvent * e )
{
	TiCc2420Adapter * cc = (TiCc2420Adapter *)ccptr;
	hal_notify_ex( EVENT_DATA_ARRIVAL, ccptr, cc->lisowner );
}


/******************************************************************************
 * cc2420 interrupt handler
 * it's called by the cc2420 FIFOP interrupt service handler (ISR). it should 
 * be called when a new frame received. you must call hal_attachhandler(), 
 * cc2420_enable_fifop() and hal_enable_interrupts() to fully enable this handler.
 *****************************************************************************/

/* @attention 
 * refer to cc2420 datasheet page 31
 *
 * - FIFO pin indicates whether there's data in the RXFIFO. and FIFO goes to low
 * when RXFIFO is empty. so you can also judge whether there's data pending in the 
 * RXFIFO by FIFO pin value.  
 *
 * @attention: the FIFOP interrupt is triggered by up edge at cc2420's FIFOP pin
 *	(connect to atmega128's INT6 PIN in GAINZ). you should configure the PIN 
 *  direction and interrupt features during initialization (down edge currently).
 *
 * - The FIFOP pin is high when the number of unread bytes in the RXFIFO exceeds 
 *   the threshold programmed into IOCFG0.FIFOP_THR
 *
 * - when address recognition is enabled the FIFOP will not go high until the 
 *   incoming frame passes address recognition. even if the number of bytes inRXFIFO 
 *   exceeds the programmed threshold
 *
 *	 => how should the interrupt handler do in this case? 
 *      it seems the cc2420 will clear the data in the RXFIFO if address recognition
 *      enabled and recognition failed. if then the interrupt handler needn't do 
 *      any thing. but how about the address recognition disabled?
 *
 * - FIFOP pin will also go high when the last byte of a new packet is received 
 *   even if the threshold is not exceeded. If so  the FIFOP  pin  will go back 
 *   to low once one byte has been read out of the RXFIFO. 
 * 
 *    => summary: FIFOP goes to high when the received bytes in RXFIFO exceed the 
 *       threshold or the whole frame is complete, no matter the address recognition
 *       is enabled or not.
 *    => FIFOP goes to low once one byte has been read out of the RXFIFO.
 *
 * - When address recognition is enabled, data should not be read out of the RXFIFO 
 *   before the address is completely received, since  the frame may be automatically 
 *   flushed by CC2420 if it fails  address recognition. This may be handled by 
 *   using the FIFOP pin, since this pin does not go high until the frame  passes  
 *   address recognition. 
 *
 * Q: what will be if a frame pending inisde cc2420? (???)
 * R: when the first frame arrives, both the FIFOP and FIFO go to high and the 
 *	FIFOP interrupt notify the MCU to process it. 
 *	  assume the MCU hasn't process the FIFOP interrupt in time, the frame will 
 *  be kept inside cc2420's RXFIFO and the second frame arrives now. then overflow 
 *  occurs and the FIFO pin goes to low. at this time, the FIFOP is still high
 *  so the interrupt still has chance to be executes. 
 * 
 *    attention the FIFOP interrupt will not raised to MCU because it's already 
 *  high in the overflow case. there's no chance to change from low to high (we
 *  use the up edge interrupt now).
 * 
 *    suggested processing: if overflow occurs, then you can still read the first 
 *  frame out. then the ISR should be:
 *    1) read first frame out. 
 *    2) check if overflow occurs. if true, then clear the RXFIFO
 *
 * @modified by zhangwei on 20090809
 *	- attention: Remove the debugging source code in this handler. Attention that 
 *  the hal_delay() shouldn't appeared inside this handler because it runs in interrupt 
 *  mode, except you know clearly what the future behavior will be. 
 */
void _cc2420_fifop_handler( void * ccptr, TiEvent * e )
{
    TiCc2420Adapter * cc = (TiCc2420Adapter *)ccptr;

	// HAL_READ_CC_FIFO_PIN() returns 0 means RXFIFO overflow. this should seldom 
	// occur, but the program should be able to recover from this case. 
	// 
	// the FIFO pin is set to high when the length byte of the frame is put into 
	// the RXFIFO and remains high if there's data in the RXFIFO.
	// 
	// RXFIFO may contains serveral serveral frames unless the total length is less
	// than 128. if overflow occurs, then cc2420 will signal to MCU by setting FIFO
	// to low while FIFOP is still high now. data already in the RXFIFO will not be 
	// affected by the overflow, i.e. frame already received maybe read out.
	//
	//if (CC_READ_FIFOP_PIN() && (!HAL_READ_CC_FIFO_PIN())) 
	if ((!HAL_READ_CC_FIFO_PIN()) && CC_READ_FIFOP_PIN())
	//前面那句始终为真，中断时总进入此方块，而使用这句就没问题。它们有什么区别吗？无语了
	//if (!HAL_READ_CC_FIFO_PIN())
	{
		// 0x3F is the RXFIFO register
		
		// todo 
		// 20090724
		// the old version include uncommented cc2420_readregister(cc, 0x3F)
		//cc2420_readregister( cc, 0x3F );		
		// send SFLUSHRX twice to ensure the cc2420's internal RXFIFO is clear. 
		// and both the FIFO and SFD pin go to low. 

		cc2420_sendcmd( cc, 0x08 );
		// hal_delayus( 5 );
		cc2420_sendcmd( cc, 0x08 );
		// hal_delayus( 10 );
		// hal_assert( HAL_READ_CC_FIFO_PIN() == 0 );
		// hal_assert( HAL_READ_CC_SFD_PIN() == 0 );
        return;
    }
    
	// indicate the master program there's new frame coming. you can use either the 
	// share global variable, event queue or semaphore. we use event queue in openwsn
	// implemention. 
	//
	// @attention 
	// - the listener or the master program must read the frame out from RXFIFO as 
	// soon as possible or else the next frame will be padded to the first one. the 
	// current program doesn't take this case into account. 
	// - if there's overflow in the RXFIFO, then the first correct frame will also be 
	// cleared. 

	if (cc->listener != NULL)
	{
		// @attention
		// @warning
		// if you use listener, the master program must repeated call cc2420_recv()
		// to deal with the possible overflow cases. if you call cc2420_recv() only 
		// one time in the listener, then the listener will be never called for the 
		// second time if RXFIFO is overflow. => this may lead the whole application
		// looks deadlock and receive any frames therefore.
		//
		cc->listener( cc->lisowner, NULL );
	}
	else if (cc->rxlen == 0)
	{	
		// @attention
		// @warning
		//	If the master program cannot serve the incoming frames fast enough, then the
		// newly incoming frames will overwrite the last one. This will lead to 
		// frame lossing.
		cc->rxlen = _cc2420_readrxfifo( cc, (char*)(&cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00 );
	}
	else{
		// do nothing in the interrupt handler if the (cc->rxbuf) is still used. 
		// attention the FIFO pin will still keep high because RXFIFO isn't empty now.
		// but no interrupt will be generated later. the master program must call cc2420_recv()
		// to read the data out from cc2420 transceiver's RXFIFO or else overflow 
		// may occur.  and the second interrupt will not raised because FIFOP is already 
		// high.

		// @modified by zhangwei on 20090919
		// The above analysis is correct. In order to simplify the whole process,
		// I add the following line of source code here. The call of cc2420_readrxfifo() 
		// will overwrite last frame inside the rxbuf. 
		// I assume the new incoming frame always has the high priority than the elder one.
		//
		cc->rxlen = _cc2420_readrxfifo( cc, (char*)(&cc->rxbuf[0]), CC2420_RXBUFFER_SIZE, 0x00 );
	}

	// attention: there's a little chance that 2 or more frames pending inside the TXFIFO
	// if so, you need to try to read them out.

    return;
}

/* For high precision time synchronization
 * This handler will be called by hardware timer's capture interrupt. This interrupt
 * will be fired when the SFD pin changes. 
 * 
 * Q: Where can I found the interrupt configuration source code?
 * R: hal_interrupt.c
 */
#ifdef CONFIG_CC2420_SFD
void _cc2420_sfd_handler( void * ccptr, TiEvent * e )
{
	TiCc2420Adapter * cc = (TiCc2420Adapter *)ccptr;
	// todo stop the timer capture here
	// xiejing: probably, you should read the timer value out here
	led_toggle( LED_RED );
}
#endif
