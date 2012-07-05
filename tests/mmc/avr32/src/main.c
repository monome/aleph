/* main,c
 * mmc_test
 * aleph
 *
 * test program for accessing FAT filesystem on SD, vi SPI
 */

#include <string.h>
#include "compiler.h"
#include "preprocessor.h"
#include "board.h"
#include "print_funcs.h"
#include "ctrl_access.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"
#include "spi.h"
#include "conf_at45dbx.h"
#include "fat.h"
#include "file.h"
#include "navigation.h"

//---------------------------
//---- pin config

// debug with USART1
#  define DBG_USART               (&AVR32_USART1)
#  define DBG_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define DBG_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define DBG_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define DBG_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define DBG_USART_BAUDRATE      57600

#  define TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz


  /*
#define SD_MMC_SPI_MEM                          LUN_2
#define LUN_ID_SD_MMC_SPI_MEM                   LUN_ID_2
#define LUN_2_INCLUDE                           "sd_mmc_spi_mem.h"
#define Lun_2_test_unit_ready                   sd_mmc_spi_test_unit_ready
#define Lun_2_read_capacity                     sd_mmc_spi_read_capacity
#define Lun_2_wr_protect                        sd_mmc_spi_wr_protect
#define Lun_2_removal                           sd_mmc_spi_removal
#define Lun_2_usb_read_10                       sd_mmc_spi_usb_read_10
#define Lun_2_usb_write_10                      sd_mmc_spi_usb_write_10
#define Lun_2_mem_2_ram                         sd_mmc_spi_mem_2_ram
#define Lun_2_ram_2_mem                         sd_mmc_spi_ram_2_mem
#define LUN_2_NAME                              "\"SD/MMC Card over SPI\""
 */


static void init_dbg_usart (long pba_hz) {
  // GPIO map for USART.
  static const gpio_map_t DBG_USART_GPIO_MAP =
  {
    {DBG_USART_RX_PIN, DBG_USART_RX_FUNCTION},
    {DBG_USART_TX_PIN, DBG_USART_TX_FUNCTION}
  };

  // Options for USART.
  static const usart_options_t DBG_USART_OPTIONS =
  {
    .baudrate = DBG_USART_BAUDRATE,
    .charlength = 8,
    .paritytype = USART_NO_PARITY,
    .stopbits = USART_1_STOPBIT,
    .channelmode = USART_NORMAL_CHMODE
  };

  // Set up GPIO for DBG_USART, size of the GPIO map is 2 here.
  gpio_enable_module(DBG_USART_GPIO_MAP,
                     sizeof(DBG_USART_GPIO_MAP) / sizeof(DBG_USART_GPIO_MAP[0]));

  // Initialize it in RS232 mode.
  usart_init_rs232(DBG_USART, &DBG_USART_OPTIONS, pba_hz);
}

/*! \brief Initializes SD/MMC resources: GPIO, SPI and SD/MMC.
*/
static void sd_mmc_resources_init(void) {
// GPIO pins used for SD/MMC interface
static const gpio_map_t SD_MMC_SPI_GPIO_MAP = { { SD_MMC_SPI_SCK_PIN,
SD_MMC_SPI_SCK_FUNCTION }, // SPI Clock.
{ SD_MMC_SPI_MISO_PIN, SD_MMC_SPI_MISO_FUNCTION }, // MISO.
{ SD_MMC_SPI_MOSI_PIN, SD_MMC_SPI_MOSI_FUNCTION }, // MOSI.
{ SD_MMC_SPI_NPCS_PIN, SD_MMC_SPI_NPCS_FUNCTION } // Chip Select NPCS.
};

// SPI options.
spi_options_t spiOptions = {
.reg = SD_MMC_SPI_NPCS,
.baudrate = SD_MMC_SPI_MASTER_SPEED, // Defined in conf_sd_mmc_spi.h.
.bits = SD_MMC_SPI_BITS, // Defined in conf_sd_mmc_spi.h.
.spck_delay = 0, .trans_delay = 0, .stay_act = 1, .spi_mode = 0,
.modfdis = 1 };

// Assign I/Os to SPI.
gpio_enable_module(SD_MMC_SPI_GPIO_MAP, sizeof(SD_MMC_SPI_GPIO_MAP)
/ sizeof(SD_MMC_SPI_GPIO_MAP[0]));

// Initialize as master.
spi_initMaster(SD_MMC_SPI, &spiOptions);

// Set SPI selection mode: variable_ps, pcs_decode, delay.
spi_selectionMode(SD_MMC_SPI, 0, 0, 0);

// Enable SPI module.
spi_enable(SD_MMC_SPI);

// Initialize SD/MMC driver with SPI clock (PBA).
sd_mmc_spi_init(spiOptions, PBA_HZ);
}

int main (void) {
  uint32_t tmp;

  // switch to osc0 for main clock
   pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); 


  // Initialize RS232 shell text output.
  init_dbg_usart(TARGET_PBACLK_FREQ_HZ);

  // Initialize AT45DBX resources: GPIO, SPI and AT45DBX.
  at45dbx_resources_init();

  // Display memory status
  print(DBG_USART, "\r\nMemory ");

  if (mem_test_unit_ready(LUN_ID_AT45DBX_MEM) == CTRL_GOOD)
  {
    // Get and display the capacity
    mem_read_capacity(LUN_ID_AT45DBX_MEM, &tmp);
    print(DBG_USART, "OK:\t");
    print_ulong(DBG_USART, (tmp + 1) >> (20 - FS_SHIFT_B_TO_SECTOR));
    print(DBG_USART, " MB\r\n");
  }
  else
  {
    // Display an error message
    print(DBG_USART, "Not initialized: Check if memory is ready...\r\n");
  }

  // Display the prompt
  print(DBG_USART, "8===>");


}
