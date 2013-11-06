#include "compiler.h"
#include "board.h"
#include "eic.h"
#include "flashc.h"
#include "pm.h"
// #include "print_funcs.h"
#include "smc.h"
#include "sysclk.h"

//!  SRAM base address
#define SRAM              ((void *)AVR32_EBI_CS1_ADDRESS)
//! SRAM size.
#define SRAM_SIZE         (1 << smc_get_cs_size(1) >> 2)

// startup routine runs before main()
int _init_startup(void);
int _init_startup(void) {

  // Get base address of SRAM module
  volatile uint32_t *sram = SRAM;

  // Switch to external oscillator 0.
  //	pm_switch_to_osc0(&AVR32_PM, FOSC0, OSC0_STARTUP);
  sysclk_init();
  sysclk_enable_pbb_module(SYSCLK_SMC_REGS);

  // Initialize debug serial line
  //	init_dbg_rs232(FOSC0);
  init_dbg_rs232(FPBA_HZ);

  // Display a header to user
  // print_dbg("\x1B[2J\x1B[H\r\nSMC Example\r\n");

  // print_dbg("Board running at ");
  //	// print_dbg_ulong(FOSC0 / 1000000);
  // print_dbg_ulong(FCPU_HZ / 1000000);
  // print_dbg(" MHz\r\n");

  // print_dbg("Initializing SRAM at address: ");
  // print_dbg_hex((unsigned long int)SRAM);
  // print_dbg(" ...");

  // Initialize the external SRAM chip.
  //	smc_init(FOSC0);
  smc_init(FHSB_HZ);
  // print_dbg("done\r\n\r\n");

  // print_dbg("Testing SRAM...\r\n");

  // Test each address location inside the chip with a write/readback
  uint32_t total_tests  = 0;
  uint32_t total_errors = 0;

  for (uint32_t total_tests = 0; total_tests < SRAM_SIZE; total_tests++) {
    sram[total_tests] = total_tests;

    if (total_tests != sram[total_tests]) {
      total_errors++;

      // print_dbg("Error at 0x");
      // print_dbg_hex((uint32_t)&sram[total_tests]);
      // print_dbg("\r\n");
    }
  }

  if (total_errors == 0) {
    // print_dbg("SRAM test successfully completed\r\n");
  }
  else {
    // print_dbg("SRAM test completed with ");
    // print_dbg_ulong(total_errors);
    // print_dbg(" errors out of ");
    // print_dbg_ulong(total_tests);
    // print_dbg(" tests\r\n");
  }

  return 0;


  /*  // Exception Vector Base Address */
  /*  extern void _evba; */

  /*  //--- setup clocks */
  /*  // wait-state for >33Mhz  */
  /*  flashc_set_wait_state( 1 ); */
  /*  // lock PLL and assign sys/peripheral clocks */
  /*  sysclk_init(); */
  /*  // enable the SMC module on PBB */
  /*  sysclk_enable_pbb_module(SYSCLK_SMC_REGS); */

  /*  // Load the Exception Vector Base Address in the corresponding system register */
  /*  Set_system_register(AVR32_EVBA, (int)&_evba); */

  /*  // Enable exceptions.  Enable_global_exception(); */

  /*  // initialize interrupt vectors */
  /*  irq_initialize_vectors(); */

  /*  init_dbg_rs232(FPBA_HZ); */
  /*  // print_dbg("\r\n _init_startup"); */

  /*  // setup static memory controller  */
  /*  smc_init(FHSB_HZ); */
 
  /* // return (dont-care) */
  /*  return 1; */

}
