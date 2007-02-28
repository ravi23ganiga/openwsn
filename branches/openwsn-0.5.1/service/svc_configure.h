#ifndef _CONFIGURE_H_2539_
#define _CONFIGURE_H_2539_

#include "svc_foundation.h"
#include "..\hal\hal_uart.h"
#include "svc_console.h"

// uniqueid
// uniqueid id read from an electronic component. this is a world-wide unique 
// identifier. the device can use this unique id to generate unique network 
// address.
//
// rmtaddress
// for wireless mode, there should at least 2 device. however, this software 
// support multiple devices larger than 2. they can organized as one group. 
// and when you sending data, you should adopt the frame-support UART stream 
// interface so that you can assign the destination node address in the frame. 
// for transparent streaming wireless modem, it could only transmit the data 
// between two devices. in this case, the rmtaddress is the sender's parter device
// address.
//
#define CONFIGURE_MODE_MODEMGROUP	0
#define CONFIGURE_MODE_STREAM 		1
#define CONFIGURE_MODE_OPENMAC		2

typedef struct{
  uint8 mode;				// transparent line / modem group / distributed MAC @TODO in future
  uint8 majorversion;
  uint8 minorversion;
  uint16 uart_baudrate;
  uint8 uart_databits;
  uint8 uart_stopbits;
  uint8 uart_parity;
  uint8 uart_frameprefix;
  uint8 uart_frameidentify;	// search frames in the stream buffer
  uint8 uniqueid[6];		// PHY unique ID, this world-wide unqiue id is read from hardware.
  uint16 panid;         	// MAC layer PAN ID
  uint16 localaddress;		// MAC layer local address
  uint16 rmtaddress;		// MAC layer remote address
  uint8 wlschannel;			// wireless channel, to change the frequvency
  uint8 wlspower;			// wireless TX power
  char  owner[64];
}TConfigStore;

typedef struct{
  boolean modified;
  TConsole * console;
  TConfigStore data;
}TConfigure;

TConfigure * config_construct( char * buf, uint16 size, TConsole * con );
void config_destroy( TConfigure * config );
void config_save( TConfigure * config, uint32 flashaddr, uint32 flashsize );
void config_load( TConfigure * config, uint32 flashaddr, uint32 flashsize );
boolean config_modified( TConfigure * config );
void config_apply( TConfigure * config );
void config_getdefault( TConfigStore * configstore );
void config_restore_default( TConfigure * config );
int8 config_execute( TConfigure * config );

#endif
