/* main.c
 * avr32
 * aleph
 *
 */

// ASF
#include "compiler.h"
#include "board.h"
#include "conf_sd_mmc_spi.h"
#include "delay.h"
#include "flashc.h"
#include "intc.h"
#include "power_clocks_lib.h"
#include "print_funcs.h"
#include "pm.h"
#include "gpio.h"
#include "sysclk.h"
#include "twi.h"

//// aleph
// avr32
#include "aleph_board.h"
#include "conf_tc_irq.h"
#include "global.h"
#include "i2c.h"
#include "init.h"
#include "interrupts.h"

//==================================================
//====  defines

//==================================================
//====  extern variables

//==================================================
//====  static variables

//=================================================
//==== static declarations

static void init_avr32(void);

//=================================================
//==== definitons

// top-level peripheral init
static void init_avr32(void) {
  //  volatile avr32_tc_t *tc = APP_TC;
  // clocks
  // setup clocks
  sysclk_init();

  // not sure why but when need to explictly enable clock for static mem ctlr
  //  sysclk_enable_pbb_module(SYSCLK_SMC_REGS);
  flashc_set_bus_freq(FCPU_HZ);
  // need this for high-speed operation
  flashc_set_wait_state(1);

  /// interrupts
  //  print_dbg("\r\n  irq_initialize_vectors() ");
  irq_initialize_vectors();
  // disable all interrupts for init
  //  print_dbg("\r\n  cpu_irq_disable() ");
  cpu_irq_disable();

  // serial usb
  print_dbg("\r\n  init_ftdi_usart() ");
  init_ftdi_usart();

  // initialize other GPIO
  print_dbg("\r\n  init_gpio() ");
  init_gpio();

  // register interrupts
  //  print_dbg("\r\n  register_interrupts() ");
  //  register_interrupts();

  // enable interrupts
  print_dbg("\r\n  cpu_irq_enable() ");
  cpu_irq_enable();

  // i2c slave
  print_dbg("\r\n  init_i2c() ");
  init_i2c();

  // i2c master
   // print_dbg("\r\n  init_i2c_master() ");
   // init_i2c_master();
  
}

////main function
int main (void) {
  u8 tx = 50;
  //  u8 rx = 0;
  // set up avr32 hardware and peripherals
  init_avr32();

  print_dbg("\r\n enter tx loop");

    while(1) {
      // print_dbg("\r\n i2c tx: 0x");
      // print_dbg_char_hex(tx);

      // test i2c with loopback
      init_i2c_master();
      i2c_master_tx(&tx);
      // while(twi_is_busy()) {
      //   // delay_us(1);
      //   ;;
      // }
      // delay_us(250);
      init_i2c_slave();


      // delay_us(10);      
      // i2c_master_rx(&rx);
      
      // print_dbg(" , rx: 0x");
      // print_dbg_char_hex(rx);
      
      ++tx;
      delay_ms(803);
    }
      
}
