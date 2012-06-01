/* oled.c
   ui test
   aleph
*/

// ASF
#include "gpio.h"
#include "usart.h"
// aleph
#include "config.h"
#include "oled.h"
#include "font.h"

static U8 screen[NPIXELS];

static void write_data(U8 c);
static void write_data(U8 c) {
  usart_spi_selectChip(OLED_USART_SPI);
  // pull register select high to write data
  //  gpio_set_gpio_pin(OLED_REGISTER_PIN);
  //? 
  usart_putchar(OLED_USART_SPI, 0x55);
  usart_putchar(OLED_USART_SPI, c);
  usart_spi_unselectChip(OLED_USART_SPI);
}

static void write_command(U8 c);
static void write_command(U8 c) {
  usart_spi_selectChip(OLED_USART_SPI);
  // pull register select low to write a command
  //  gpio_clr_gpio_pin(OLED_REGISTER_PIN);
  usart_putchar(OLED_USART_SPI, c);
  usart_spi_unselectChip(OLED_USART_SPI);
}

void init_oled(void) {
  U16 i;
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
	
  // clear data
  for(i = 0; i < 4096; i++){
    write_data(0);	
  }
	
  for(i = 0; i < 512; i++){
    screen[i] = 0;
  }
  write_command(0xAF);	// on
       
}

/////// testing...
void oled_draw_pixel_raw(U16 pos, U8 a) {
  a &= 0x0f;
  screen[pos] &= 0xf0;
  screen[pos] |= a;
}


//// FIXME: clarify and get rid of magic numbers
void oled_draw_pixel(U16 x, U16 y, U8 a) {
  a &= 0x0f; // scrub off left four bits
  U16 position = (y * 64) + (x/2);
  if(!(x & 1)) {
    screen[position] &= 0xf0;
    screen[position] |= a;
  }
  else {
    screen[position] &= 0x0f;
    screen[position] |= (a<<4);
  }
}

void oled_draw_char(U16 x, U16 y, U8 c, U8 a) {
  //magic numbers: 8 = height, 8 = width
  U16 row, col;
  for(row=0;row<6;row++) {
    for(col=0;col<6;col++) {
      oled_draw_pixel(x+col, y+row, ((rom_font[(c*7)+row+1] & (1<<col))!=0)*a);
    }
  }
}


void oled_draw_string(U16 x, U16 y, U8 *str, U8 a) {
  while(*str != 0) {
    oled_draw_char(x, y, *str, a);
    x += rom_font[(*str * 7)];
    str++;
  }
}

void oled_refresh(void) {
  U16 i;
  for(i=0; i<NPIXELS; i++) { write_data(screen[i]); }
}
