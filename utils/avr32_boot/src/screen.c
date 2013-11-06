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
#include "gpio.h"
#include "util.h"
#include "intc.h"
// #include "print_funcs.h"
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
const U8 lines[CHAR_ROWS] = { 0, 8, 16, 24, 32, 40, 48, 56 };

// screen buffer
static U8 screen[GRAM_BYTES];
// common static vars
static u8 x, y, i, j;
static u32 pos;
static char buf[FIX_DIG_TOTAL];

//-----------------------------
//---- static functions
static void write_data(U8 c);
static void write_data(U8 c) {
  spi_selectChip(OLED_SPI, OLED_SPI_NPCS);
  // pull register select high to write data
  gpio_set_gpio_pin(OLED_REGISTER_PIN);
  spi_write(OLED_SPI, c);
  spi_unselectChip(OLED_SPI, OLED_SPI_NPCS);
}

static void write_command(U8 c);
static void write_command(U8 c) {
  spi_selectChip(OLED_SPI, OLED_SPI_NPCS);
  // pull register select low to write a command
  gpio_clr_gpio_pin(OLED_REGISTER_PIN);
  spi_write(OLED_SPI, c);
  spi_unselectChip(OLED_SPI, OLED_SPI_NPCS);
}

// fill one column in a line of text with blank pixels (for spacing)
static void zero_col(U16 x, U16 y);
static void zero_col(U16 x, U16 y) {
  static U8 i;
  for(i=0; i<FONT_CHARH; i++) {
    screen_pixel(x, y+i, 0);
  }
}

//------------------
// external functions
void init_oled(void) {
  U32 i;
  volatile u64 delay;
  //  cpu_irq_disable();
  Disable_global_interrupt();
  delay = FCPU_HZ >> 10 ; while(delay > 0) { delay--; }
  // flip the reset pin
  gpio_set_gpio_pin(OLED_RESET_PIN);
  //  delay_ms(1);
  delay = FCPU_HZ >> 10 ; while(delay > 0) { delay--; }
  gpio_clr_gpio_pin(OLED_RESET_PIN);
  // delay_ms(1);
  delay=FCPU_HZ >> 10; while(delay > 0) { delay--; }
  gpio_set_gpio_pin(OLED_RESET_PIN);
  //delay_ms(10);
  delay = FCPU_HZ >> 8; while(delay > 0) { delay--; }
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

  // clear OLED RAM and local screenbuffer
  for(i=0; i<GRAM_BYTES; i++) { 
    screen[i] = 0;
    write_data(0);
  }
  write_command(0xAF);	// on

  //  delay_ms(10) 
  delay = FCPU_HZ >> 8; while(delay > 0) { delay--; }
  //  cpu_irq_enable();
  Enable_global_interrupt();
}


// send screen buffer contents to OLED
void screen_refresh(void) {
  U16 i;
  u8* pScreen;

  /// FIXME: use specific IRQ levels
  cpu_irq_disable();

  // set update box (to full screen)
  write_command(0x15);
  write_command(0);
  write_command(63);
  write_command(0x75);
  write_command(0);
  write_command(63);

  // pull register select high to write data
  gpio_set_gpio_pin(OLED_REGISTER_PIN);
  spi_selectChip(OLED_SPI, OLED_SPI_NPCS);

  pScreen=&(screen[GRAM_BYTES_1]);
  for(i=0; i<GRAM_BYTES; i++) {     
    spi_write(OLED_SPI, *pScreen);
    pScreen--;
  }
  spi_unselectChip(OLED_SPI, OLED_SPI_NPCS);
  cpu_irq_enable();
}

// draw a single pixel
void screen_pixel(U16 x, U16 y, U8 a) {
  pos = (y << COLS_LSHIFT) + (x >> 1);

 // rotate: swap (and read backwards in refresh)
  if (x&1) {
    screen[pos] &= 0xf0;
    screen[pos] |= (a & 0x0f);
  } else {
    screen[pos] &= 0x0f;
    screen[pos] |= (a << 4);
  }
}

// get value of pixel
U8 screen_get_pixel(U8 x, U8 y) {
  pos = (y << COLS_LSHIFT) + (x>>1);
  if (x&1) {
    return screen[pos] & 0x0f; 
   } else {
    return (screen[pos] & 0xf0) >> 4;
  }
}

// draw a single character glyph with fixed spacing
U8 screen_char_fixed(U16 col, U16 row, char gl, U8 a) {
  //  static U8 x;
  for(j=0; j<FONT_CHARH; j++) {
    for(i=0; i<FONT_CHARW; i++) {
      if( (font_data[gl - FONT_ASCII_OFFSET].data[i] & (1 << j))) {
	screen_pixel(i+col, j+row, a);
      } else {
	screen_pixel(i+col, j+row, 0);
      }
    }
  }
  return x+1;
}

// draw a single character glyph with proportional spacing
U8 screen_char_squeeze(U16 col, U16 row, char gl, U8 a) {
  //  static U8 x;
  static U8 xnum;
  static const glyph_t * g;
  g = &(font_data[gl - FONT_ASCII_OFFSET]);
  xnum = FONT_CHARW - g->first - g->last;
  //  // print_dbg("\r\n char at row: ");
  //  // print_dbg_ulong(row);
  for(j=0; j<FONT_CHARH; j++) {
    for(i=0; i<xnum; i++) {
      if( (g->data[i + g->first] & (1 << j))) {
	screen_pixel(i + col, j + row, a);
      } else {
	screen_pixel(i + col, j + row, 0);
      }
    }
  }
  return xnum;
}

// draw a string with fixed spacing
U8 screen_string_fixed(U16 x, U16 l, char *str, U8 a) {
  //  static u8 y;
  y = lines[l];
  while(*str != 0) {
    x += screen_char_fixed(x, y, *str, a) + 1;
    str++;
  }
  return x;
}

// draw a string with proportional spacing
U8 screen_string_squeeze(U16 x, U16 l, char *str, U8 a) {
  //  static u8 y;
  y = lines[l];
  while(*str != 0) {
    x += screen_char_squeeze(x, y, *str, a);
    zero_col(x, y);
    // extra pixel... TODO: maybe variable spacing here
    x++;
    str++;
  }
  refresh = 1;
  return x;
}

// draw a string (default) 
inline U8 screen_string(U16 x, U16 l, char *str, U8 a) {
  return screen_string_squeeze(x, l, str, a);
}

// print a formatted integer
U8 screen_int(U16 x, U16 l, S16 i, U8 a) {
  //  static char buf[32];
  //  snprintf(buf, 32, "%d", (int)i);
  //  static char buf[FIX_DIG_TOTAL];
  //snprintf(buf, 32, "%.1f", (float)f);
  //  print_fix16(buf, (u32)i << 16 );
  itoa_whole(i, buf, 5);
  //buf = ultoa(int);
  return screen_string_squeeze(x, l, buf, a);
}

// print a formatted float
/*
U8 screen_float(U16 x, U16 y, F32 f, U8 a) {
  static char buf[32];
  snprintf(buf, 32, "%.1f", (float)f);
  return screen_string_squeeze(x, y, buf, a);
}
*/

// print a formatted fix_t
U8 screen_fix(U16 x, U16 l, fix16_t v, U8 a) {
  static char buf[FIX_DIG_TOTAL];
  //snprintf(buf, 32, "%.1f", (float)f);
  print_fix16(buf, v);
  return screen_string_squeeze(x, l, buf, a);
}

// fill a line with blank space to end
void screen_blank_line(U16 x, U16 l) {
  U8 i, j;
  const u8 y = lines[l];
  for(i=x; i<NCOLS; i++) {
    for(j=y; j<(FONT_CHARH + y); j++) {
      screen_pixel(i, j, 0);
    }
  }
 }

// highlight a line
void screen_hl_line(U16 x, U16 l, U8 a) {
  U8 i, j;
  const u8 y = lines[l];
  for(i=x; i<NCOLS; i++) {
    for(j=y; j<(y+FONT_CHARH); j++) {
      if (screen_get_pixel(i, j) == 0) {
	screen_pixel(i, j, a);
      }
    }
  }
}

// draw a line and blank to end
// argument is line number
U8 screen_line(U16 x, U16 l, char *str, U8 hl) {
  x = screen_string(x, l, str, hl);
  screen_blank_line(x, l);
  //// test
  //  // print_dbg("\r\n");
  //  // print_dbg(str);
  refresh = 1;
  return NCOLS;
}

void screen_clear(void) {
  u16 i;
 // clear OLED RAM and local screenbuffer
  for(i=0; i<GRAM_BYTES; i++) { 
    screen[i] = 0;
    //    write_data(0);
  }
}


/* // fill graphics ram with a test pattern */
/* void screen_test_fill(void) { */
/*   u32 i; */
/*   u32 x=0; */
/*   u32 l=5; */

/*   screen_line(0, 0, "_ LINE 1 ! ! ! ! ", 0xf); */
/*   screen_line(0, 1, "_ LINE 2 @ @ @ @ ", 0xf); */
/*   screen_line(0, 2, "_ LINE 3 # # # # ", 0xf); */
/*   screen_line(0, 3, "_ LINE 4 $ $ $ $ ", 0xf); */
/*   screen_line(0, 4, "_ LINE 5 % % % % ", 0xf); */

/*   for(i=0; i<font_nglyphs; i++) { */
/*     x = x + screen_char_squeeze(x, l, i + FONT_ASCII_OFFSET, 0xf); */
/*     x++; */
/*     if (x > NCOLS) { */
/*       x -= NCOLS; */
/*       l++; */
/*     } */
/*   } */
/*   refresh = 1; */
/* } */
