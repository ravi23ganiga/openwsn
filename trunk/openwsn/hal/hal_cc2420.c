/******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network System.
 *
 * Copyright (C) 2005,2006,2007 zhangwei (openwsn@gmail.com)
 * 
 * OpenWSN is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later version.
 * 
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with eCos; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or use macros
 * or inline functions from this file, or you compile this file and link it
 * with other works to produce a work based on this file, this file does not
 * by itself cause the resulting work to be covered by the GNU General Public
 * License. However the source code for this file must still be made available
 * in accordance with section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work based on
 * this file might be covered by the GNU General Public License.
 * 
 *****************************************************************************/ 

/******************************************************************************
 * @author zhangwei on 2006-07-20
 * TCc2420Driver
 * This is an software abstraction of the cc2420 transceiver hardware. you can 
 * fully manipulate the cc2420 hardware through this object.
 * 
 * @modified by zhangwei on 2007-05-14
 * no important modifications. just revision of the source code.
 * 
 * @modified by zhangwei on 20070529
 * revision on this module. simplified the processing of read and write().
 * eliminate some un-necessary variables.
 * 
 * @modified by zhangwei on 20070608
 * eliminate volatile TCc2420Frame pRxInfo;
 *  
 *****************************************************************************/

#ifdef CONFIG_DEBUG
#define GDEBUG
#endif

#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_target.h"
#include "hal_interrupt.h"
#include "hal_cpu.h"
#include "hal_led.h"
#include "hal_assert.h"
#include "hal_spi.h"
#include "hal_cc2420def.h"
#include "hal_cc2420base.h"
#include "hal_cc2420.h"

#ifdef GDEBUG
#include "hal_uart.h"
#endif
#include "hal_global.h"

#ifdef CONFIG_DEBUG
#define GDEBUG
#endif

#ifdef CONFIG_TARGET_DEFAULT
#undef CONFIG_TARGET_DEFAULT
#endif

#if ((!defined(CONFIG_TARGET_OPENNODE_10)) && (!defined(CONFIG_TARGET_OPENNODE_20)) \
  	&& (!defined(CONFIG_TARGET_OPENNODE_30)) && (!defined(CONFIG_TARGET_WLSMODEM_11)))
#define CONFIG_TARGET_DEFAULT
#endif

/* RSSI to Energy Detection conversion
 * RSSI_OFFSET defines the RSSI level where the PLME.ED generates a zero-value */
 
#define RSSI_OFFSET -38
#define RSSI_2_ED(rssi)   ((rssi) < RSSI_OFFSET ? 0 : ((rssi) - (RSSI_OFFSET)))
#define ED_2_LQI(ed) (((ed) > 63 ? 255 : ((ed) << 2)))

static void _cc2420_init(TCc2420 * cc);
static void cc2420_interrupt_init( void );
static bool _hardware_sendframe(TCc2420 * cc, char * framex, uint8 len, bool ackrequest);
//static TCc2420Frame * _hardware_recvframe( TCc2420 * cc,TCc2420Frame *pRRI);
static void __irq cc2420_interrupt_service( void );
static void cc2420_interrupt_handler( TCc2420 * cc );


/******************************************************************************
 * construct a TCc2420 object
 * usually, you should always call configure() after construct the object in the 
 * memory.
 * 
 * this function doesn't interact with the hardware. so it will always be successful.
 * 
 * @param
 * 	buf			memory started address
 * 	size		memory size allocated for this object. this size must be large
 * 				or equal to the object size. 
 * 	spi			TSpiDriver object. the MCU uses this driver to communicate with 
 * 				"cc2420" hardware chip.
 * example: 
 * 	TCc2420 *cc, buf;
 * 	cc = cc2420_construct( buf, ...);
 * 	cc2420_configure( cc, CC2420_CONFIG_PANID, (void*)&panid );
 * 	cc2420_configure( cc, CC2420_CONFIG_NODEADDRESS, (void*)&nodeaddress );
 * 	cc2420_configure( cc, CC2420_CONFIG_TUNNING_POWER, (void*)&power );
 * 	cc2420_configure( cc, CC2420_CONFIG_CHANNEL, (void*)&channel );
 *   cc2420_configure( cc, CC2420_CONFIG_APPLY, NULL );
 * 
 *****************************************************************************/
TCc2420 * cc2420_construct( char * buf, uint16 size, TSpiDriver * spi )
{
	               
	TCc2420 *cc;
	char * out_string = "cc2420 construct succesful!\n";
	
	if (sizeof(TCc2420) > size)
		cc = NULL;
	else
		cc = (TCc2420 *)buf;
	
	if (cc != NULL)
	{	
		memset( (char*)cc, 0x00, sizeof(TCc2420) );
		cc->state = CC_STATE_POWERDOWN;
		cc->nextstate = CC_STATE_POWERDOWN;
		cc->spi = spi;
		cc->panid = CC2420_DEFAULT_PANID;
		cc->address = CC2420_DEFAULT_ADDRESS;
		cc->channel = CC2420_DEFAULT_CHANNEL;
		cc->txlen = 0;
		cc->rxlen = 0;
		cc->sleeprequest = FALSE;
		cc->power = CC2420_POWER_DEFAULT;
		cc->ackrequest = false;
		cc->ack_response = false;
		cc->seqid = 0;
		cc->rssi = 0;
		cc->receiveOn = true;
		
		uart_write(g_uart, out_string,28, 0);
	}
        
	return cc;
}

/******************************************************************************
 * destroy a TCc2420 object 
 * release all necessary resouces.
 *****************************************************************************/ 
void cc2420_destroy( TCc2420 * cc )
{
	if (cc != NULL)
	{
		cc2420_close( cc );
		cc->txlen = 0;
		cc->rxlen = 0;
	}
}

void cc2420_configure( TCc2420 * cc, uint8 ctrlcode, uint16 value, uint8 size )
{
	switch (ctrlcode)
	{
	case CC2420_BASIC_INIT:
	    cc->state = CC_STATE_POWERDOWN;
		cc->nextstate = CC_STATE_POWERDOWN;
		cc->panid = CC2420_DEFAULT_PANID; 
		cc->address = CC2420_DEFAULT_ADDRESS; 
		cc->channel = CC2420_DEFAULT_CHANNEL; 
		cc->txlen = 0;
		cc->rxlen = 0;
		cc->power = 1;
		cc->ackrequest = 0;
		cc->sleeprequest = FALSE;

		spi_configure( cc->spi );
		//spi_open( cc->spi, 0 );
	    _cc2420_init(cc); 
	    cc2420_interrupt_init();

	    break;
	
	case CC2420_CONFIG_PANID:
		cc->panid = value;
		FAST2420_WRITE_RAM_LE(cc->spi,&(cc->panid), CC2420RAM_PANID, 2);
		break;
		
	case CC2420_CONFIG_LOCALADDRESS:
		cc->address = value;
		FAST2420_WRITE_RAM_LE(cc->spi,&(cc->address), CC2420RAM_SHORTADDR, 2);
		break;
		
	case CC2420_CONFIG_TUNNING_POWER:
		cc->power = (uint8)value;
		cc2420_set_power(cc,cc->power);
		break;
		
	case CC2420_CONFIG_CHANNEL:
	    cc->channel = (uint8)value; 
		cc2420_setchannel( cc, value );
		break;
		
	case CC2420_XTAL_SWITCH:
		if(value) 
	    	FAST2420_STROBE(cc->spi,CC2420_SXOSCON);
	    else
	    	FAST2420_STROBE(cc->spi,CC2420_SXOSCOFF);
	    break;
	        
	case CC2420_SET_ACKREQUEST:
	    cc->ackrequest = (uint8 )value;
	    break;
	        
	case CC2420_CONFIG_SNIFFER_MODE:
		// disable address recognition
		// the following instruction will also disable ACK at the same time.
	    FAST2420_SETREG(cc->spi,CC2420_MDMCTRL0,0x02E2) ;
	    break;
	
	// @modified by zhangwei on 20061027
	// though huanghuan regards APPLY is unnecessary, zhangwei still insists a APPLY here. 
	// this is because some configure operations only update the variable in memory, APPLY
	// means make these changes effective. APPLY is a real operation to affect the hardware.
	// pls keep APPLY for future expansion.
	//
	case CC2420_CONFIG_APPLY: 
	                                      //有些配置不需要apply，直接写到某变量就行了
	    FAST2420_WRITE_RAM_LE(cc->spi,&(cc->panid), CC2420RAM_PANID, 2);
	    FAST2420_WRITE_RAM_LE(cc->spi,&(cc->address), CC2420RAM_SHORTADDR, 2);   
	    FAST2420_SETREG(cc->spi,CC2420_TXCTRL, cc->power);    
	                                      //还是觉得没有apply的必要，因为配置包含了硬件，软件，应该
	                                      //根据配置的种类来做事                      
		break;

	default: 
		break;
		
	}	
}

/******************************************************************************
 * cc2420 chip must be initialized successfully before it can be used for sending
 * or receive
 * 
 * Initializes CC2420 for radio communication via the basic RF library functions. 
 * Turns on the voltage regulator, resets the CC2420, turns on the crystal oscillator, 
 * writes all necessary registers and protocol addresses (for automatic address 
 * recognition). Note that the crystal oscillator will remain on (forever).
 *  
 *****************************************************************************/

//----------------------------------------------------------------------------
//  void basicRfInit(BASIC_RF_RX_INFO *pRRI, UINT8 channel, WORD panId, WORD myAddr)
//
//  ARGUMENTS:
//      BASIC_RF_RX_INFO *pRRI
//          A pointer the BASIC_RF_RX_INFO data structure to be used during the first packet reception.
//			The structure can be switched upon packet reception.
//      UINT8 channel
//          The RF channel to be used (11 = 2405 MHz to 26 = 2480 MHz)
//      WORD panId
//          The personal area network identification number
//      WORD myAddr
//          The 16-bit short address which is used by this node. Must together with the PAN ID //orm a
//			unique 32-bit identifier to avoid addressing conflicts. Normally, in a //802.15.4 network, the
//			short address will be given to associated nodes by the PAN coordinator.
//----------------------------------------------------------------------------
void _cc2420_init(TCc2420 * cc) 
{
    uint16 rereg;
    uint8 status;

	uart_write( g_uart, "_cc2420_init 1\n", 15, 0x00 );

    // Make sure that the voltage regulator is on, and that the reset pin is inactive
    SET_VREG_ACTIVE();
    hal_delay(500); //at least delay_us(600);


    SET_RESET_ACTIVE();
    hal_delay(10); // at least delay_us(1). low voltage is effective
    SET_RESET_INACTIVE();
	uart_write( g_uart, "_cc2420_init 2", 15, 0 ); // debug
    hal_delay(500); // at least dela_us(1) 
    status = FAST2420_STROBE(cc->spi,CC2420_SXOSCON);
	while (!(status & 0x40)) 
	{
		uart_putchar( g_uart, status ); // debug
		status = FAST2420_STROBE(cc->spi,CC2420_SXOSCON);
	}
	uart_write( g_uart, "_cc2420_init 3", 15, 0 ); // debug
    hal_delay(1000);
	 
    //FASTSPI_SETREG(CC2420_TXCTRL, 0xA0E3); // To control the output power, added by huanghuan
    FAST2420_SETREG(cc->spi,CC2420_MDMCTRL0, 0x0AF2); // Turn on automatic packet acknowledgment 
    FAST2420_SETREG(cc->spi,CC2420_MDMCTRL1, 0x0500); // Set the correlation threshold = 20
    FAST2420_SETREG(cc->spi,CC2420_IOCFG0, 0x007F);   // Set the FIFOP threshold to maximum
    FAST2420_SETREG(cc->spi,CC2420_SECCTRL0, 0x01C4); // Turn off "Security enable"
    
    // Set the RF channel
    cc2420_setchannel(cc, cc->channel);

	#ifdef GDEBUG
    //led_twinkle(LED_RED,1);
	#endif
 
    // the following sections are used for debugging only. it will dump serveral 
    // important registers to uart. so you can see them on the host computer.
	#ifdef GDEBUG
    //while(1){
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
    //}
    #endif
    
    cc->seqid = 0;
    cc->receiveOn = FALSE;
    cc->ack_response = FALSE;
    
    //FAST2420_UPD_STATUS(cc->spi, (uint8*)(&rereg) );
    //uart_putchar(g_uart,(char)rereg);
	// Wait for the crystal oscillator to become stable
    _cc2420_waitfor_crystal_oscillator(cc->spi);
	// Write the short address and the PAN ID to the CC2420 RAM (requires that the XOSC is on and stable)
  
    FAST2420_WRITE_RAM_LE(cc->spi,&(cc->address), CC2420RAM_SHORTADDR, 2);
    FAST2420_WRITE_RAM_LE(cc->spi,&(cc->panid), CC2420RAM_PANID, 2);
    
    //FAST2420_READ_RAM_LE(cc,reram,CC2420RAM_SHORTADDR,2);
    //reram[0]++;
    //reram[1]++; 
}

/******************************************************************************
 * open the driver for read and write
 * the open process will also configure the cc2420 transceiver according to 
 * last configurations. the confiugration values are kept in the TCc2420 object.  
 *****************************************************************************/
void cc2420_open( TCc2420 * cc )
{
    cc2420_receive_on(g_cc2420);  
    IRQEnable(); 
}

/******************************************************************************
 * close the cc2420 driver
 * this function will cause the driver and transceiver work in POWERDOWN mode. 
 * you must call open() again to enable read/write()
 *****************************************************************************/
void cc2420_close( TCc2420 * cc ) 
{
	// disable cc2420 interrupt
}

uint8 cc2420_state( TCc2420 * cc )
{
	return cc->state;
}

/* return the last I/O operation's result.
 * result & 0x01		still has data sending
 * result & 0x02		still has data in the internal buffer and to be read
 * 
 * Though this function gives you the ability to check the state of this module, 
 * we hope you do not use this function. 
 */
uint8 cc2420_ioresult( TCc2420 * cc )
{
	uint8 ioresult = 0x00;
	
	if (cc->txlen > 0)
		ioresult |= 0x01;
	if (cc->rxlen > 0)
		ioresult |= 0x02;
		
	return ioresult;	
}

/******************************************************************************
 * send a frame out. the frame should be arranged well by the master function
 * 
 * for data frame
 * frame = [2B Frame Control] [1B Sequence Number][4 or 20 Address][n Data Payload][2 FCS]
 * 
 * generally
 * address = [2B PAN Id] [2B Destination] [2B Source]
 *
 * frame control, sequence num, checksum are feeded by the hardware. so you 
 * needn't arrange them. 
 *   
 * @attention
 * 	you must guarantee the internal buffer size is larger than frame length
 * or you may encounter unexpected errors.
 *
 * @parameter
 *  frame		point to an TCc2420Frame structure in the memory
 *  len         the data length in the memory buffer. 
 *              len - 1 is the MAC layer frame length handed to hardware.
 *              "1" means the "length" byte.
 *
 * @return
 *  how many bytes sucessfully wroten to the TCc2420 object. 
 *  attention the value is usually less than the data length sent by the hardware.
 *  generally, it equals to the parameter "len"
 *
 *****************************************************************************/ 
int8 cc2420_rawwrite( TCc2420 * cc, char * frame, uint8 len, uint8 opt )
{
	bool ack;
	int8 count;
	
	if (cc->txlen == 0)
	{
		count = (len-1) & 0x7F;
		cc->txbuf.length = count;  
		memmove( (char*)(&cc->txbuf.control), frame+1, 2 );
		memmove( (char*)(&cc->txbuf.panid), frame+4, 2 );
		memmove( (char*)(&cc->txbuf.nodeto), frame+6, 2 );
		memmove( (char*)(&cc->txbuf.nodefrom), frame+8, 2 );
		//memmove(&cc->txbuf.payload, frame+10, count-11 ); //?    
		// @warning: you must guarantee the data is packed tightly in the memory
		// or else the following memmove() may lead to wrong results
		memmove( (char*)(&cc->txbuf.payload), frame+10, count-9 );    
	
		//if ((opt & 0x01))  
		//	cc->ackrequest = 0;            
        
    	if (cc->ackrequest ==1) 
		{
	    	ack = _hardware_sendframe(cc, frame, len, true);
	    	if (!ack) 
	       		count = -1;
    	}
    	else{
      		_hardware_sendframe(cc, frame, len, false);
		}
		
		cc->nextstate = CC_STATE_IDLE;
	}
	else
		count = 0;

	cc->txlen = 0;
	//cc2420_evolve( cc );
	return (int8)(count & 0x7F);
}

/* @param
 *   frame        point to an TCc2420Frame
 *   opt          default to 0x00
 * 
 * you must initialize the "frame" correctly before you can this function. 
 * you must initialize the following:
 *		length, panid, nodeto, payload
 * generally, you'd better initialize (can simply set it to 0x0000):
 *		control
 * you need not initialize
 *      seqid, nodefrom
 */
int8 cc2420_write( TCc2420 * cc, TCc2420Frame * frame, uint8 opt)
{
	bool ack;
	int8 count;
	
	if (cc->txlen == 0)
	{
		count = frame->length & 0x7F;
		memmove( (char*)(&cc->txbuf), (char*)frame, count ); 
		cc->txbuf.length = count;

		if (cc->ackrequest == 1) 
		{
	    	ack = _hardware_sendframe(cc, (char*)frame, count, true);
	    	if (!ack) 
	    		count = -1;
   		}
    	else{
            _hardware_sendframe(cc, (char*)frame, count, false);
        }
        
		cc->nextstate = CC_STATE_IDLE;
	}
	else
		count = 0;
			
	cc->txlen = 0;
	//cc2420_evolve( cc );
	return (int8)(count & 0x7F);	
}

/******************************************************************************
 * return the received frame entirely to the frame buffer. 
 * read data from the driver's internal buffer. the interrupt service routine 
 * is responsible to place the received data into the internal buffer.
 * you can always call this function to check whether there has data in the internal 
 * buffer no matter what state the wireless chip "cc2420" is. 
 * 
 * @attention
 * you may lost the input data when you cannot call this function fast enough.
 *
 * @attention 20061013
 * the master program must guarteen the buffer has enough space to hold the whole 
 * frame or else some data may lost!!! the buffer capacity is indicated by 
 * parameter "size".
 *****************************************************************************/ 
int8 cc2420_rawread( TCc2420 * cc, char * frame, uint8 capacity, uint8 opt )
{
	uint8 count;
	
	if (cc->rxlen > 0)
	{
		IRQDisable();

		// the additional three bytes are for "length" byte itself and two "footer" bytes.
		count = cc->rxbuf.length + 3;
		assert( (count >= BASIC_RF_PACKET_OVERHEAD_SIZE) && (count <= capacity) );

		/*
		memmove( frame+0, (char*)(&cc->rxbuf.length), 1 );
		memmove( frame+1, (char*)(&cc->rxbuf.control), 2 );
		memmove( frame+3, (char*)(&cc->rxbuf.seqid), 1 );	
		memmove( frame+4, (char*)(&cc->rxbuf.panid), 2 );
		memmove( frame+6, (char*)(&cc->rxbuf.nodeto), 2 );	
		memmove( frame+8, (char*)(&cc->rxbuf.nodefrom), 2 );

		// copy the main body of the frame into the parameter buffer, including the last 
		// two bytes for footer.
		//
		//memmove( frame+10, cc->rxbuf.payload, cc->receivepayload_len);
		memmove( frame+10, (char*)(cc->rxbuf.payload), count - BASIC_RF_PACKET_OVERHEAD_SIZE );
		//memmove( frame+9 + cc->receivepayload_len, &cc->rxbuf.footer,2 );
		cc->rxlen = 0;
		*/
		
		memmove( frame, (char*)(&cc->rxbuf), count );
		cc->rxlen = 0;
		IRQEnable();			
	}
	else
		count = 0;
		
	return (int8)(count & 0x7F);
}

int8 cc2420_read( TCc2420 * cc,TCc2420Frame * frame, uint8 opt)
{
	uint8 count;
	
	if (cc->rxlen > 0)
	{
		IRQDisable();
		// increase count by 3 because the additional "length"(1B) and "footer"(2B)
		// in the TCc2420Frame structure.
	    count = cc->rxbuf.length + 3;
	    memmove( (char*)frame, (char*)(&cc->rxbuf), count );
		cc->rxlen = 0;
		IRQEnable();	
	}
	else
		count = 0;
		
	return (int8)(count & 0x7F);
}

//应该是bool型的，当ackrequest要求时， 1代表发送成功， 0 代表没受到ack，发送不成功。
//而如果要得到发送了多少长度，这个长度保存在cc中，通过修改cc->sendpacket_len来控制发送长度
/*
@TODO
注意函数原型不可以改变
*/
/* send a frame out. this function will start the sending process immediately.
 * @param
 *  cc       an pointer to the TCc2420 object
 *  frame  an frame to be sent in the memory. it's a standard TCc2420Frame format. so the first byte in the buffer
 *              is essentially the length byte.
 *  len       the data length in the buffer. 
 *  ackrequest
 *
 * @return
 * 	>0 	    how many byte sent
 *	=0   	no byte sent
 *	-1		failed sending. for example, not got ACK when ACK required.
 *			generally speaking, this function rarely return -1.
 */
bool _hardware_sendframe( TCc2420 * cc, char * framex, uint8 len, bool ackrequest ) 
{
    uint16 framecontrol;
    uint8 framelength;
    bool success;
    BYTE spiStatusByte;

	// @modified by zhangwei on 20070628
	// what's its functionality?
	cc2420_receive_off( cc );
		
    // measure the timing using following source code.
    // for test only. recommend keeping in the source code.
    // uint8 a[4];
    // a[0] = 0x17;a[1] = 0x50;a[2] = 0x89;
    // while(1) FAST2420_WRITE_FIFO(cc->spi,a, 3);      
     
    // wait until the transceiver is idle
    while (VALUE_OF_FIFOP() || VALUE_OF_SFD());

    // flush the TX FIFO just in case...
    // @TODO: shall we send two SFFLUSHTX or just one here?
    FAST2420_STROBE( cc->spi, CC2420_SFLUSHTX );

    // turn on RX if necessary
    if (!cc->receiveOn) 
    {
    	FAST2420_STROBE( cc->spi,CC2420_SRXON );
    }
    
    // @TODO: why shall we wait for the RSSI?
    // wait for the RSSI value to become valid
    do{
        FAST2420_UPD_STATUS( cc->spi,&spiStatusByte );
    }while (!(spiStatusByte & BM(CC2420_RSSI_VALID)));
    // @TODO: i think we should wait here for the 2420's SRXON OK

    // @TODO: why comment the following? is hal_delay(1) enough?
    // TX begins after the CCA check has passed
    // do{
	//	FASTSPI_STROBE( CC2420_STXONCCA );
	//	FASTSPI_UPD_STATUS( spiStatusByte );
	//	hal_delay(1);
    // }while (!(spiStatusByte & BM(CC2420_TX_ACTIVE)));

    // write the packet to the TX FIFO (the FCS is appended automatically 
    // when AUTOCRC is enabled)
  
    //framelength = pRTI->length + BASIC_RF_PACKET_OVERHEAD_SIZE;
    //framelength = cc->sendpayload_len + BASIC_RF_PACKET_OVERHEAD_SIZE;

	// the framelength is actually the 802.15.4 MAC frame length. it's the data
	// to be handed to hardware transceiver.
	//
    framelength = cc->txbuf.length;
	assert( framelength >  BASIC_RF_PACKET_OVERHEAD_SIZE );
    
    // @TODO
    framecontrol = ackrequest ? BASIC_RF_FCF_ACK : BASIC_RF_FCF_NOACK;

	// @TODO: you can arrage these data in the buffer and send them one time 
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&framelength, 1);               // Packet length
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&framecontrol, 2);         // Frame control field
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->seqid, 1);    // Sequence number
    
	cc->txbuf.panid = cc->panid;
	cc->txbuf.nodefrom = cc->address;

    // @TODO: or use cc->panid directly? i think this is better
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->txbuf.panid, 2);
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->txbuf.nodeto, 2);
    
    // @TODO: or use cc->nodefrom directly? i think this is better
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->txbuf.nodefrom, 2);         // Source address
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->txbuf.payload, framelength - BASIC_RF_PACKET_OVERHEAD_SIZE);  // Payload

	// wait for the transmission to begin before exiting (makes sure that this 
	// function cannot be called a second time, and thereby cancelling the first 
	// transmission (observe the FIFOP + SFD test above).

	// the following code are for testing only. it read out the data just wroten
	// to the TX FIFO. you can check whether we write FIFO successfully in this
	// way.
   	// FASTSPI_READ_RAM_LE( returnram,CC2420RAM_TXFIFO,20 ); 
   	// returnram[0] ++;
   	// returnram[1] ++;
   	// returnram[2] ++;
   	// returnram[3] ++;                          
   
   	// FASTSPI_STROBE(CC2420_SFLUSHTX);
   	// hal_delay(1000);
   	// FASTSPI_STROBE(CC2420_SFLUSHTX);
   	// hal_delay(1000);
   	// FASTSPI_READ_RAM_LE(returnram,CC2420RAM_TXFIFO,20); 
   	// returnram[0] ++;
   	// returnram[1] ++;
   	// returnram[2] ++;
   	// returnram[3] ++;  //测试flush后ram的值变为多少
   
	// FASTSPI_WRITE_FIFO(returnram,200);
   	// FASTSPI_UPD_STATUS(&spiStatusByte); //测试是否溢出
     
    // start sending by sending a STXON command
	FAST2420_STROBE(cc->spi,CC2420_STXON);
        
	while (!VALUE_OF_SFD()) 
		NULL;
	
	// wait for acknowledgement(ACK) if necessary
	// @TODO: you'd better judge this by checking control byte in the frame
    if (ackrequest) 
    {
    	// wait for ACK frame. the interrupt will update the value of "cc->ack_response".
    	// @attention: you should wait long enough to make sure you won't miss
    	// the ACK frame.
    	//
		cc->ack_response = FALSE;

		// wait for the SFD to go low again. this indicate the sending done.
		while (VALUE_OF_SFD()) NULL;

        // we'll enter RX automatically, so just wait until we can be sure that 
        // the ack reception should have finished. The timeout consists of a 
        // 12-symbol turnaround time, the ack packet duration, and a small margin
        hal_delay((12 * BASIC_RF_SYMBOL_DURATION) + (BASIC_RF_ACK_DURATION) + (2 * BASIC_RF_SYMBOL_DURATION) + 100);

		// if an acknowledgment has been received (indicated by the FIFOP 
		// interrupt), the ack_response flag should be set. attention that the 
		// interrupt handler will update this flag. it must be "volatile".
		success = cc->ack_response;
    } 
    else{
        success = TRUE;
    }

	// turn off the receiver if it should not continue to be enabled
	if (!cc->receiveOn)
	{ 
		FAST2420_STROBE(cc->spi, CC2420_SRFOFF);
	}

	// @attention
	// if what you sent is a data or command frame (except ACK/NAK) frame
	// you should increase the sequence id counter no matter the above process 
	// success or failed. 
	// 
	// so if you want to send ACK/NAK frame manually, you will encounter "seqid"
	// error. 
	//
    cc->seqid++;
	cc->txlen = 0;

	// @modified by zhangwei on 20070628
	// what's its functionality?
	cc2420_receive_on( cc );

    return success;
}

/* try to recv a frame from cc2420 driver
 * @return 
 * 	>0 		how many bytes received in the buffer(= should frame length)
 *	=0		no byte received
 *  -1      failed receiving. (error occured)
 *			generally speaking, this function rarely return -1.
 */
//此函数无需返回接收到的长度，而应该返回一个包。长度存在于cc->receive_len中，cc->receive_len在接收中断中赋值
/*TCc2420Frame* _hardware_recvframe(TCc2420 * cc,TCc2420Frame *pRRI) 
{
    cc->rxbuf = *pRRI;
    //cc->rssi        = cc->rfSettings.rssi;
    //cc->receivepayload_len = cc->rfSettings.payload_length;
    //cc->receivepacket_len = cc->rfSettings.payload_length + 11;
    //cc->rxlen = cc->rx_payload_len + 11; // @TODO
   
    // continue using the (one and only) reception structure
    return pRRI;
} 
*/
/* evolve the state machine of "TCc2420".
 * This evoluation function will be called when the you try to switch the current 
 * state to a new one and when you stay in some long time states. There are four
 * state in the CC driver now:
 * 
 * 	SENDING, RECVING, SLEEP and POWERDOWN
 * 
 * The driver will firstly in POWERDOWN state when the system powered on.
 * 
 * @attention
 * 	attention that the switch function almost does NONE valid checkings when you 
 * switch the state! This may cause problems in some cases! for example, you 
 * switch the wireless module into sleep() state while there are still some data
 * in the buffer to be sent! This will cause data loss! You master code should 
 * responsible for this fault!
 * 
 * @attention
 * 	the SLEEP state may not be used in some applications! these applications 
 * often require low transmission latency and you'd better turn on the transceiver
 * all the time! 
 */
int8 cc2420_evolve( TCc2420 * cc )
{
	/* @modified by zhangwei on 20061013
	 * @TODO
	 * 黄欢请按照如下逻辑实现
	 */

	BOOL done = TRUE;
	
	do{
		switch (cc->state)
		{
		case CC_STATE_IDLE:
			/* 请补充
			if (driver内部有数据要发送)
				发送之 spi_write
				
			else if cc->nextstate == CC_STATE_SLEEP
				cc2420 sleep
				
			else 
				尝试接收数据。新收到的frame可以覆盖以前老的frame: spi_read
			*/
			break;
		
		case CC_STATE_SLEEP:
			if (cc->nextstate != CC_STATE_SLEEP)
			{
				// wakeup cc2420
				cc->state = cc->nextstate;
				done = FALSE;
			}	
			break;
		
		case CC_STATE_POWERDOWN:
			if (cc->nextstate != CC_STATE_POWERDOWN)
			{
				// wakeup cc2420
				// may need do calibrate
				cc->state = cc->nextstate;
				done = false;
			}
			break;
			
		default:
			cc->nextstate = CC_STATE_IDLE;
			break;
		}
	}while (!done);
	
	return 0;
}
 
/* @attention: 
 * the valid channel value varies from 11 to 26.
 * while, the frequvency f = 2405 + 5*(channel - 11) MHz
 */
void cc2420_setchannel( TCc2420 * cc, uint8 channel )
{
	uint16 f;

	cc->channel = channel;
	assert((channel >= 11) && (channel <=26));

	// derive frequency to be programmed from the given channel number
	f = (uint16) (channel - 11); // subtract the base channel 
	f = f + (f << 2);    		 // multiply with 5, which is the channel spacing
	f = f + 357 + 0x4000;		 // 357 is 2405-2048, 0x4000 is LOCK_THR = 1
	
	FAST2420_SETREG(cc->spi,CC2420_FSCTRL, f);
}

void cc2420_interrupt_init()
{              		
	#ifdef CONFIG_TARGET_OPENNODE_10
	EXTMODE        = 0x08;              
	EXTPOLAR       = 0x08;                          //EINT3中断为上升沿触发  
	VICIntEnClr    = ~(1 << 17);                    // 使能IRQ中断	          	
	VICIntSelect   = 0x00000000;		            // 设置所有中断分配为IRQ中断
	VICVectCntl0   = 0x20 | 17;		                // 分配外部中断3到向量中断0
	VICVectAddr0   = (uint32)cc2420_interrupt_service;	// 设置中断服务程序地址
	VICIntEnable   = 1 << 17;		                // 使能EINT3中断
	EXTINT         = 0x08;			                // 清除EINT3中断标志 
	#endif
	
	#ifdef CONFIG_TARGET_OPENNODE_20
	EXTMODE        = 0x04;              
	EXTPOLAR       = 0x04;                          //EINT2中断为上升沿触发     
	VICIntEnClr    = ~(1 << 16);                    // 使能IRQ中断	       	
	VICIntSelect   = 0x00000000;		            // 设置所有中断分配为IRQ中断
	VICVectCntl0   = 0x20 | 16;		                // 分配外部中断2到向量中断0
	VICVectAddr0   = (uint32)cc2420_interrupt_service;	// 设置中断服务程序地址
	VICIntEnable   = 1 << 16;		                // 使能EINT2中断
	EXTINT         = 0x04;			                // 清除EINT2中断标志 
	#endif	
	
	#ifdef CONFIG_TARGET_OPENNODE_30
	EXTMODE        = 0x04;              
	EXTPOLAR       = 0x04;                          //EINT2中断为上升沿触发     
	VICIntEnClr    = ~(1 << 16);                    // 使能IRQ中断	       	
	VICIntSelect   = 0x00000000;		            // 设置所有中断分配为IRQ中断
	VICVectCntl0   = 0x20 | 16;		                // 分配外部中断2到向量中断0
	VICVectAddr0   = (uint32)cc2420_interrupt_service;	// 设置中断服务程序地址
	VICIntEnable   = 1 << 16;		                // 使能EINT2中断
	EXTINT         = 0x04;			                // 清除EINT2中断标志 
	#endif	
	
	#ifdef CONFIG_TARGET_WLSMODEM_11
	EXTMODE        = 0x04;              
	EXTPOLAR       = 0x04;                          //EINT2中断为上升沿触发     
	VICIntEnClr    = ~(1 << 16);                    // 使能IRQ中断	       	
	VICIntSelect   = 0x00000000;		            // 设置所有中断分配为IRQ中断
	VICVectCntl0   = 0x20 | 16;		                // 分配外部中断2到向量中断0
	VICVectAddr0   = (uint32)cc2420_interrupt_service;	// 设置中断服务程序地址
	VICIntEnable   = 1 << 16;		                // 使能EINT2中断
	EXTINT         = 0x04;			                // 清除EINT2中断标志 
	#endif

	#ifdef CONFIG_TARGET_DEFAULT
	EXTMODE        = 0x04;              
	EXTPOLAR       = 0x04;                          //EINT2中断为上升沿触发     
	VICIntEnClr    = ~(1 << 16);                    // 使能IRQ中断	       	
	VICIntSelect   = 0x00000000;		            // 设置所有中断分配为IRQ中断
	VICVectCntl0   = 0x20 | 16;		                // 分配外部中断2到向量中断0
	VICVectAddr0   = (uint32)cc2420_interrupt_service;	// 设置中断服务程序地址
	VICIntEnable   = 1 << 16;		                // 使能EINT2中断
	EXTINT         = 0x04;			                // 清除EINT2中断标志 
	#endif	
}

/* the interrupt handler is responsible to read data from cc2420 to driver's
 * internal buffer or write the internal buffer's data to cc2420 transceiver.
 * 
 * the handler also responses for changing the state of the driver.
 */

void __irq cc2420_interrupt_service( void )
{
	cc2420_interrupt_handler( g_cc2420 );

	#ifdef CONFIG_TARGET_OPENNODE_10
	EXTINT = 0x08;		
	#endif
	
	#ifdef CONFIG_TARGET_OPENNODE_20
	EXTINT = 0x04;	
	#endif
	
	#ifdef CONFIG_TARGET_OPENNODE_30
	EXTINT = 0x04;	
	#endif
	
	#ifdef CONFIG_TARGET_WLSMODEM_11
	EXTINT = 0x04;
	#endif
	
	#ifdef CONFIG_TARGET_DEFAULT
	EXTINT = 0x04;	
	#endif
	
	VICVectAddr   = 0;	// 向量中断结束
}

/* this function is called when cc2420 raised an interrupt request.
 * 
 * @parameter
 * 	cc			indicate which "TCc2420" should respond to the interrupt.
 * 				so that this handler function can be shared by serveral TCc2420
 * 				object.
 */
void cc2420_interrupt_handler( TCc2420 * cc )
{
	WORD framecontrol;
	UINT8 length;
	BYTE footer[2];
	uint8 ack;
	static rx_seqid = 0;

	#ifdef GDEBUG
	//led_toggle(LED_RED);
	#endif
      
    // @modified by zhangwei on 20070601  
    // @attention: if the last frame received hasn't read out by the master module,
    // then the interrupt will simply exit and the new comming frame will be lost!!!
    // so you should keep polling the frame out as fast as possible.
    //
	if (cc->rxlen > 0)
	{
		//return;
	}
	
    // clean up and exit in case of FIFO overflow, which is indicated by FIFOP = 1 
    // and FIFO = 0
    //
	if ((VALUE_OF_FIFOP()) && (!(VALUE_OF_FIFO()))) 
	{	   
	    FAST2420_STROBE(cc->spi,CC2420_SFLUSHRX);
	    FAST2420_STROBE(cc->spi,CC2420_SFLUSHRX);
	    return;
	}

	// get the length(the first byte) from cc2420 
	// attention you'd better to ignore the MSB to confirm it is a valid length byte.
	// the maximum length is only 127.
	//
	FAST2420_READ_FIFO_BYTE(cc->spi, &length);
	length &= 0x7F; 

    // ignore the packet if the length is too short
    // otherwise, if the length is valid, then proceed with the rest of the packet
    if (length < BASIC_RF_ACK_PACKET_SIZE) 
    {
    	FAST2420_READ_FIFO_GARBAGE(cc->spi,length);
    } 
    else{
        // register the payload length
        // cc->rfSettings.payload_length = length - BASIC_RF_PACKET_OVERHEAD_SIZE;
        
        // read the frame control field
        FAST2420_READ_FIFO_NO_WAIT(cc->spi,(BYTE*) &framecontrol, 2);
        cc->rxbuf.control = framecontrol;
        ack = !!(framecontrol & BASIC_RF_FCF_ACK_BM); // @TODO is it right? why double !!?
        
        // read the sequence num in the frame received
        // @TODO: why not FAST2420_READ_FIFO_NO_WAIT? what's the difference?
    	FAST2420_READ_FIFO_BYTE(cc->spi,(BYTE*)&rx_seqid);
    	cc->rxbuf.seqid = rx_seqid;
    	
		// if this is an acknowledgment packet, compare the sequence id received 
		// and saved after last sending
    	if ((length == BASIC_RF_ACK_PACKET_SIZE) && (framecontrol == BASIC_RF_ACK_FCF) 
    		&& (rx_seqid == cc->seqid)) 
    	{
 	       	// read the footer and check for CRC OK
			FAST2420_READ_FIFO_NO_WAIT(cc->spi,(BYTE*) footer, 2);
			// indicate the successful ack reception (this flag is polled by the 
			// _hardware_send() routine)
			if (footer[1] & BASIC_RF_CRC_OK_BM)
			{ 
				cc->ack_response = TRUE;
			}				
			
			cc->rxbuf.length = length; // @modified by zhangwei on 20070610. added this new line
			cc->rxlen = length+3; 

			// @TODO
			// if the TCc2420 object running in sniffer mode, you should update 
			// cc->rxlen to indicate a new frame received. now the frame is ACK/NAK
			// frame. or else you can simply leave cc->rxlen unchanged. 
			//
			//if mode == SNIFFER
				//cc->rxlen = length;
			//	NULL;
		} 
		// if the frame is too small to be a valid packet, the simply discard it
		else if (length < BASIC_RF_PACKET_OVERHEAD_SIZE) 
		{
			FAST2420_READ_FIFO_GARBAGE(cc->spi, length - 3); 
			return;
		}
		// generally, a valid data frame now
		// @TODO: it seems the NAK frame also goes into this branch. is it?
		else{
			// skip the destination PAN and address (that's taken care of by harware address recognition!)
			// FAST2420_READ_FIFO_GARBAGE(cc->spi,4);
			
			// read the PanID
			FAST2420_READ_FIFO_NO_WAIT(cc->spi,(BYTE*) &cc->rxbuf.panid, 2);
			
			// read the destination address(local address)
			FAST2420_READ_FIFO_NO_WAIT(cc->spi,(BYTE*) &cc->rxbuf.nodeto, 2);

			// read the source address
			FAST2420_READ_FIFO_NO_WAIT(cc->spi,(BYTE*) &cc->rxbuf.nodefrom, 2);

			// read the packet payload
			//FAST2420_READ_FIFO_NO_WAIT(cc->spi,(BYTE*) cc->rxbuf.payload, length - BASIC_RF_PACKET_OVERHEAD_SIZE);
			FAST2420_READ_FIFO_NO_WAIT(cc->spi,(BYTE*) cc->rxbuf.payload, length - 11);

			// read the footer to get the RSSI value
			FAST2420_READ_FIFO_NO_WAIT(cc->spi,(BYTE*) footer, 2);
			cc->rxbuf.footer = (footer[1] << 8) + footer[0];
			cc->rssi = footer[0];

			// notify the application about the received _data_ packet if the CRC is OK
			// @TODO needs confirm? why the following condition? is it right???
			// whether it should be || instead of &&? 
			// zhangwei thinks the following comparison is not quite good.
			if (((framecontrol & (BASIC_RF_FCF_BM)) == BASIC_RF_FCF_NOACK) 
				&& (footer[1] & BASIC_RF_CRC_OK_BM)) 
			{
				cc->rxbuf.length = length;
				 //_hardware_recvframe(cc,(TCc2420Frame *)(&cc->rxbuf));
				 //cc->pRxInfo = *(_hardware_recvframe(cc,(TCc2420Frame *)(&cc->pRxInfo)));
			}
			cc->rxbuf.length = length; // @modified by zhangwei on 20070610. added this new line
			cc->rxlen = length+3;  // @TODO?? shall we update it so early? and here?
		}
    }
}

void cc2420_receive_on(TCc2420 * cc) 
{
	cc->receiveOn = TRUE;
	FAST2420_STROBE(cc->spi,CC2420_SRXON);
	FAST2420_STROBE(cc->spi,CC2420_SFLUSHRX);
} 

void cc2420_receive_off(TCc2420 * cc) 
{
	cc->receiveOn = FALSE;
	FAST2420_STROBE(cc->spi,CC2420_SRFOFF);
} 

/* this function will Poll the SPI status byte until the crystal oscillator is stable    
 * your must wait until it is stable before doing further read() or write() 
 */
void _cc2420_waitfor_crystal_oscillator(TSpiDriver * spi) 
{
	static BYTE status;

	do{	   
		FAST2420_UPD_STATUS(spi, (uint8*)(&status) );
	}while (!(status & BM(CC2420_XOSC16M_STABLE)));
}

// set the RF power. this will affect the distance.
// refer to cc2420 datasheet page 52 value-dbm table for the setting values
//
void cc2420_set_power(TCc2420 * cc,uint8 power)
{
    uint16 power_register;
    
    switch(power)
    {
        case CC2420_POWER_1:  power_register = 0xa0ff; break;
        case CC2420_POWER_2:  power_register = 0xa0fb; break;
        case CC2420_POWER_3:  power_register = 0xa0f7; break;
        case CC2420_POWER_4:  power_register = 0xa0f3; break;
        case CC2420_POWER_5:  power_register = 0xa0ef; break;
        case CC2420_POWER_6:  power_register = 0xa0eb; break;
        case CC2420_POWER_7:  power_register = 0xa0e7; break;
        case CC2420_POWER_8:  power_register = 0xa0e3; break;
        default :             power_register = 0xa0ff; break;
    }
    
    FAST2420_SETREG(cc->spi,CC2420_TXCTRL, power_register);     
}

uint8 cc2420_rssi( TCc2420 * cc )
{
	return cc->rssi;
}

