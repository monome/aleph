/* main,c
 * aleph-avr32
 *
 */


//ASF
#include <string.h>
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "compiler.h"
#include "ctrl_access.h"
//#include "fat.h"
//#include "file.h"
#include "gpio.h"
#include "intc.h"
//#include "navigation.h"
#include "pdca.h"
#include "power_clocks_lib.h"
//#include "preprocessor.h"
#include "print_funcs.h"
//#include "sd_mmc_spi.h"
// aleph
#include "bfin.h"
#include "conf_aleph.h"
#include "files.h"
#include "init.h"


//------------------------
//----- variables
// end of PDCA transfer
volatile bool end_of_transfer;

//----------------------
//---- static functions 

// interrupt handlers
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

/// debug
static U32 dum = 0;

// interrupt handler for PB00-PB07
__attribute__((__interrupt__))
static void irq_port1_line0(void) {
  // BFIN_HWAIT: set value
  if(gpio_get_pin_interrupt_flag(BFIN_HWAIT_PIN)) {
    if ((dum++ % 2000) == 0) { 
      gpio_tgl_gpio_pin(LED2_GPIO);
    }
    hwait = gpio_get_pin_value(BFIN_HWAIT_PIN);
    gpio_clear_pin_interrupt_flag(BFIN_HWAIT_PIN);
  }
}

// register interrupts
static void register_interrupts(void) {
  // generate an interrupt when bfin HWAIT changes
  gpio_enable_pin_interrupt( BFIN_HWAIT_PIN, GPIO_PIN_CHANGE);
  
  // register IRQ for port B, 0-7
  //  INTC_register_interrupt( &irq_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PB00 / 8), AVR32_INTC_INT1 );
  
  // register IRQ for port A, 0-7
  INTC_register_interrupt( &irq_port1_line0, AVR32_GPIO_IRQ_0 + (AVR32_PIN_PA00 / 8), AVR32_INTC_INT1 );
  
  // register IRQ for PDCA transfer
  INTC_register_interrupt(&irq_pdca, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1); 
}

////main function
int main (void) {
  U32 waitForCard = 0;

  // switch to osc0 for main clock
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); 

  // initialize Interrupt Controller
  INTC_init_interrupts();

  // disable interrupts
  Disable_global_interrupt();

  // initialize RS232 text output.
  init_dbg_usart(PBA_HZ);

  // initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
  init_sd_mmc_resources();

  // initialize PDCA controller
  init_local_pdca();

  // initialize blackfin resources
  init_bfin_resources();

  // register interrupts
  register_interrupts();
  
  // Enable all interrupts.
  Enable_global_interrupt();

  print_dbg("\r\nwaiting for SD card... ");
  // Wait for a card to be inserted

  gpio_set_gpio_pin(LED3_GPIO);
  gpio_set_gpio_pin(LED4_GPIO);
  gpio_set_gpio_pin(LED5_GPIO);  

  while (!sd_mmc_spi_mem_check()) {
    waitForCard++;
  }
  print_dbg("\r\ncard detected! ");

  // Read Card capacity
  sd_mmc_spi_get_capacity();
  print_dbg("capacity = ");
  print_dbg_ulong(capacity >> 20);
  print_dbg(" MBytes");


  // set up file navigation using available drives
  init_files();

  // list files
    files_list();

  ///// TEST:
  // load blackfin from first .ldr file in filesystem
   load_bfin_sdcard_test();
}
