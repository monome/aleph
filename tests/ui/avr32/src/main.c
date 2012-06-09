/* main.c
 * 
 * avr32 interface board test
 */

#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
//#include "intc.h"

#include "config.h"
#include "init.h"
#include "screen.h"
#include "font.h"

// test
#include "board.h"
#include "power_clocks_lib.h"
#include "usart.h"
#include "delay.h"

//--------------------
//--- static functions

// interrupt handler for PB00-PB07
static void int_handler_port1_line0(void);
// register interrupts
static void register_interrupts(void);
// interrupt handlers
static void handle_enc( U8 pin );
static void handle_sw( U8 pin );
// determine which pin triggered the interrupt
static inline void check_sw_pin(const U8 pin);
static inline void check_enc_pin(const U8 pin);


// interrupt handler for PB00-PB07
static void int_handler_port1_line0(void) {
  /*
  if(gpio_get_pin_interrupt_flag(BFIN_HWAIT_PIN)) {
    hwait = gpio_get_pin_value(BFIN_HWAIT_PIN);
    gpio_clear_pin_interrupt_flag(BFIN_HWAIT_PIN);
  }
  */
}

// register interrupts
static void register_interrupts(void) {
  // generate an interrupt when bfin HWAIT changes
  //gpio_enable_pin_interrupt( BFIN_HWAIT_PIN, GPIO_PIN_CHANGE);
  // assign interrupt handler
  //INTC_register_interrupt( &int_handler_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), AVR32_INTC_INT1 );
}

// main function
int main(void) {  
  U32 i, x, y;
  U8 alpha;

  init_avr();
  init_oled();

  Disable_global_interrupt();
  register_interrupts();
  Enable_global_interrupt();
  
  x = 0;
  y = 0;
  alpha = 1;
  for(i=0; i<font_nglyphs; i++) {
    x += screen_draw_char_squeeze(x, y, i + FONT_ASCII_OFFSET, alpha) + 1;
    //    x += FONT_CHARW;
    if (x >= NCOLS) {
      x = 0;
      y += FONT_CHARH;
    }
    alpha++;
    if(alpha > 0x0f) {
      alpha = 1;
    } 
  }
  y += FONT_CHARH;
  screen_draw_string_squeeze(0, y, "this is a \"STRING\" : ?!?", 0x0f);
  y += FONT_CHARH;
  x = 0;
  x += screen_draw_int(x, y, 2919, 0x0f);
  x += screen_draw_string_squeeze(x, y, "/", 0x0f);
  y += FONT_CHARH;
  x = screen_draw_float(0, y, 2919.0, 0x0f);
  y += FONT_CHARH;
  x = 0;
  x = screen_draw_string_squeeze(x, y, " this", 0x0f);
  x = screen_draw_string_squeeze(x, y, " plus?", 0x0f);
  x = screen_draw_string_squeeze(x, y, " this!", 0x0f);
  x = screen_draw_string_squeeze(x, y, " OK.", 0x0f);


  screen_refresh();
  return 0;
}

