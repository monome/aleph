/* oled.c
   ui test
   aleph
*/

// std
#include <stdio.h>
// ASF
#include "gpio.h"
#include "usart.h"
#include "delay.h"
#include "intc.h"
// aleph
#include "config.h"
#include "screen.h"
#include "font.h"

//-----------------------------
//---- static variables
// screen buffer
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

// fill one column in a line of text with blank pixels (for spacing)
static void zero_col(U16 x, U16 y);
static void zero_col(U16 x, U16 y) {
  static U8 i;
  for(i=0; i<FONT_CHARH; i++) {
    screen_draw_pixel(x, y+i, 0);
  }
}

//------------------
// external functions
void init_oled(void) {
  U32 i;
  //  cpu_irq_disable();
  Disable_global_interrupt();
  delay_ms(100);
  //// initialize OLED
  /// todo: maybe toggle oled RESET to clear its shift register?
  write_command(0xAE);	// off
  write_command(0xB3);	// clock rate
  write_command(0x91);
  write_command(0xA8);	// multiplex
  write_command(0x3F);
  write_command(0x86);	// full current range
  write_command(0x81);	// contrast to full
  write_command(0x7F);
  write_command(0xB2);	// frame freq
  write_command(0x51);
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
  write_command(0xB4);	// precharge
  write_command(0x02);
  write_command(0xB0);	// precharge
  write_command(0x28);
  write_command(0xBF);	// vsl
  write_command(0x0F);
  write_command(0xA4);	// normal display
	
  write_command(0xB8);		// greyscale table
  write_command(0x01);
  write_command(0x11);
  write_command(0x22);
  write_command(0x32);
  write_command(0x43);
  write_command(0x54);
  write_command(0x65);
  write_command(0x76);	
	
	
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

  delay_ms(100);

  //  cpu_irq_enable();
  Enable_global_interrupt();

       
}

// draw a single pixel
void screen_draw_pixel(U16 x, U16 y, U8 a) {
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

// draw a single character glyph with fixed spacing
U8 screen_draw_char(U16 col, U16 row, char gl, U8 a) {
  static U8 x, y;
  for(y=0; y<FONT_CHARH; y++) {
    for(x=0; x<FONT_CHARW; x++) {
      if( (font_data[gl - FONT_ASCII_OFFSET].data[x] & (1 << y))) {
	screen_draw_pixel(x+col, y+row, a);
      } else {
	screen_draw_pixel(x+col, y+row, 0);
      }
    }
  }
  return x+1;
}

// draw a single character glyph with proportional spacing
U8 screen_draw_char_squeeze(U16 col, U16 row, char gl, U8 a) {
  static U8 y, x;
  static U8 xnum;
  static const glyph_t * g;
  g = &(font_data[gl - FONT_ASCII_OFFSET]);
  xnum = FONT_CHARW - g->first - g->last;
  
  for(y=0; y<FONT_CHARH; y++) {
    for(x=0; x<xnum; x++) {
      if( (g->data[x + g->first] & (1 << y))) {
	screen_draw_pixel(x + col, y + row, a);
      } else {
	screen_draw_pixel(x + col, y + row, 0);
      }
    }
  }
  return xnum;
}

// draw a string with fixed spacing
U8 screen_draw_string(U16 x, U16 y, char *str, U8 a) {
  while(*str != 0) {
    x += screen_draw_char(x, y, *str, a) + 1;
    str++;
  }
  return x;
}

// draw a string with proportional spacing
U8 screen_draw_string_squeeze(U16 x, U16 y, char *str, U8 a) {
  while(*str != 0) {
    x += screen_draw_char_squeeze(x, y, *str, a);
    zero_col(x, y);
    // extra pixel... TODO: maybe variable spacing here
    x++;
    str++;
  }
  return x;
}

// print a formatted integer
U8 screen_draw_int(U16 x, U16 y, S32 i, U8 a) {
  static char buf[32];
  snprintf(buf, 32, "%d", (int)i);
  return screen_draw_string_squeeze(x, y, buf, a);
}

// print a formatted float
U8 screen_draw_float(U16 x, U16 y, F32 f, U8 a) {
  static char buf[32];
  snprintf(buf, 32, "%f", (float)f);
  return screen_draw_string_squeeze(x, y, buf, a);
}

// send screen buffer contents to OLED
void screen_refresh(void) {
  U16 i;
  //  cpu_irq_disable();
  //  Disable_global_interrupt();
  for(i=0; i<GRAM_BYTES; i++) { write_data(screen[i]); }
  //  cpu_irq_enable();
  //  Enable_global_interrupt();
}
