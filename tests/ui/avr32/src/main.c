/* main.c
 * 
 * avr32 interface board test
 */

#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
//#include "intc.h"

#include "config.h"
//#include "init.h"
//#include "oled.h"

// test
#include "board.h"
#include "power_clocks_lib.h"
#include "usart.h"
#include "delay.h"

//-----------------------------
//---- defines
// number of columns
#define NCOLS 128
#define NCOLS_2 64
// number of rows
#define NROWS 64
#define NROWS_2 32
// pixels in graphics ram 
#define GRAM_PIX 8192 // ncols * nrows
// bytes in graphics RAM
#define GRAM_BYTES 4096 // 2 pixels per byte

//-----------------------------
//---- static variables
static U8 alpha = 0;
static U8 data = 0; // 2 pixels, i guess

  static const gpio_map_t USART_SPI_GPIO_MAP = {
    {OLED_USART_SPI_SCK_PIN,  OLED_USART_SPI_SCK_FUNCTION },
    {OLED_USART_SPI_MISO_PIN, OLED_USART_SPI_MISO_FUNCTION},
    {OLED_USART_SPI_MOSI_PIN, OLED_USART_SPI_MOSI_FUNCTION},
    {OLED_USART_SPI_NSS_PIN,  OLED_USART_SPI_NSS_FUNCTION }
  };
  
  static const usart_spi_options_t USART_SPI_OPTIONS = {
    .baudrate     = 60000,
    .charlength   = 8,
    .spimode      = 3, // clock starts high, sample on rising edge
    .channelmode  = USART_NORMAL_CHMODE
  };
  
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

// main function
int main(void) {  
  U32 i, j;

//init_avr();
  //init_oled();
  //  register_interrupts();

  pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
  delay_init(FOSC0);

  // Assign GPIO to SPI.
  gpio_enable_module(USART_SPI_GPIO_MAP,
		     sizeof(USART_SPI_GPIO_MAP) / sizeof(USART_SPI_GPIO_MAP[0]));

  // Initialize USART in SPI mode.
  usart_init_spi_master(OLED_USART_SPI, &USART_SPI_OPTIONS, FOSC0);
  
  delay_ms(10);
  
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
  
  // checkerboard, hopefully
  for (i=0; i<NROWS_2; i++) {
    for (j=0; j<NCOLS_2; j++) {
      data = 0x0f & alpha;
      write_data(data);
      delay_ms(10);
    }
    for (j=0; j<NCOLS_2; j++) {
      data = (alpha << 4) & 0xf0;
      write_data(data);
      delay_ms(10);
    }
    alpha++;
    alpha &= 0x0f;
  }

  /* 
  for(pix = 0; pix < ; pix++) {
    data = 0;
    data |= alpha;
    alpha = (alpha + 1) % 0x0f;
    data |= (alpha << 4);
    
    write_data(data);
    delay_ms(50);
  */
    /*
    usart_spi_selectChip(OLED_USART_SPI);
    usart_putchar(OLED_USART_SPI, 0x0f);
    //    usart_putchar(OLED_USART_SPI, 0x08);
    usart_spi_unselectChip(OLED_USART_SPI);
    */

    /*
    oled_draw_pixel_raw(whichpix, alpha);
    oled_refresh();
    alpha = (alpha + 1) % 0x0f;
    whichpix = (whichpix + 1 ) % NPIXELS;
    delay=1000;
    while(delay > 0) { delay--; }
    */


 
  return 0;
}

