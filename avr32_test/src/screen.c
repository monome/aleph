/* screen.c
   avr32
   aleph
*/


// std 
/// FIXME: eliminate!!
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// ASF
#include "delay.h"
#include "gpio.h"
//#include "util.h"
#include "intc.h"
#include "print_funcs.h"
#include "spi.h"
// aleph
#include "aleph_board.h"
//#include "conf_aleph.h"
#include "fix.h"
#include "font.h"
#include "global.h"
#include "screen.h"

//-----------------------------
//---- variables
// const U8 lines[CHAR_ROWS] = { 0, 8, 16, 24, 32, 40, 48, 56 };

// screen buffer
static U8 screenBuf[GRAM_BYTES];

// common static vars
static u32 i, j;
//static u32 pos;
// fixed-point text buffer
//static char buf[FIX_DIG_TOTAL];

static void write_command(U8 c);
static void write_command(U8 c) {
  spi_selectChip(OLED_SPI, OLED_SPI_NPCS);
  // pull register select low to write a command
  gpio_clr_gpio_pin(OLED_REGISTER_PIN);
  spi_write(OLED_SPI, c);
  spi_unselectChip(OLED_SPI, OLED_SPI_NPCS);
}

// set the current drawing area of the physical screen (hopefully)
static void screen_set_rect(u8 x, u8 y, u8 w, u8 h);
void screen_set_rect(u8 x, u8 y, u8 w, u8 h) {
 // set column address
  write_command(0x15);		// command
  write_command(x);	// column start
  write_command((x+w-1));	// column end
  // set row address
  write_command(0x75);		// command
  write_command(y);	// column start
  write_command(y+h-1);	// column end
}

//------------------
// external functions
void init_oled(void) {
  //U32 i;
  // volatile u64 delay;
  //  cpu_irq_disable();
  Disable_global_interrupt();
  //  delay_ms(1);
  // flip the reset pin
  gpio_set_gpio_pin(OLED_RESET_PIN);
  delay_ms(1);
  gpio_clr_gpio_pin(OLED_RESET_PIN);
  delay_ms(1);
  gpio_set_gpio_pin(OLED_RESET_PIN);
  delay_ms(10);

  //// initialize OLED
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
  write_command(0x50);	// 0b01010000
			// a[6] : enable COM split odd/even
			// a[4] : enable COM re-map
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
  write_command(0xB8);	// greyscale table
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
  write_command(63); // ???

  screen_clear();
  
  write_command(0xAF);	// on
  delay_ms(10) ;
  //  cpu_irq_enable();
  Enable_global_interrupt();
}


// draw data given target rect
// assume x-offset and width are both even!
extern void screen_draw_region(u8 x, u8 y, u8 w, u8 h, u8* data) {
  u8* pScr; // movable pointer to screen buf
  u32 nb; // count of destination bytes
  // 1 row address = 2 horizontal pixels
  w >>= 1;
  x >>= 1;
  nb = w * h;
  /// copy to global buffer and pack bytes
  //  pScr  = (u8*)screenBuf;
  /// the screen is mounted upside down!'

  /// ah this is wrong
  pScr = (u8*)screenBuf + nb - 1;
  for(j=0; j<h; j++) {
    for(i=0; i<w; i+=2) {
      *pScr = (0xf0 & (*data << 4) );
      data++;
      *pScr |= (*data & 0xf);
      data++;
      pScr--;
    }
  }
  
  // flip the screen coordinates 
  x = SCREEN_ROW_BYTES - x - w;
  y = SCREEN_COL_BYTES - y - h;
  
  // set drawing region
  screen_set_rect(x, y, w, h);
  // select chip for data
  spi_selectChip(OLED_SPI, OLED_SPI_NPCS);
  // register select high for data
  gpio_set_gpio_pin(OLED_REGISTER_PIN);
  // send data
  for(i=0; i<(nb); i++) {
    spi_write(OLED_SPI, screenBuf[i]);
  }
  spi_unselectChip(OLED_SPI, OLED_SPI_NPCS);
}

// startup screen (FIXME: just grey at the moment)
void screen_startup(void) {
  u32 i;

  // fill screenbuffer with grey background
  for(i=0; i<GRAM_BYTES; i++) {
    screenBuf[i] = 0x77;
  }

  // net art: draw glyphs to screen buf directly,
  // should be stretched out and upside down
  font_glyph('A', screenBuf + 80,      64, 0xff, 0x44);
  font_glyph('L', screenBuf + 80 + 8,  64, 0xff, 0x44);
  font_glyph('E', screenBuf + 80 + 16, 64, 0xff, 0x44);
  font_glyph('P', screenBuf + 80 + 24, 64, 0xff, 0x44);
  font_glyph('H', screenBuf + 80 + 32, 64, 0xff, 0x44);

    // send screenbuffer
  spi_selectChip(OLED_SPI, OLED_SPI_NPCS);
  // register select high for data
  gpio_set_gpio_pin(OLED_REGISTER_PIN);
  // send data
  for(i=0; i<GRAM_BYTES; i++) {
    spi_write(OLED_SPI, screenBuf[i]);
  }
  spi_unselectChip(OLED_SPI, OLED_SPI_NPCS); 
}



 // clear OLED RAM and local screenbuffer
void screen_clear(void) {
  spi_selectChip(OLED_SPI, OLED_SPI_NPCS);
  // pull register select high to write data
  gpio_set_gpio_pin(OLED_REGISTER_PIN);
  for(i=0; i<GRAM_BYTES; i++) { 
    screenBuf[i] = 0;
    //    write_data(0);
    spi_write(OLED_SPI, 0);
  }
  spi_unselectChip(OLED_SPI, OLED_SPI_NPCS);
}
