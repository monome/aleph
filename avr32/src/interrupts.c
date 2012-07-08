// ASF
#include "compiler.h"
#include "gpio.h"
#include "intc.h"
#include "pdca.h"
#include "sd_mmc_spi.h"
// aleph
#include "bfin.h"
#include "conf_aleph.h"
#include "interrupts.h"


//------------------------
//----- variables
// end of PDCA transfer
volatile bool end_of_transfer;

//----------------------
//---- static functions 
// interrupt handlers

// irq for pdca (sdcard)
__attribute__((__interrupt__))
static void irq_pdca(void);

// irq for app timer
__attribute__((__interrupt__))
static void irq_port0_line0(void);

// irq for PA00-PA07
__attribute__((__interrupt__))
static void irq_port0_line0(void);


//---------------------------------
//----- static function definitions
__attribute__((__interrupt__))
static void irq_pdca(void) {
  volatile U16 delay;
  // Disable all interrupts.
  Disable_global_interrupt();

  // Disable interrupt channel.
  pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);

  sd_mmc_spi_read_close_PDCA();//unselects the SD/MMC memory.

  // wait (FIXME)
  delay=0; while(delay < 5000) { delay++; }

  // Disable unnecessary channel
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);

  // Enable all interrupts.
  Enable_global_interrupt();

  end_of_transfer = true;
}


// interrupt handler for PB00-PB07
__attribute__((__interrupt__))
static void irq_port0_line0(void) {
  // BFIN_HWAIT: set value
  if(gpio_get_pin_interrupt_flag(BFIN_HWAIT_PIN)) {
    hwait = gpio_get_pin_value(BFIN_HWAIT_PIN);
    gpio_clear_pin_interrupt_flag(BFIN_HWAIT_PIN);
  }
}


//-----------------------------
//---- external function definitions

// register interrupts
void register_interrupts(void) {
  // generate an interrupt when bfin HWAIT changes
  gpio_enable_pin_interrupt( BFIN_HWAIT_PIN, GPIO_PIN_CHANGE);
  
  // register IRQ for port B, 0-7
  //  INTC_register_interrupt( &irq_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), AVR32_INTC_INT1 );
  
  // register IRQ for port A, 0-7
  INTC_register_interrupt( &irq_port0_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA00 / 8), AVR32_INTC_INT1 );
  
  // register IRQ for PDCA transfer
  INTC_register_interrupt(&irq_pdca, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1); 
}
