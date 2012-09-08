/* main.c
 * 
 * avr32 -> blackfin spi boot test.
 */

#include <avr32/io.h>
#include "compiler.h"
#include "gpio.h"
#include "intc.h"
#include "spi.h"

#include "config.h"
#include "init.h"

//-----------------------------
//---- static variables

// flag to pause SPI transfer during blackfin boot
static U8 hwait = 0;

// blackfin .ldr data
static const U64 ldrBytes = 
#include "../../bfin/aleph-osc2.ldr_size.inc"
  ;
static U8 ldrData[] =  {
#include "../../bfin/aleph-osc2.ldr.inc"
};

//-----------------------------
//---- static functions

// interrupt handler for PB00-PB07
static void int_handler_port1_line0(void);
static void int_handler_port1_line0(void) {
  if(gpio_get_pin_interrupt_flag(BFIN_HWAIT_PIN)) {
    hwait = gpio_get_pin_value(BFIN_HWAIT_PIN);
    gpio_clear_pin_interrupt_flag(BFIN_HWAIT_PIN);
  }
}

// register interrupts
static void register_interrupts(void);
static void register_interrupts(void) {
  // generate an interrupt when bfin HWAIT changes
  gpio_enable_pin_interrupt( BFIN_HWAIT_PIN, GPIO_PIN_CHANGE);
  // assign interrupt handler
  INTC_register_interrupt( &int_handler_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), AVR32_INTC_INT1 );
}

// main function
int main(void) {  
  U64 delay;
  U64 byte;
  
  init_avr();
  register_interrupts();

  // reset the blackfin
  gpio_clr_gpio_pin(BFIN_RESET_PIN);
  // wait a little
  delay = 10; while (--delay > 0) {;;}
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  // wait a little more
  delay = 1000; while (--delay > 0) {;;}

  // loop over .ldr data
  byte = 0;
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  while(byte < ldrBytes) {
    // pause if hwait is held high by the blackfin
    while(hwait > 0) { ;; }
    spi_write(BFIN_SPI, ldrData[byte]);
    byte++;
  }
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    
  return 0;
}
