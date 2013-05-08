/* main.c
 * avr32
 * aleph
 *
 */

// ASF
#include "compiler.h"
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "ctrl_access.h"
#include "delay.h"
#include "flashc.h"
#include "intc.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "pm.h"
#include "gpio.h"
#include "sd_mmc_spi.h"
#include "smc.h"
#include "sysclk.h"

//// aleph
// bees
#include "menu.h"
#include "net.h"
#include "net_protected.h"
#include "preset.h"
#include "scene.h"
// common
#include "files.h"
#include "fix.h"
#include "param_common.h"
#include "screen.h"
#include "simple_string.h"
#include "types.h"
#include "util.h"
// avr32
#include "aleph_board.h"
#include "adc.h"
#include "app_timers.h"
#include "bfin.h"
#include "encoders.h"
#include "events.h"
#include "filesystem.h"
#include "flash.h"
#include "font.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"
#include "memory.h"
#include "switches.h"
#include "timers.h"






// top-level peripheral init
static void init_avr32(void) {
  volatile avr32_tc_t *tc = APP_TC;
  // clocks
  // setup clocks
  sysclk_init();
  // why need here?
  sysclk_enable_pbb_module(SYSCLK_SMC_REGS);
  flashc_set_bus_freq(FCPU_HZ);
  flashc_set_wait_state(1);

  /// interrupts
  irq_initialize_vectors();

  // disable all interrupts for now
  cpu_irq_disable();
  // serial usb
  init_ftdi_usart();
  // external sram
  smc_init(FHSB_HZ);
  // initialize spi1: OLED, ADC, SD/MMC
  init_spi1();
  // initialize PDCA controller
  init_local_pdca();
  // initialize blackfin resources
  init_bfin_resources();
  // initialize application timer
  init_tc(tc);
  // initialize other GPIO
  init_gpio();
  // register interrupts
  register_interrupts();
  // initialize the OLED screen
  init_oled();
  // enable interrupts
  cpu_irq_enable();

  // usb host controller
  init_usb_host();
  
  print_dbg("\r\n avr32 init done ");
}

// control / network / logic init
static void init_ctl(void) {
  // disable interrupts
    cpu_irq_disable();

  // intialize the event queue
  init_events();
  print_dbg("\r\n init_events");

  // intialize encoders
  init_encoders();
  print_dbg("\r\n init_encoders");

  //memory manager
  init_mem();  
  print_dbg("\r\n init_mem");

  // send ADC config
  init_adc();
  print_dbg("\r\n init_adc");

  // start application timers
  init_app_timers();
  print_dbg("\r\n init_timers");
  
  //// BEES:
  net_init();
  print_dbg("\r\n net_init");

  preset_init();
  print_dbg("\r\n preset_init");

  scene_init();
  print_dbg("\r\n scene_init");

  menu_init();
  print_dbg("\r\n menu_init");

  // enable interrupts
  cpu_irq_enable();
}


//int main(void) {
////main function
int main (void) {

  // Switch main clock to external oscillator 0 (crystal).
  //  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);




  // Initialize the debug USART module.
  //  init_dbg_rs232(FOSC0);

  init_avr32();

 

  test_flash();


  while (true);

}
