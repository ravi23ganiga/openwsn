/*******************************************************************************
 * @author zhangwei on 2006-07-20
 * TCc2420Driver
 * This is an software abstraction of the cc2420 transceiver hardware. you can 
 * fully manipulate the cc2420 hardware through this object. 
 ******************************************************************************/
 
#include "hal_foundation.h"
#include "hal_configall.h"
#include "hal_cpu.h"
#include "hal_spi.h"
#include "hal_cc2420chip.h"
#include "hal_cc2420rf.h"
#include "hal_cc2420.h"
#include "hal_led.h"
#include "hal_uart.h"
#include "hal_global.h"

static void cc2420_interrupt_init( void );
static BOOL _cc2420_sendframe(TCc2420Driver * cc);
static TCc2420Frame * _cc2420_recvframe( TCc2420Driver * cc,TCc2420Frame *pRRI);
static void __irq cc2420_interrupt_handler( void );
//static void cc2420_event_handler( TCc2420Driver * cc );
static void cc2420_event_handler( void );

/*******************************************************************************
 * construct a TCc2420Driver object
 * usually, you should always call configure() after construct the object in the 
 * memory.
 * 
 * example: 
 * 	TCc2420Driver *cc, buf;
 * 	cc = cc2420_construct( buf, ...);
 * 	cc2420_configure( cc, CC2420_CONFIG_PANID, (void*)&panid );
 * 	cc2420_configure( cc, CC2420_CONFIG_NODEADDRESS, (void*)&nodeaddress );
 * 	cc2420_configure( cc, CC2420_CONFIG_TUNNING_POWER, (void*)&power );
 * 	cc2420_configure( cc, CC2420_CONFIG_CHANNEL, (void*)&channel );
 *  cc2420_configure( cc, CC2420_CONFIG_APPLY, NULL );
 * 
 ******************************************************************************/
TCc2420Driver * cc2420_construct( char * buf, uint16 size, TSpiDriver * spi )
{
	               
	TCc2420Driver *cc;
	char* out_string = "cc2420 construct succesful!\n";
	
	//led_on(LED_GREEN);
	
	
	if (sizeof(TCc2420Driver) > size)
		cc = NULL;
	else
		cc = (TCc2420Driver *)buf;
		
	
	if (cc != NULL)
	{	
		
		memset( (char*)cc, 0x00, sizeof(TCc2420Driver) );
		cc->state = CC_STATE_POWERDOWN;
		cc->nextstate = CC_STATE_POWERDOWN;
		cc->spi = spi;
		cc->panid = 1; //PANID;
		cc->address = 1; //ADDR;
		cc->channel = 1; //Channel;
		cc->txlen = 0;
		cc->rxlen = 0;
		cc->power = 1;
		cc->sleeprequest = FALSE;
		uart_write(g_uart, out_string,28, 0);
	}
        
	return cc;
}

void cc2420_destroy( TCc2420Driver * cc )
{
	if (cc != NULL)
	{
        cc2420_close( cc );
		cc->txlen = 0;
		cc->rxlen = 0;
	}
}

void cc2420_configure( TCc2420Driver * cc, uint8 ctrlcode, uint16 value, uint8 size )
{
	
	switch (ctrlcode)
	{
	case CC2420_BASIC_INIT:
	        cc->state = CC_STATE_POWERDOWN;
		cc->nextstate = CC_STATE_POWERDOWN;
		cc->panid = 0x2420; //PANID;
		cc->address = 0x1234; //ADDR;
		cc->channel = 19; //Channel;
		cc->txlen = 0;
		cc->rxlen = 0;
		cc->power = 1;
		cc->ackrequest = 0;
		cc->sleeprequest = FALSE;
		cc->if_read = 1;
	        cc2420_init(cc); 
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
		cc2420_setchannel( cc );
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
			// disable ACK 
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

//----------------------------------------------------------------------------
//  void basicRfInit(BASIC_RF_RX_INFO *pRRI, UINT8 channel, WORD panId, WORD myAddr)
//
//  DESCRIPTION:
//      Initializes CC2420 for radio communication via the basic RF library functions. Turns on the
//		voltage regulator, resets the CC2420, turns on the crystal oscillator, writes all necessary
//		registers and protocol addresses (for automatic address recognition). Note that the crystal
//		oscillator will remain on (forever).
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
void cc2420_init(TCc2420Driver * cc) 
{
    uint16 rereg;

    // Make sure that the voltage regulator is on, and that the reset pin is inactive
    SET_VREG_ACTIVE();
    halWait(1000);
     
    SET_RESET_ACTIVE();
    halWait(1000);
    
    SET_RESET_INACTIVE();
    halWait(500);
    FAST2420_STROBE(cc->spi,CC2420_SXOSCON);
    halWait(1000);
    
    //FASTSPI_SETREG(CC2420_TXCTRL, 0xA0E3); // To control the output power, added by huanghuan
    FAST2420_SETREG(cc->spi,CC2420_MDMCTRL0, 0x0AF2); // Turn on automatic packet acknowledgment 
    FAST2420_SETREG(cc->spi,CC2420_MDMCTRL1, 0x0500); // Set the correlation threshold = 20
    FAST2420_SETREG(cc->spi,CC2420_IOCFG0, 0x007F);   // Set the FIFOP threshold to maximum
    FAST2420_SETREG(cc->spi,CC2420_SECCTRL0, 0x01C4); // Turn off "Security enable"
    
    // Set the RF channel
    cc2420_setchannel(cc);
    
    led_twinkle(LED_RED,1);
 
    

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
    
	// Set the protocol configuration
    //g_rfSettings.pRxInfo = pRRI;
    cc->rfSettings.panid = cc->panid;
    cc->rfSettings.myAddr = cc->address;
    cc->rfSettings.seqid = 0;
    cc->rfSettings.receiveOn = FALSE;
    cc->rfSettings.ackReceived = FALSE;
    cc->rfSettings.payload_length = 0;
    
    //led_twinkle(LED_GREEN,1);
    
    //FAST2420_UPD_STATUS(cc->spi, (uint8*)(&rereg) );
    //uart_putchar(g_uart,(char)rereg);
	// Wait for the crystal oscillator to become stable
    cc2420_waitfor_crystal_oscillator(cc->spi);
	// Write the short address and the PAN ID to the CC2420 RAM (requires that the XOSC is on and stable)
    led_twinkle(LED_GREEN,5);
  
    FAST2420_WRITE_RAM_LE(cc->spi,&(cc->address), CC2420RAM_SHORTADDR, 2);
    FAST2420_WRITE_RAM_LE(cc->spi,&(cc->panid), CC2420RAM_PANID, 2);
    
    //FAST2420_READ_RAM_LE(cc,reram,CC2420RAM_SHORTADDR,2);
    //reram[0]++;
    //reram[1]++; 
}

void cc2420_open( TCc2420Driver * cc )
{
}

void cc2420_close( TCc2420Driver * cc ) 
{
}

uint8 cc2420_state( TCc2420Driver * cc )
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
uint8 cc2420_ioresult( TCc2420Driver * cc )
{
	uint8 ioresult = 0x00;
	
	if (cc->txlen > 0)
		ioresult |= 0x01;
	if (cc->rxlen > 0)
		ioresult |= 0x02;
		
	return ioresult;	
}

//frame = (2 + 1 + 2 + 2 + 2) + n + (2)
//framecontrol = 0;无需填写，硬件自动填完；
//txsequence   = 0;无需填写；
//PAN ID(2B), destination and source(4B)
//n payload
//footer = 0;无需填写；

int8 cc2420_rawwrite( TCc2420Driver * cc, char * frame, int8 length, uint8 opt )
{
	BOOL ack;
	int  len;
	
	len = length;
	
	memmove(&cc->txbuffer[0].panid, frame + 4,2 );
	memmove(&cc->txbuffer[0].nodeto, frame + 6,2 );
	memmove(&cc->txbuffer[0].nodefrom, frame + 8,2 );
	memmove(&cc->txbuffer[0].payload, frame + 10, length - 11 );     
	
	if((opt & 0x01))  
	cc->ackrequest = 0;            

        
        
    cc->sendpayload_len = length - 11;
    if(cc->ackrequest ==1) 
	{
	    ack = _cc2420_sendframe(cc);
	    if(!ack) 
	       len = -1;
        }
        else 
        {
            _cc2420_sendframe(cc);
        }

	cc->nextstate = CC_STATE_IDLE;
	//cc2420_evolve( cc );
	
	return len;
}


int8 cc2420_write( TCc2420Driver * cc,TCc2420Frame frame, int8 length,uint8 opt)
{
	BOOL ack;
	int8 len;
	
	len = length;
	
	cc->txbuffer[0] = frame;
	
	cc->sendpayload_len = length - 11;
	
	if(cc->ackrequest == 1) 
	{
	    ack = _cc2420_sendframe(cc);
	    if(!ack) len = -1;
        }
        else 
        {
            _cc2420_sendframe(cc);
        }
        
	cc->nextstate = CC_STATE_IDLE;
	//cc2420_evolve( cc );
	
	return len;	
}

/* return the received frame entirely to the frame buffer. 
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
 */
uint8 cc2420_rawread( TCc2420Driver * cc, char * frame, uint8 size, uint8 opt )
{
		
	if(cc->if_read) return 0;
        
	IRQDisable();
	frame[0] = cc->receivepayload_len + 11;	
	memmove( frame+1, &cc->rxbuffer[0].control,2 );
	memmove( frame+3, &cc->rxbuffer[0].seqid,1 );	
	memmove( frame+4, &cc->rxbuffer[0].panid,2 );
	memmove( frame+6, &cc->rxbuffer[0].nodeto,2 );	
	memmove( frame+8, &cc->rxbuffer[0].nodefrom,2 );
	memmove( frame+10, cc->rxbuffer[0].payload,cc->receivepayload_len);
	//memmove( frame+9 + cc->receivepayload_len, &cc->rxbuffer[0].footer,2 );
	cc->if_read = 1;
	IRQEnable();			
		
	//cc->nextstate = CC_STATE_IDLE;
	//cc2420_evolve( cc );
	
	return cc->receivepayload_len + 11;	
}

uint8 cc2420_read( TCc2420Driver * cc,TCc2420Frame * frame,uint8 len, uint8 opt)
{
	
	if(cc->if_read) return 0;
        
	IRQDisable();
	*frame = cc->rxbuffer[0];
	cc->if_read = 1;
	IRQEnable();			
        
	cc->nextstate = CC_STATE_IDLE;
	//cc2420_evolve( cc );
	
	return cc->receivepayload_len + 11;	
}




/* evolve the state machine of "TCc2420Driver".
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

int8 cc2420_evolve( TCc2420Driver * cc )
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
 
/*
void cc2420_startup( TCc2420Driver * cc )
{
	// @TODO
	//spi_write
	assert( cc->state == CC_STATE_POWERDOWN );
	cc->state = CC_STATE_IDLE;
}

void cc2420_shutdown( TCc2420Driver * cc )
{
	// @TODO
	//spi_write
	cc->state = CC_STATE_POWERDOWN;
}
*/

void cc2420_sleep( TCc2420Driver * cc )
{
	// @TODO
	//spi_write
	cc->state = CC_STATE_SLEEP;
}

// assume the current state is SLEEP state
void cc2420_wakeup( TCc2420Driver * cc )
{
	// @TODO
	//spi_write
	cc->state = CC_STATE_IDLE;
}

void cc2420_setchannel( TCc2420Driver * cc)
{
	uint16 f;
	// Derive frequency programming from the given channel number
	f = (uint16) (cc->channel - 11); // Subtract the base channel 
	f = f + (f << 2);    		 // Multiply with 5, which is the channel spacing
	f = f + 357 + 0x4000;		 // 357 is 2405-2048, 0x4000 is LOCK_THR = 1
	
	FAST2420_SETREG(cc->spi,CC2420_FSCTRL, f);
}

void cc2420_interrupt_init()
{              		
	#ifdef CONFIG_TARGET_OPENNODE_10
	EXTMODE        = 0x08;              
	EXTPOLAR       = 0x08;                                  //EINT3中断为上升沿触发  
	VICIntEnClr    = ~(1 << 17);                            // 使能IRQ中断	          	
	VICIntSelect   = 0x00000000;		                // 设置所有中断分配为IRQ中断
	VICVectCntl0   = 0x20 | 17;		                // 分配外部中断3到向量中断0
	VICVectAddr0   = (uint32)cc2420_interrupt_handler;	// 设置中断服务程序地址
	VICIntEnable   = 1 << 17;		                // 使能EINT3中断
	EXTINT         = 0x08;			                // 清除EINT3中断标志 
	#endif
	
	#ifdef CONFIG_TARGET_OPENNODE_20
	EXTMODE        = 0x04;              
	EXTPOLAR       = 0x04;                                  //EINT2中断为上升沿触发     
	VICIntEnClr    = ~(1 << 16);                            // 使能IRQ中断	       	
	VICIntSelect   = 0x00000000;		                // 设置所有中断分配为IRQ中断
	VICVectCntl0   = 0x20 | 16;		                // 分配外部中断2到向量中断0
	VICVectAddr0   = (uint32)cc2420_interrupt_handler;	// 设置中断服务程序地址
	VICIntEnable   = 1 << 16;		                // 使能EINT2中断
	EXTINT         = 0x04;			                // 清除EINT2中断标志 
	#endif	
	
	#ifdef CONFIG_TARGET_WLSMODEM_11
	EXTMODE        = 0x04;              
	EXTPOLAR       = 0x04;                                  //EINT2中断为上升沿触发     
	VICIntEnClr    = ~(1 << 16);                            // 使能IRQ中断	       	
	VICIntSelect   = 0x00000000;		                // 设置所有中断分配为IRQ中断
	VICVectCntl0   = 0x20 | 16;		                // 分配外部中断2到向量中断0
	VICVectAddr0   = (uint32)cc2420_interrupt_handler;	// 设置中断服务程序地址
	VICIntEnable   = 1 << 16;		                // 使能EINT2中断
	EXTINT         = 0x04;			                // 清除EINT2中断标志 
	#endif
}


/* the interrupt handler is responsible to read data from cc2420 to driver's
 * internal buffer or write the internal buffer's data to cc2420 transceiver.
 * 
 * the handler also responses for changing the state of the driver.
 */

void __irq cc2420_interrupt_handler( void )
{
	cc2420_event_handler();

        #ifdef CONFIG_TARGET_OPENNODE_10
	EXTINT = 0x08;		
	#endif
	
	#ifdef CONFIG_TARGET_OPENNODE_20
	EXTINT = 0x04;	
	#endif
	
	#ifdef CONFIG_TARGET_WLSMODEM_11
	EXTINT = 0x04;
	#endif
	
	VICVectAddr   = 0;	// 向量中断结束
}




/* this function is called when cc2420 raised an interrupt request 
 * you can change this function according to your wish. 
 * 
 * @parameter
 * 	cc			indicate which "TCc2420Driver" should respond to the interrupt.
 */
 
// @modified by zhangwei on 20061109
// 我还是要求如下函数原型
// void cc2420_event_handler( TCc2420Driver * cc ) 
//
// 应最大限度减少对全局变量维护。难道这有问题吗？
// 也不会降低性能的。

void cc2420_event_handler()
{
	WORD frameControlField;
	UINT8 length;
	BYTE pFooter[2];
	uint8 ack;
	
	//halWait(2000);
        
        led_toggle(LED_RED);
        
        
        
    // Clean up and exit in case of FIFO overflow, which is indicated by FIFOP = 1 and FIFO = 0
	if((VALUE_OF_FIFOP()) && (!(VALUE_OF_FIFO()))) {	   
	    FAST2420_STROBE(g_cc2420->spi,CC2420_SFLUSHRX);
	    FAST2420_STROBE(g_cc2420->spi,CC2420_SFLUSHRX);
	    return;
	}

	// Payload length
	FAST2420_READ_FIFO_BYTE(g_cc2420->spi,&length);
	g_cc2420->receivepacket_len = length;
	
	length &= BASIC_RF_LENGTH_MASK; // Ignore MSB

    // Ignore the packet if the length is too short
    if (length < BASIC_RF_ACK_PACKET_SIZE) {
    	FAST2420_READ_FIFO_GARBAGE(g_cc2420->spi,length);
        
    // Otherwise, if the length is valid, then proceed with the rest of the packet
    } else {
        // Register the payload length
        g_cc2420->rfSettings.payload_length = length - BASIC_RF_PACKET_OVERHEAD_SIZE;
        // Read the frame control field and the data sequence number
        FAST2420_READ_FIFO_NO_WAIT(g_cc2420->spi,(BYTE*) &frameControlField, 2);
        g_cc2420->rfSettings.pRxInfo.control = frameControlField;
        ack = !!(frameControlField & BASIC_RF_FCF_ACK_BM);
    	FAST2420_READ_FIFO_BYTE(g_cc2420->spi,(BYTE*)&g_cc2420->rfSettings.pRxInfo.seqid);
		// Is this an acknowledgment packet?
    	if ((length == BASIC_RF_ACK_PACKET_SIZE) && (frameControlField == BASIC_RF_ACK_FCF) && (g_cc2420->rfSettings.pRxInfo.seqid == g_cc2420->rfSettings.seqid)) {

 	       	// Read the footer and check for CRC OK
			FAST2420_READ_FIFO_NO_WAIT(g_cc2420->spi,(BYTE*) pFooter, 2);

			// Indicate the successful ack reception (this flag is polled by the transmission routine)
			if (pFooter[1] & BASIC_RF_CRC_OK_BM) g_cc2420->rfSettings.ackReceived = TRUE;
		// Too small to be a valid packet?
		} else if (length < BASIC_RF_PACKET_OVERHEAD_SIZE) {
			FAST2420_READ_FIFO_GARBAGE(g_cc2420->spi,length - 3);
			return;

		// Receive the rest of the packet
	} else {
		
		       
		    
			// Skip the destination PAN and address (that's taken care of by harware address recognition!)
			//FAST2420_READ_FIFO_GARBAGE(g_cc2420->spi,4);
			
			
			//Read the PanID
			FAST2420_READ_FIFO_NO_WAIT(g_cc2420->spi,(BYTE*) &g_cc2420->rfSettings.pRxInfo.panid, 2);
			
			//Read the destination address(local address)
			FAST2420_READ_FIFO_NO_WAIT(g_cc2420->spi,(BYTE*) &g_cc2420->rfSettings.pRxInfo.nodeto, 2);

			// Read the source address
			FAST2420_READ_FIFO_NO_WAIT(g_cc2420->spi,(BYTE*) &g_cc2420->rfSettings.pRxInfo.nodefrom, 2);

			// Read the packet payload
			FAST2420_READ_FIFO_NO_WAIT(g_cc2420->spi,(BYTE*) g_cc2420->rfSettings.pRxInfo.payload, g_cc2420->rfSettings.payload_length);

			// Read the footer to get the RSSI value
			FAST2420_READ_FIFO_NO_WAIT(g_cc2420->spi,(BYTE*) pFooter, 2);
			g_cc2420->rfSettings.pRxInfo.footer = (pFooter[1] << 8) + pFooter[0];
			g_cc2420->rfSettings.rssi = pFooter[0];
			
			
			
			// Notify the application about the received _data_ packet if the CRC is OK
			if (((frameControlField & (BASIC_RF_FCF_BM)) == BASIC_RF_FCF_NOACK) && (pFooter[1] & BASIC_RF_CRC_OK_BM)) {
				 g_cc2420->rfSettings.pRxInfo = *(_cc2420_recvframe(g_cc2420,(TCc2420Frame *)(&g_cc2420->rfSettings.pRxInfo)));
			}
			
		}
    }
}


void cc2420_receive_on(TCc2420Driver * cc) 
{

        cc->rfSettings.receiveOn = TRUE;

	FAST2420_STROBE(cc->spi,CC2420_SRXON);

	FAST2420_STROBE(cc->spi,CC2420_SFLUSHRX);
} 

void cc2420_receive_off(TCc2420Driver * cc) 
{
        cc->rfSettings.receiveOn = FALSE;
	FAST2420_STROBE(cc->spi,CC2420_SRFOFF);
} 

/* this function will Poll the SPI status byte until the crystal oscillator is stable    
 * your must wait until it is stable before doing further read() or write() 
 */
void cc2420_waitfor_crystal_oscillator(TSpiDriver * spi) 
{
	static BYTE spiStatusByte;

	do {	   
		FAST2420_UPD_STATUS(spi, (uint8*)(&spiStatusByte) );
	} while (!(spiStatusByte & BM(CC2420_XOSC16M_STABLE)));
}

//应该是bool型的，当ackrequest要求时， 1代表发送成功， 0 代表没受到ack，发送不成功。
//而如果要得到发送了多少长度，这个长度保存在cc中，通过修改cc->sendpacket_len来控制发送长度
/*
@TODO
注意函数原型不可以改变
*/
/* send a frame out
 * @return
 * 	>0 	    how many byte sent
 *	=0   	no byte sent
 *	-1		failed sending. for example, not got ACK when ACK required.
 *			generally speaking, this function rarely return -1.
 */

BOOL _cc2420_sendframe(TCc2420Driver * cc) {
    WORD frameControlField;
    UINT8 packetLength;
    BOOL success;
    BYTE spiStatusByte;
    
    
    //uint8 a[4];
    
    //a[0] = 0x17;a[1] = 0x50;a[2] = 0x89;
    
    //while(1) FAST2420_WRITE_FIFO(cc->spi,a, 3);      
    
    
     
    // Wait until the transceiver is idle
    while (VALUE_OF_FIFOP() || VALUE_OF_SFD());

    // Flush the TX FIFO just in case...
    FAST2420_STROBE(cc->spi,CC2420_SFLUSHTX);

    // Turn on RX if necessary
    if (!cc->rfSettings.receiveOn) FAST2420_STROBE(cc->spi,CC2420_SRXON);

    // Wait for the RSSI value to become valid
    do {
        FAST2420_UPD_STATUS(cc->spi,&spiStatusByte);
    } while (!(spiStatusByte & BM(CC2420_RSSI_VALID)));
/*
    // TX begins after the CCA check has passed
    do {
		FASTSPI_STROBE(CC2420_STXONCCA);
		FASTSPI_UPD_STATUS(spiStatusByte);
		halWait(1);
    } while (!(spiStatusByte & BM(CC2420_TX_ACTIVE)));
*/   
    // Write the packet to the TX FIFO (the FCS is appended automatically when AUTOCRC is enabled)
  
    //packetLength = pRTI->length + BASIC_RF_PACKET_OVERHEAD_SIZE;
    packetLength = cc->sendpayload_len + BASIC_RF_PACKET_OVERHEAD_SIZE;
    frameControlField = cc->ackrequest ? BASIC_RF_FCF_ACK : BASIC_RF_FCF_NOACK;
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&packetLength, 1);               // Packet length
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&frameControlField, 2);         // Frame control field
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->rfSettings.seqid, 1);    // Sequence number
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->txbuffer[0].panid, 2);          // Dest. PAN ID
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->txbuffer[0].nodeto, 2);            // Dest. address
    FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->txbuffer[0].nodefrom, 2);         // Source address
	FAST2420_WRITE_FIFO(cc->spi,(BYTE*)&cc->txbuffer[0].payload, cc->sendpayload_len);  // Payload

	// Wait for the transmission to begin before exiting (makes sure that this function cannot be called
	// a second time, and thereby cancelling the first transmission (observe the FIFOP + SFD test above).

 
  
   /*
   //for test
   FASTSPI_READ_RAM_LE(returnram,CC2420RAM_TXFIFO,20); 
   returnram[0] ++;
   returnram[1] ++;
   returnram[2] ++;
   returnram[3] ++;                            //读出ram值，看是否写入
   
   FASTSPI_STROBE(CC2420_SFLUSHTX);
   halWait(1000);
   FASTSPI_STROBE(CC2420_SFLUSHTX);
   halWait(1000);
   FASTSPI_READ_RAM_LE(returnram,CC2420RAM_TXFIFO,20); 
   returnram[0] ++;
   returnram[1] ++;
   returnram[2] ++;
   returnram[3] ++;                            //测试flush后ram的值变为多少
   
   FASTSPI_WRITE_FIFO(returnram,200);
   FASTSPI_UPD_STATUS(&spiStatusByte);         //测试是否溢出
   //for test  
   */
   
     
	FAST2420_STROBE(cc->spi,CC2420_STXON);
        
        led_on(LED_YELLOW);
	while (!VALUE_OF_SFD());
	led_on(LED_GREEN);  
	
	
	
    // Wait for the acknowledge to be received, if any
    if (cc->ackrequest) {
		cc->rfSettings.ackReceived = FALSE;

		// Wait for the SFD to go low again
		while (VALUE_OF_SFD());

        // We'll enter RX automatically, so just wait until we can be sure that the ack reception should have finished
        // The timeout consists of a 12-symbol turnaround time, the ack packet duration, and a small margin
        halWait((12 * BASIC_RF_SYMBOL_DURATION) + (BASIC_RF_ACK_DURATION) + (2 * BASIC_RF_SYMBOL_DURATION) + 100);

		// If an acknowledgment has been received (by the FIFOP interrupt), the ackReceived flag should be set
		success = cc->rfSettings.ackReceived;
    } else {
        success= TRUE;
    }

	// Turn off the receiver if it should not continue to be enabled
   
	if (!cc->rfSettings.receiveOn) FAST2420_STROBE(cc->spi,CC2420_SRFOFF);

    // Increment the sequence number, and return the result
    cc->rfSettings.seqid++;
    return success;

} // halRfSendPacket



/* try to recv a frame from cc2420 driver
 * @return 
 * 	>0 		how many bytes received in the buffer(= should frame length)
 *	=0		no byte received
 *  -1      failed receiving. (error occured)
 *			generally speaking, this function rarely return -1.
 */
//此函数无需返回接收到的长度，而应该返回一个包。长度存在于cc->receive_len中，cc->receive_len在接收中断中赋值
TCc2420Frame* _cc2420_recvframe(TCc2420Driver * cc,TCc2420Frame *pRRI) 
{
   g_cc2420->if_read = 0;      //clear the if_read flag;
   
   cc->rxbuffer[0] = *pRRI;
   cc->if_read = 0;
   cc->rssi        = cc->rfSettings.rssi;
   cc->receivepayload_len = cc->rfSettings.payload_length;
   cc->receivepacket_len = cc->rfSettings.payload_length + 11;
   
    // Continue using the (one and only) reception structure
    return pRRI;
} 

void cc2420_set_power(TCc2420Driver * cc,uint8 power)
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
    
    FAST2420_SETREG(cc->spi,CC2420_TXCTRL, power_register);     //cc2420 datasheet page 52 value-dbm table
}