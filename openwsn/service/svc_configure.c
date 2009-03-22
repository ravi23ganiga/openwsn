
#include "svc_configall.h"
#include <../hal/hal_foundation.h>
#include <../hal/hal_cpu.h>
#include <../hal/hal_flash.h>
#include <../hal/hal_uart.h>
#include <../hal/hal_assert.h>
#include "svc_foundation.h"
#include "svc_configure.h"

#define CONSOLE_LINE_WIDTH  80

#define kwriteln(con,s) console_writeline(con,s)
#define kwrite(con,s) console_write(con,s,strlen(s))
#define kreadln(con,line,size) console_readline(con,line,size)
#define kprompt(con,s,line,size) console_prompt(con,s,line,size)

static void _config_display_mainmenu( TiConfigure * config );
static void _config_system_summary( TiConfigure * config );
static void _config_general( TiConfigure * config );
static void _config_uart( TiConfigure * config );
static void _config_wireless( TiConfigure * config );
#ifdef GDEBUG
static void _config_show_debuginformation( TiConfigure * config );
#endif

TiConfigure * config_construct( char * buf, uint16 size, TiConsole * con )
{
	TiConfigure * config;
	
	if (sizeof(TiConfigure) > size)
		config = NULL;
	else
		config = (TiConfigure *)buf;
		
	if (config != NULL)
	{
		memset( buf, 0x00, sizeof(TiConfigure) );
		config->modified = FALSE;
		config->console = con;
		config_getdefault( &(config->data) );
	}
	
	return config;
}

// @note
//	you should NOT destroy the internal TUartDriver object here. It may still 
// be used by other objects yet.
//
void config_destroy( TiConfigure * config )
{
	NULL;
}

// write the config data in "config->data" to flash memory.
// @TODO flash_write
void config_save( TiConfigure * config, uint32 flashaddr, uint32 flashsize )
{
	assert( sizeof(TiConfigStore) >= flashsize ); 
	flash_write( flashaddr, (uint32)(&(config->data)), sizeof(TiConfigStore) ); 
}

// load config data in flash memory into "config->data".
// @TODO flash_read
void config_load( TiConfigure * config, uint32 flashaddr, uint32 flashsize )
{
	memset( &(config->data), 0x00, sizeof(TiConfigStore) );
	flash_read( flashaddr, (uint32)(&(config->data)), sizeof(TiConfigStore) ); 
}

boolean config_modified( TiConfigure * config )
{
	return config->modified;
}

// @TODO you may need activate other configurations in this function.
// the current version will reboot the whole system to make the changings active.
//
void config_apply( TiConfigure * config )
{
//	uart_configure( g_uart, config->data.uart_baudrate, config->data.uart_databits,
//		config->data.uart_stopbits, config->data.uart_parity, 0x01 );
	NULL;
}

void config_getdefault( TiConfigStore * store )
{
  	store->mode = CONFIGURE_MODE_MODEMGROUP;
	store->majorversion = OPENWSN_MAJOR_VERSION;
  	store->minorversion = OPENWSN_MINOR_VERSION;
  	store->uart_baudrate = 9600;
  	store->uart_databits = 8;
  	store->uart_stopbits = 1;
  	store->uart_parity = 0;
  	store->uart_frameprefix = 27; // character ESC(27)
  	store->uart_frameidentify = TRUE;
  	memset( (void*)(store->uniqueid[0]), 0x00, 6  );
  	store->panid = 1;
  	store->localaddress = 1;
  	store->rmtaddress = 1;
  	store->mode = 0;
  	store->wlschannel = 5; 	// decided by cc2420
  	store->wlspower = 1;	// medium distance transmission power
  	strncpy( (char *)&(store->owner[0]), "TongJi OpenM2M zhang_wi@tongji.edu.cn", 32 );
}

// Restore the default settings of the application. Attention that all the default
// parameters are hard-coded in the source code. They are all in this function. 
//
// @attention: this function only restore the default configuration parameters
// into the SRAM TiConfigure object. You should save this object into flash memory
// or E2PROM if you want to keep them permanantly.
//
void config_restore_default( TiConfigure * config )
{
	config_getdefault( &(config->data) );
}

// run the configuration process of the application. You can manipulate the 
// configuration process by connect a computer to the device with an RS232 cable.
// and start a Terminal on the computer. Then you should first see the welcome
// message.
//
// @return 
//	1		configure save and exit. This function will automatically call
//			config_apply() to make these changes effective in this case.
//	-1		not save and exit
//
#define CONFIG_LINE_BUFFER_SIZE 8
int8 config_execute( TiConfigure * config )
{
	char linebuf[8], choice;
	boolean done = FALSE;
	int16 ret = 0;
	
	kwriteln( config->console, "Configuration Booting.................................." );
	kwriteln( config->console, "Welcome to Wireless Modem." );
	kwriteln( config->console, "Simens Automation. 2006" );
	
	while (!done)
	{
		_config_display_mainmenu( config );
		while (TRUE)
		{
			memset( &(linebuf[0]), 0x00, CONFIG_LINE_BUFFER_SIZE );
			if (console_readline( config->console, linebuf, CONFIG_LINE_BUFFER_SIZE ) > 0)
				break;
		}
		choice = linebuf[0];	
		kwriteln( config->console, NULL );

		switch (choice)
		{
		case '1':
			_config_system_summary( config );
			break;
			
		case '2':
			_config_general( config );
			break;

		case '3':
			_config_uart( config );
			break;

		case '4':
			_config_wireless( config );
			break;
			
		case '5':
			kwriteln( config->console, "Warning: all the current settings will be lost!!!" );
			kwrite(   config->console, "Do you still want to restore the default settings? (Y/N)" );
			while (TRUE)
			{
				memset( &(linebuf[0]), 0x00, CONFIG_LINE_BUFFER_SIZE );
				if (console_readline( config->console, linebuf, CONFIG_LINE_BUFFER_SIZE ) > 0)
					break;
			}
			if (toupper(linebuf[0]) == 'Y')
			{
				config_restore_default( config );
				kwriteln( config->console, NULL );
				_config_system_summary( config );
				kwriteln( config->console, "restore default setting successfully!" );
			}
			else{
				kwriteln( config->console, "\r\nrestore default settings aborted!" );
			}
			break;
			
		case '6':
			kwriteln( config->console, "Warning: all the old settings will be lost!!!" );
			kwrite(   config->console, "Save & Exit? (Y/N)" );
			while (TRUE)
			{
				memset( &(linebuf[0]), 0x00, CONFIG_LINE_BUFFER_SIZE );
				if (console_readline( config->console, linebuf, CONFIG_LINE_BUFFER_SIZE ) > 0)
					break;
			}
			if (toupper(linebuf[0]) == 'Y')
			{
				// zhangwei comment it to make it compile passed  20090320
				// 
				//config_save( config, CONFIG_FLASHSTORE_ADDR, CONFIG_FLASHSTORE_SIZE );
				kwriteln( config->console, "\r\nsave successfully." );
				config_apply( config );
				config->modified = FALSE;
				ret = 1;
				done = TRUE;
			}
			break;
			
		case '7':
			kwriteln( config->console, "Warning: all the current settings will be lost!!!" );
			kwrite(   config->console, "Discard Changes & Exit? (Y/N)" );
			while (TRUE)
			{
				memset( &(linebuf[0]), 0x00, CONFIG_LINE_BUFFER_SIZE );
				if (console_readline( config->console, linebuf, CONFIG_LINE_BUFFER_SIZE ) > 0)
					break;
			}
			if (toupper(linebuf[0]) == 'Y')
			{
				// zhangwei comment the following to make it compile passed
				//config_load( config, CONFIG_FLASHSTORE_ADDR, CONFIG_FLASHSTORE_SIZE );
				kwriteln( config->console, "\r\nchanges discarded." );
				config->modified = FALSE;
				ret = 2;
				done = TRUE;
			}
			break;
			
		#ifdef GDEBUG
		case '9':
			_config_show_debuginformation( config );
			break;
		#endif
		
		}
	}
	
	return ret;
}
#undef CONFIG_LINE_BUFFER_SIZE

void _config_display_mainmenu( TiConfigure * config )
{
	kwriteln( config->console, NULL );
	kwriteln( config->console, "================ Main Menu ================" );
	kwriteln( config->console, "1) System summarization" );
	kwriteln( config->console, "2) General settings" );
	kwriteln( config->console, "3) UART settings" );
	kwriteln( config->console, "4) Wireless settings" );
	kwriteln( config->console, "5) Restore default settings" );
	kwriteln( config->console, "6) Save & exit" );
	kwriteln( config->console, "7) Discard changes & exit" );
	#ifdef GDEBUG
	kwriteln( config->console, "9) Print debug information" );
	#endif
	kwrite(   config->console, "Please input your choice (1-7) and press <Enter>:" );
}

void _config_system_summary( TiConfigure * config )
{
	char buf[CONSOLE_LINE_WIDTH];
	char * pc = &(buf[0]);
	char * fmt10 = "  Version : %d.%d";
	char * fmt20 = "  General : Pan/Node Address %d/%d";
	char * fmt21 = "            Smart Packet Identification %s";
	char * fmt22 = "             (Prefix Flag 0x%x)";
	char * fmt30 = "  UART    : baudrate %d, databits %d, stopbits %d ";
	char * fmt31 = "              parity %d";
	char * fmt40 = "  Wireless: channel %d";
	
	kwriteln( config->console, "System Configuration Summary:" );
	memset( pc, 0x00, CONSOLE_LINE_WIDTH );

	snprintf( pc, CONSOLE_LINE_WIDTH-1, fmt10, config->data.majorversion, config->data.minorversion );  
	kwriteln( config->console, pc );

	snprintf( pc, CONSOLE_LINE_WIDTH-1, fmt20, config->data.panid, config->data.localaddress );
	kwriteln( config->console, pc );

	if (config->data.uart_frameidentify)
	{
		snprintf( pc, CONSOLE_LINE_WIDTH-1, fmt21, "ON" );
		kwriteln( config->console, pc );
		snprintf( pc, CONSOLE_LINE_WIDTH-1, fmt22, config->data.uart_frameprefix );
		kwriteln( config->console, pc );
	}
	else{
		snprintf( pc, CONSOLE_LINE_WIDTH-1, fmt21, "OFF" );
		kwriteln( config->console, pc );
	}
	
	snprintf( pc, CONSOLE_LINE_WIDTH-1, fmt30, config->data.uart_baudrate, config->data.uart_databits, config->data.uart_stopbits );
	kwriteln( config->console, pc );

	snprintf( pc, CONSOLE_LINE_WIDTH-1, fmt31, config->data.uart_parity );
	kwriteln( config->console, pc );
	
	snprintf( pc, CONSOLE_LINE_WIDTH-1, fmt40, config->data.wlschannel ); 
	kwriteln( config->console, pc );
}

void _config_general( TiConfigure * config )
{
	char buf[CONSOLE_LINE_WIDTH];
	char * line = &(buf[0]);
	
	memset( line, 0x00, CONSOLE_LINE_WIDTH );

	kwriteln( config->console, "General Settings:" );
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  current Pan/Node Address %d/%d", config->data.panid, config->data.localaddress );
	kwriteln( config->console, line );

	snprintf( line, CONSOLE_LINE_WIDTH-1, "  input new Pan address(%d)(between 1-128): ", config->data.panid );
	if (kprompt(config->console,line,line,CONSOLE_LINE_WIDTH-1) > 0)
	{
		config->data.panid = atoi(line) % 128;
		config->modified = TRUE;
	}
	
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  input new Node address(%d)(between 1-128): ", config->data.localaddress );
	if (kprompt(config->console,line,line,CONSOLE_LINE_WIDTH-1) > 0)
	{
		config->data.localaddress = atoi(line) % 128;
		config->modified = TRUE;
	}
	
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  input new Remote Node address(%d)(between 1-128): ", config->data.localaddress );
	if (kprompt(config->console,line,line,CONSOLE_LINE_WIDTH-1) > 0)
	{
		config->data.localaddress = atoi(line) % 128;
		config->modified = TRUE;
	}
	
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  Change Pan/Local/Remote Address as %d/%d/%d Ok.", 
		config->data.panid, config->data.localaddress, config->data.rmtaddress );
	kwriteln( config->console, NULL );
	kwriteln( config->console, line );
	
	// @TODO: smart packet identification sections here
}

void _config_uart( TiConfigure * config )
{
	char buf[CONSOLE_LINE_WIDTH];
	char * line = &(buf[0]);

	kwriteln( config->console, "UART Settngs:" );
	memset( line, 0x00, CONSOLE_LINE_WIDTH );
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  current baudrate %d, databits %d, stopbits %d, parity %d",
		config->data.uart_baudrate, config->data.uart_databits,
		config->data.uart_stopbits, config->data.uart_parity );
	kwriteln( config->console, line );
	
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  input new Baudrate(%d): ", config->data.uart_baudrate );
	if (kprompt(config->console, line, line, CONSOLE_LINE_WIDTH-1) > 0)
	{
		config->data.uart_baudrate = atoi(line);
		config->modified = TRUE;
	}
	
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  input new Databits(%d): ", config->data.uart_databits );
	if (kprompt(config->console, line, line, CONSOLE_LINE_WIDTH-1) > 0)
	{
		config->data.uart_databits = atoi(line);
		config->modified = TRUE;
	}
	
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  input new Stopbits(%d): ", config->data.uart_stopbits );
	if (kprompt(config->console, line, line, CONSOLE_LINE_WIDTH-1) > 0)
	{
		config->data.uart_stopbits = atoi(line);
		config->modified = TRUE;
	}
	
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  input new Parity(%d): ", config->data.uart_parity );
	if (kprompt(config->console, line, line, CONSOLE_LINE_WIDTH-1) > 0)
	{
		config->data.uart_parity = atoi(line);
		config->modified = TRUE;
	}
	
	memset( line, 0x00, CONSOLE_LINE_WIDTH );
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  baudrate %d, databits %d, stopbits %d, parity %d",
		config->data.uart_baudrate, config->data.uart_databits,
		config->data.uart_stopbits, config->data.uart_parity );
	kwriteln( config->console, "  Changeing UART parameters..." );
	kwriteln( config->console, "  You should restart the computer terminal to communicate with this device using new settings after saving." );
	
	// you SHOULD not do uart_configure() here. you must keep the old UART settings
	// here to continue the configuration process.
	//
	//uart_configure( g_uart, config->data.uart_baudrate, config->data.uart_databits,
	//	config->data.uart_stopbits, config->data.uart_parity );
	//kwriteln( config->console, "  Change UART to new settings OK." );
	
	kwriteln( config->console, line );
}

void _config_wireless( TiConfigure * config )
{
	char buf[CONSOLE_LINE_WIDTH];
	char * line = &(buf[0]);
	char choice = 'X';
	boolean done = FALSE;

	kwriteln( config->console, "Wireless Settngs:" );
	memset( line, 0x00, CONSOLE_LINE_WIDTH );
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  current channel %d", config->data.wlschannel );
	kwriteln( config->console, line );
	
	// @TODO change the freuency settings 
	// more channels? i remember cc2420 has 32 channel
	kwriteln( config->console, "  1. channel 1(24024 MHz), Machster" );
	kwriteln( config->console, "  2. channel 1(24024 MHz), Machster" );
	kwriteln( config->console, "  3. channel 1(24024 MHz), Machster" );
	kwriteln( config->console, "  4. channel 1(24024 MHz), Machster" );
	kwriteln( config->console, "  5. channel 1(24024 MHz), Machster" );
	kwriteln( config->console, "  6. channel 1(24024 MHz), Machster" );
	kwriteln( config->console, "  7. channel 1(24024 MHz), Machster" );
	kwriteln( config->console, "  8. channel 1(24024 MHz), Machster" );
	
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  input your choice(1-8): " );
	done = FALSE;
	while (!done)
	{
		if (kprompt(config->console, line, line, CONSOLE_LINE_WIDTH-1) > 0)
		{
			choice = line[0];
			switch (choice)
			{
			case '1': config->data.wlschannel = 1; done = TRUE; break;
			case '2': config->data.wlschannel = 2; done = TRUE; break;
			case '3': config->data.wlschannel = 3; done = TRUE; break;
			case '4': config->data.wlschannel = 4; done = TRUE; break;
			case '5': config->data.wlschannel = 5; done = TRUE; break;
			case '6': config->data.wlschannel = 6; done = TRUE; break;
			case '7': config->data.wlschannel = 7; done = TRUE; break;
			case '8': config->data.wlschannel = 8; done = TRUE; break;
			default: choice = 'X'; done = FALSE; break;
			}
		}
	}
	
	memset( line, 0x00, CONSOLE_LINE_WIDTH );
	snprintf( line, CONSOLE_LINE_WIDTH-1, "  your choice is %c", choice );
	kwriteln( config->console, line );
	kwriteln( config->console, "  change settings OK." );
	config->modified = TRUE;
}

#ifdef GDEBUG
void _config_show_debuginformation( TiConfigure * config )
{
	kwriteln( config->console, "Debug Information: " );
	// @TODO: print the internal debug data to the console for debugging
}
#endif

// these macros are only effective in this file. so you should undef them
// at the end of this module.
#undef kkwriteln
#undef kwrite
#undef kreadln
#undef kprompt

