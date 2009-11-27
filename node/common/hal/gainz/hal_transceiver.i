#ifndef _CC2420_H_4892_
#define _CC2420_H_4892_
/*******************************************************************************
 * cc2420 adapter
 * simple wraper of cc2420 low power wireless transceiver. it helps to implement 
 * the MAC network layer. 
 * 
 * @state
 *	developing
 * 
 * @history
 * @author zhangwei on 200609
 * @modified by zhangwei on 20090718
 *	- revision
 * @modified by zhangwei on 20090819
 *	- add default settings of panid, local address and pan id in cc2420_open()
 * @modified by zhangwei on 20090927
 *	- revision. divide all the interface functions into serveral group according 
 *    to their functions.
 *
 ******************************************************************************/




frame_transceiver_send
frame_transceiver_recv
frame_transceiver_evolve


byte_transceiver_send
byte_transceiver_recv
byte_transceiver_evolve














#include "hal_configall.h"
#include "hal_foundation.h"
#include "hal_cc2420const.h"

/*******************************************************************************
 * Configuration
 ******************************************************************************/

/* If you want to enable the high precise time synchronization, then the macro
 * CONFIG_CC2420_SFD should be disabled. 
 * 
 * The default setting of SFD interrupt is disabled.
 * @todo: SFD interrupt management hasn't been tested yet
 */

#ifndef CONFIG_CC2420_SFD
	//#define CONFIG_CC2420_SFD
#endif

#ifndef CONFIG_DEBUG  
#define CONFIG_DEBUG
//#undef CONFIG_DEBUG
#endif

#define CC2420_DEF_PANID                0x0001
#define CC2420_DEF_LOCAL_ADDRESS        0x0001   
#define CC2420_DEF_REMOTE_ADDRESS       0x0002
#define CC2420_DEF_CHANNEL              11

/*******************************************************************************
 * Interface
 ******************************************************************************/

/* the minimal frame is the ACK frame, including only 5 bytes (2B Frame Control, 
 * 1B Sequence, and 2B checksum) 
 */
#define CC2420_MAX_FRAME_LENGTH         0x7F
#define CC2420_MIN_FRAME_LENGTH         5

/* The buffer size is 1 larger than cc2420's MAC frame size, because there needs 
 * another byte to save the length of the frame itself. Attention the length value
 * doesn't include the length byte itself. 
 */
#define CC2420_RXBUFFER_SIZE (CC2420_MAX_FRAME_LENGTH+1)
#define CC2420_TXBUFFER_SIZE (CC2420_MAX_FRAME_LENGTH+1)

/* State of TiCc2420Adapter object
 * Attention this state is different from cc2420 transceiver's internal state.
 */
#define CC2420_STATE_RECVING 0
#define CC2420_STATE_SENDING 1
#define CC2420_STATE_SLEEP   2

/* CC2420 Power level
 * POWER_1 is the lowest, while POWER_8 is the highest. The current value is measured
 * using OpenNODE hardware platform (LPC2136 ARM7 + cc2420)
 */
#define CC2420_POWER_MIN CC2420_POWER_1
#define CC2420_POWER_MAX CC2420_POWER_8
#define CC2420_POWER_DEFAULT CC2420_POWER_MIN

#define CC2420_POWER_1  0x01                     //-25dBm    8.5mA 
#define CC2420_POWER_2  0x02                     //-15dBm    8.9mA
#define CC2420_POWER_3  0x03                     //-10dBm   11.2mA
#define CC2420_POWER_4  0x04                     // -7dBm   12.5mA
#define CC2420_POWER_5  0x05                     // -5dBm   13.9mA
#define CC2420_POWER_6  0x06                     // -3dBm   15.2mA
#define CC2420_POWER_7  0x07                     // -1dBm   16.5mA
#define CC2420_POWER_8  0x08                     //  0dBm   17.4mA


/* RSSI to Energy Detection conversion
 * RSSI_OFFSET defines the RSSI level where the PLME.ED generates a zero-value 
 */
#define RSSI_OFFSET -38
#define RSSI_2_ED(rssi)   ((rssi) < RSSI_OFFSET ? 0 : ((rssi) - (RSSI_OFFSET)))
#define ED_2_LQI(ed) (((ed) > 63 ? 255 : ((ed) << 2)))


/* TiCc2420Adapter
 * is a lightweight wrapper of TI/Chipcon's cc2420 2.4G transceiver. 
 * design principle: lightweight, fast, simple, easy for portable
 *
 *	id				         cc2420 identifier. should always be 0 currently
 *	state			         state of the current TiCc2420Adapter object
 *	rssi                     received signal strength indicator
 *  lqi                      link quality indicaator
 *	//spistatus              spistatus in the last operation. for debugging only.
 */
typedef struct{
    uint8 id;
	uint8 state;
	TiFunEventHandler listener;
	void * lisowner;
    uint8 option;
	volatile uint8 rxlen;
	volatile char rxbuf[CC2420_RXBUFFER_SIZE];
	uint8 rssi;
	uint8 lqi;
	volatile uint8 spistatus;
	uint16 param[14];
}TiCc2420Adapter;

typedef struct{
  read
  write
  setpandi
  setshortaddrto
  
}UiTransceiverAdapter;

Ti
Ui
Ix
I
TiXxxInterface



Ti
Itmnopqrstuv
Iv
UiTransceiverAdapter


/*******************************************************************************
 * cc2420_construct
 * Construct the cc2420 object inside the specified memory block
 * 
 * cc2420_destroy
 * Destroy the memory block allocated to cc2420 object
 *
 * cc2420_open
 * Initialize cc2420 object for read/write. 
 * 
 * cc2420_close
 * Close cc2420 object. Disable related interrupts, and release allocated resources.
 *
 ******************************************************************************/

TiCc2420Adapter * cc2420_construct( void * mem, uint16 size );
void cc2420_destroy( TiCc2420Adapter * cc );
// todo: TiSpiAdapter
TiCc2420Adapter * cc2420_open( TiCc2420Adapter * cc, uint8 id, TiFunEventHandler listener, void * lisowner, uint8 option );
void cc2420_close( TiCc2420Adapter * cc );

uint8 cc2420_state( TiCc2420Adapter * cc );

#define cc2420_write(cc,buf,len,option) cc2420_send(cc,buf,len,option)
#define cc2420_read(cc,buf,len,option) cc2420_recv(cc,buf,len,option)

uint8 cc2420_write( TiCc2420Adapter * cc, char * buf, uint8 len, uint8 option );
uint8 cc2420_read( TiCc2420Adapter * cc, char * buf, uint8 size, uint8 option );
void  cc2420_evolve( TiCc2420Adapter * cc );
uint8 cc2420_setrxmode( TiCc2420Adapter * cc );
uint8 cc2420_settxmode( TiCc2420Adapter * cc );

uint8 cc2420_sleep( TiCc2420Adapter * cc );
uint8 cc2420_wakeup( TiCc2420Adapter * cc );


/*******************************************************************************
 * cc2420 PIN based operations
 ******************************************************************************/

#define cc2420_ischannelclear(cc) HAL_READ_CC_CCA_PIN()
uint8 cc2420_vrefon( TiCc2420Adapter * cc );
uint8 cc2420_vrefoff( TiCc2420Adapter * cc );
uint8 cc2420_powerdown( TiCc2420Adapter * cc );
uint8 cc2420_powerup( TiCc2420Adapter * cc );

/*******************************************************************************
 * cc2420 command operations
 * These functions can be called only when the voltage regulator is on.
 ******************************************************************************/

uint8 cc2420_sendcmd( TiCc2420Adapter * cc, uint8 addr);
inline uint8 cc2420_getcmdstatus( TiCc2420Adapter * cc) {return cc2420_sendcmd(cc,CC2420_SNOP);};

uint8 cc2420_snop( TiCc2420Adapter * cc );
uint8 cc2420_oscon( TiCc2420Adapter * cc );
uint8 cc2420_oscoff( TiCc2420Adapter * cc );
uint8 cc2420_calibrate( TiCc2420Adapter * cc );
uint8 cc2420_rxon( TiCc2420Adapter * cc );
uint8 cc2420_txon( TiCc2420Adapter * cc );
uint8 cc2420_txoncca( TiCc2420Adapter * cc );
uint8 cc2420_rfoff( TiCc2420Adapter * cc );                 

uint8 cc2420_flushrx( TiCc2420Adapter * cc );
uint8 cc2420_flushtx( TiCc2420Adapter * cc );               
uint8 cc2420_ack( TiCc2420Adapter * cc );      
uint8 cc2420_rxdec( TiCc2420Adapter * cc );               
uint8 cc2420_txenc( TiCc2420Adapter * cc );               


/*******************************************************************************
 * cc2420 register/memory access functions and configuration functions.
 ******************************************************************************/

uint8 cc2420_writeregister( TiCc2420Adapter * cc, uint8 addr, uint16 data);
uint16 cc2420_readregister( TiCc2420Adapter * cc, uint8 addr);
uint8 cc2420_readlargeram( TiCc2420Adapter * cc, uint16 addr, uint8 length, uint8 *buffer);
uint8 cc2420_writelargeram( TiCc2420Adapter * cc, uint16 addr, uint8 length, uint8 *buffer);

/*	If cc2420's MDMCTRL0.AUTOACK is enabled, an acknowledge frame is transmitted 
 * for all incoming frames accepted by the address recognition with the acknowledge  
 * request flag set and a valid CRC. AUTOACK therefore does not make sense unless 
 * also ADR_DECODE and AUTOCRC are enabled. The sequence number is copied from 
 * the incoming frame. 
 *	AUTOACK may be used for non-beacon systems as long as the frame pending field 
 * is cleared. The acknowledge frame is then transmitted 12 symbol periods after 
 * the last symbol of the incoming frame. 
 *	Two command strobes SACKPEND are defined to transmit the acknowledge frames.
 * The ACK frame is only transmitted when CRC ok.
 */
uint8 cc2420_enable_autoack( TiCc2420Adapter * cc ) ;
uint8 cc2420_disable_autoack( TiCc2420Adapter * cc );

uint8 cc2420_enable_addrdecode( TiCc2420Adapter * cc );
uint8 cc2420_disable_addrdecode( TiCc2420Adapter * cc ) ;

void  cc2420_setcoordinator( TiCc2420Adapter * cc, bool flag );

uint8 cc2420_setchannel( TiCc2420Adapter * cc, uint8 chn );
uint8 cc2420_setshortaddress( TiCc2420Adapter * cc, uint16 addr );
uint8 cc2420_getshortaddress( TiCc2420Adapter * cc, uint16 * addr );
uint8 cc2420_setpanid( TiCc2420Adapter * cc, uint16 id );
uint8 cc2420_getpanid( TiCc2420Adapter * cc, uint16 * id );

uint8 cc2420_settxpower( TiCc2420Adapter * cc, uint8 power );

/*******************************************************************************
 * cc2420 adapter enhanced functions
 ******************************************************************************/

/* returns the rssi(received signal strength indicator) of the frame just read
 * by cc2420_recv(...) */
uint8 cc2420_rssi( TiCc2420Adapter * cc );

/* returns the lqi(link quality indicator) of the frame just read by cc2420_recv(...) */
uint8 cc2420_lqi( TiCc2420Adapter * cc );

/* returns true when the frame just read by cc2420_recv(...) passed crc verification*/
bool cc2420_crctest( TiCc2420Adapter * cc );

void cc2420_dump( TiCc2420Adapter * cc );

/*******************************************************************************
 * cc2420 interrupt related functions
 ******************************************************************************/

/* cc2420_enable_fifop
 * Enable the cc2420's FIFOP interrupt. The cc2420 uses the FIFOP interrupt to notify 
 * the MCU a frame is arrival and accepted.
 */
void cc2420_enable_fifop( TiCc2420Adapter * cc );
void cc2420_disable_fifop( TiCc2420Adapter * cc );

/* cc2420_enable_sfd
 * Enable the cc2420's SFD interrupt. The cc2420 uses the SFD interrupt to notify 
 * the MCU a frame is just incoming. This interrupt is usually used in time synchronization.
 */
void cc2420_enable_sfd( TiCc2420Adapter * cc );
void cc2420_disable_sfd( TiCc2420Adapter * cc );
//void cc2420_set_sfd_listener( TiCc2420Adapter * cc, TiFunEventHandler sfd_listener, void * owner );

void cc2420_default_listener( void * ccptr, TiEvent * e ); 

#endif  /* _CC2420_H_4892_ */
