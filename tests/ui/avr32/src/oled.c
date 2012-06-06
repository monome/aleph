/* oled.c
   ui test
   aleph
*/

// ASF
#include "gpio.h"
#include "usart.h"
#include "delay.h"
// aleph
#include "config.h"
#include "oled.h"
#include "font.h"

//-----------------------------
//---- static variables
static U8 screen[GRAM_BYTES];

//-----------------------------
//---- static functions
static void write_data(U8 c);
static void write_data(U8 c) {
  usart_spi_selectChip(OLED_USART_SPI);
  // pull register select high to write data
  gpio_set_gpio_pin(OLED_REGISTER_PIN);
  usart_putchar(OLED_USART_SPI, c);
  usart_spi_unselectChip(OLED_USART_SPI);
}

static void write_command(U8 c);
static void write_command(U8 c) {
  usart_spi_selectChip(OLED_USART_SPI);
  // pull register select low to write a command
  gpio_clr_gpio_pin(OLED_REGISTER_PIN);
  usart_putchar(OLED_USART_SPI, c);
  usart_spi_unselectChip(OLED_USART_SPI);
}

//------------------
// external functions
void init_oled(void) {
  U32 i;
  //// initialize OLED
  /// todo: maybe toggle oled RESET to clear its shift register?
  write_command(0xAE);	// off
  write_command(0x86);	// full current range
  write_command(0xA4);	// normal display
  write_command(0x81);	// contrast to full
  write_command(0x7F);
  write_command(0xA8);	// multiplex
  write_command(0x3F);
  write_command(0xBC);	// precharge
  write_command(0x10);
  write_command(0xBE);	// voltage
  write_command(0x1C);
  write_command(0xAD);	// dcdc
  write_command(0x02);
  write_command(0xA0);	// remap
  write_command(0x50);
  write_command(0xA1);	// start
  write_command(0x0);
  write_command(0xA2);	// offset
  write_command(0x4C);
  write_command(0xB1);	// set phase
  write_command(0x55);
	
  // set update box (to full screen)
  write_command(0x15);
  write_command(0);
  write_command(63);
  write_command(0x75);
  write_command(0);
  write_command(63);
		
  // clear OLED RAM
  for(i=0; i<GRAM_BYTES; i++) { write_data(0); }
   
  write_command(0xAF);	// on

  delay_ms(20);
       
}

/////// testing...
/*
void oled_draw_screen_raw(U16 pos, U8 data) {
  screen[pos] = (data);
}
*/

void oled_draw_pixel(U16 x, U16 y, U8 a) {
  static U32 pos;
  pos = (y * NCOLS_2) + (x>>1);
  if (x%2) {
    screen[pos] &= 0x0f;
    screen[pos] |= (a << 4);
  } else {
    screen[pos] &= 0xf0;
    screen[pos] |= (a & 0x0f);

  }
}

void oled_draw_char(U16 col, U16 row, char c, U8 a) {
  static U8 x, y;
  for(y=0; y<FONT_CHARH; y++) {
    for(x=0; x<FONT_CHARW; x++) {
      // for brians font:  
      /*
	for(y=0; y<6; y++) {
	for(x=0; x<6; x++) {
      */
      if((rom_font[c * FONT_CHARH + y] & (FONT_COLMASK >> x)) > 0) {
	// for brians font:
	// if((rom_font[c * FONT_CHARH + y + FONT_ROW_OFFSET] & (1 << x)) > 0) {
	oled_draw_pixel(x+col, y+row, a);
      } else {
	oled_draw_pixel(x+col, y+row, 0);
      }
    }
  }
}


void oled_draw_string(U16 x, U16 y, char *str, U8 a) {
  while(*str != 0) {
    oled_draw_char(x, y, *str, a);
    x += FONT_CHARW;
    //x += rom_font[(*str * 7)];
    str++;
  }
}

void oled_refresh(void) {
  U16 i;
  for(i=0; i<GRAM_BYTES; i++) { write_data(screen[i]); }
}
