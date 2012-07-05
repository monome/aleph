/* main,c
 * mmc_test
 * aleph
 *
 * test program for accessing FAT filesystem on SD, vi SPI
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
//#include "pdca.h"
#include "power_clocks_lib.h"
//#include "preprocessor.h"
#include "print_funcs.h"
//#include "sd_mmc_spi.h"
// aleph
#include "conf_aleph.h"
#include "files.h"
#include "init.h"
#include "interrupts.h"


//=============================
//===== static functions


/// register interrutps 
//static void register_interrupts(void); 

////main function
int main (void) {
  //uint32_t tmp;

  // switch to osc0 for main clock
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); 

  // Initialize RS232 text output.
  init_dbg_usart(PBA_HZ);

  // Initialize Interrupt Controller
  INTC_init_interrupts();

  // Initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
  init_sd_mmc_resources();

  // Initialize PDCA controller before starting a transfer
  init_local_pdca();

  // register interrupts
  register_interrupts();
  
  // Enable all interrupts.
  Enable_global_interrupt();

  // Wait for a card to be inserted
  while (!sd_mmc_spi_mem_check());
  print_dbg("\r\nCard detected!");

  // Read Card capacity
  sd_mmc_spi_get_capacity();
  print_dbg("Capacity = ");
  print_dbg_ulong(capacity >> 20);
  print_dbg(" MBytes");
  
  // set up file navigation using available drives
  init_files();

  // list files
  files_list();

  // load blackfin
  load_bfin_elf();

}
