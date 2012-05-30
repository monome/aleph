/* main.c
 * 
 * avr32 interface board test
 */

#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
#include "intc.h"
#include "spi.h"
//#include "usart.h"

#include "config.h"
#include "init.h"
#include "oled.h"

//-----------------------------
//---- static variables
static U16 whichpix = 0;
static U8 alpha = 0;

//-----------------------------
//---- static functions


// interrupt handler for PB00-PB07
static void int_handler_port1_line0(void);
static void int_handler_port1_line0(void) {
  /*
  if(gpio_get_pin_interrupt_flag(BFIN_HWAIT_PIN)) {
    hwait = gpio_get_pin_value(BFIN_HWAIT_PIN);
    gpio_clear_pin_interrupt_flag(BFIN_HWAIT_PIN);
  }
  */
}



// register interrupts
static void register_interrupts(void);
static void register_interrupts(void) {
  /*
  // generate an interrupt when bfin HWAIT changes
  gpio_enable_pin_interrupt( BFIN_HWAIT_PIN, GPIO_PIN_CHANGE);
  // assign interrupt handler
  INTC_register_interrupt( &int_handler_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), AVR32_INTC_INT1 );
  */
}


// main function
int main(void) {  
  U64 delay;
  
  init_avr();
  init_oled();
  //  register_interrupts();

  while (true) {

    oled_draw_pixel_raw(whichpix, alpha);
    oled_refresh();
    alpha = (alpha + 1) % 0x0f;
    whichpix = (whichpix + 1 ) % NPIXELS;
    delay=1000;
    while(--delay) { ;; }
  }
 
  return 0;
}
