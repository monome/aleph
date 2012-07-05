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
#include "fat.h"
#include "file.h"
#include "gpio.h"
#include "intc.h"
#include "navigation.h"
#include "pdca.h"
#include "power_clocks_lib.h"
//#include "preprocessor.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"
// aleph
#include "conf_aleph.h"
#include "init.h"

//! buffer for filename
static char str_buf[MAX_FILE_PATH_LENGTH];

// Used to indicate the end of PDCA transfer
volatile bool end_of_transfer;

//=============================
//===== static functions

// Software wait
static void wait(void);
void wait(void)
{
  volatile int i;
  for(i = 0 ; i < 5000; i++);
}

/// register interrutps 
//static void register_interrupts(void); 

////main function
int main (void) {
  //uint32_t tmp;

  // switch to osc0 for main clock
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); 

  // Initialize RS232 shell text output.
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

  ///// LS  
  // Reset navigators .
  nav_reset();
  // Use the last drive available as default.
  nav_drive_set(nav_drive_nb() - 1);
  // Mount it.
  nav_partition_mount();

  // Get the volume name
  nav_dir_name((FS_STRING)str_buf, MAX_FILE_PATH_LENGTH);
  // Display general informations (drive letter and current path)
  print(DBG_USART, "\r\nVolume is ");
  print_char(DBG_USART, 'A' + nav_drive_get());
  print(DBG_USART, ":\r\nDir name is ");
  print(DBG_USART, str_buf);
  print(DBG_USART, CRLF);
  // Try to sort items by folders
  if (!nav_filelist_first(FS_DIR))
    {
      // Sort items by files
      nav_filelist_first(FS_FILE);
    }
  // Display items informations
  print(DBG_USART, "\tSize (Bytes)\tName\r\n");
  // reset filelist before to start the listing
  nav_filelist_reset();
  // While an item can be found
  while (nav_filelist_set(0, FS_FIND_NEXT))
    {
      // Get and display current item informations
      print(DBG_USART, (nav_file_isdir()) ? "Dir\t" : "   \t");
      print_ulong(DBG_USART, nav_file_lgt());
      print(DBG_USART, "\t\t");
      nav_file_name((FS_STRING)str_buf, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
      print(DBG_USART, str_buf);
      print(DBG_USART, CRLF);
      
      // Open the file.
      file_open(FOPEN_MODE_R);
      // While the end isn't reached
      while (!file_eof())
	{
	  // Display next char from file.
	  print_char(DBG_USART, file_getc());
	}
      // Close the file.
      file_close();
      print(DBG_USART, CRLF);

    }
  // Display the files number
  print_ulong(DBG_USART, nav_filelist_nb(FS_FILE));
  print(DBG_USART, "  Files\r\n");
  // Display the folders number
  print_ulong(DBG_USART, nav_filelist_nb(FS_DIR));
  print(DBG_USART, "  Dir\r\n");

}
