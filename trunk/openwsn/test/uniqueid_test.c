#include "..\foundation.h"
#include "..\global.h"
#include "uniqueid_test.h"
#include "..\hal\hal_led.h"


void uniqueid_run( void )
{
  
  TUniqueIdDriver uid;
  char uidbuf[6];
  
  target_init();

  led_twinkle(LED_RED,3);
  led_twinkle(LED_YELLOW,3);
  led_twinkle(LED_GREEN,3);
  
  global_construct();
  uart_configure( g_uart, CONFIG_UART_BAUDRATE, 0, 0, 0, 0 );
  uid_construct( 1, (char*)(&uid), sizeof(uid), 1 );
  uart_putchar(g_uart, 0x22);
  
  while(1){
   led_twinkle(LED_YELLOW,4);
   uart_putchar(g_uart, 0x88);
   uid_read( &uid, uidbuf, 6, 0x00 ); 
    uart_putchar( g_uart, (char)uid.familycode);
    uart_write( g_uart, uidbuf, sizeof(uidbuf), 0x00 );
  }
  
  /*while(1){
 
  uid_read( uniqueid,,, );
  value = uniqueid->familycode;
  
  led_twinkle(LED_GREEN,2);
  
  uart_putchar(g_uart, 0x88);
  uart_putchar( g_uart, (char)uid.familycode);
  uart_write( g_uart,(char *) &uniqueid.familycode, 8, 0 );
  led_twinkle(LED_YELLOW,4);
  }*/
  
}
