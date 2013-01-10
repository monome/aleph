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

#include "ctrl_access.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "usart.h"
#include "spi.h"
#include "pdca.h"
#include "intc.h"
//#include "conf_at45dbx.h"
#include "conf_sd_mmc_spi.h"
#include "sd_mmc_spi.h"


#if 1
#include "fat.h"
#else 

#include "fat.h"
#include "file.h"
#include "navigation.h"

#endif

//---------------------------
//---- pin config

// debug with USART1
#  define DBG_USART               (&AVR32_USART1)
#  define DBG_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#  define DBG_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define DBG_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#  define DBG_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define DBG_USART_BAUDRATE      57600

#include "print_funcs.h"

#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI1_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI1_TX

#  define TARGET_PBACLK_FREQ_HZ FOSC0  // PBA clock target frequency, in Hz
//! \brief PBA clock frequency (Hz)
#define PBA_HZ                FOSC0

#define AVR32_PDCA_CHANNEL_SPI_RX 0 // In the example we will use the pdca channel 0.
#define AVR32_PDCA_CHANNEL_SPI_TX 1 // In the example we will use the pdca channel 1.
// bytes in the PDCA receive buffer when operating in slave mode
#define BUFFERSIZE            64

//! buffer for filename
static char str_buff[MAX_FILE_PATH_LENGTH];

// PDCA Channel pointer
volatile avr32_pdca_channel_t* pdca_channelrx ;
volatile avr32_pdca_channel_t* pdca_channeltx ;

// Used to indicate the end of PDCA transfer
volatile bool end_of_transfer;

// Local RAM buffer for the example to store data received from the SD/MMC card
volatile char ram_buffer[1000];

// Dummy char table
const char dummy_data[] =
#include "dummy.h"
  ;


static void local_pdca_init(void);
void wait(void);
__attribute__((__interrupt__))
static void pdca_int_handler(void);

// Software wait
void wait(void)
{
  volatile int i;
  for(i = 0 ; i < 5000; i++);
}

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

//// interrupt handler
__attribute__((__interrupt__))
static void pdca_int_handler(void)
{
  // Disable all interrupts.
  Disable_global_interrupt();

  // Disable interrupt channel.
  pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);

  sd_mmc_spi_read_close_PDCA();//unselects the SD/MMC memory.
  wait();
  // Disable unnecessary channel
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);

  // Enable all interrupts.
  Enable_global_interrupt();

  end_of_transfer = true;
}

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



/*! \brief Initialize PDCA (Peripheral DMA Controller A) resources for the SPI transfer and start a dummy transfer
 */
void local_pdca_init(void)
{
  // this PDCA channel is used for data reception from the SPI
  pdca_channel_options_t pdca_options_SPI_RX ={ // pdca channel options

    .addr = ram_buffer,
    // memory address. We take here the address of the string dummy_data. This string is located in the file dummy.h

    .size = 512,                              // transfer counter: here the size of the string
    .r_addr = NULL,                           // next memory address after 1st transfer complete
    .r_size = 0,                              // next transfer counter not used here
    .pid = AVR32_PDCA_CHANNEL_USED_RX,        // select peripheral ID - data are on reception from SPI1 RX line
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer: 8,16,32 bits
  };

  // this channel is used to activate the clock of the SPI by sending a dummy variables
  pdca_channel_options_t pdca_options_SPI_TX ={ // pdca channel options

    .addr = (void *)&dummy_data,              // memory address.
                                              // We take here the address of the string dummy_data.
                                              // This string is located in the file dummy.h
    .size = 512,                              // transfer counter: here the size of the string
    .r_addr = NULL,                           // next memory address after 1st transfer complete
    .r_size = 0,                              // next transfer counter not used here
    .pid = AVR32_PDCA_CHANNEL_USED_TX,        // select peripheral ID - data are on reception from SPI1 RX line
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer: 8,16,32 bits
  };

  // Init PDCA transmission channel
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_TX, &pdca_options_SPI_TX);

  // Init PDCA Reception channel
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_RX, &pdca_options_SPI_RX);

  //! \brief Enable pdca transfer interrupt when completed
  INTC_register_interrupt(&pdca_int_handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);  // pdca_channel_spi1_RX = 0

}

////main functions
int main (void) {
  //uint32_t tmp;

  // switch to osc0 for main clock
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP); 

  // Initialize RS232 shell text output.
  init_dbg_usart(TARGET_PBACLK_FREQ_HZ);

  // Initialize Interrupt Controller
  INTC_init_interrupts();

  // Initialize SD/MMC driver resources: GPIO, SPI and SD/MMC.
  sd_mmc_resources_init();

  // Wait for a card to be inserted
  while (!sd_mmc_spi_mem_check());
  print_dbg("\r\nCard detected!");

  // Read Card capacity
  sd_mmc_spi_get_capacity();
  print_dbg("Capacity = ");
  print_dbg_ulong(capacity >> 20);
  print_dbg(" MBytes");

  // Enable all interrupts.
  Enable_global_interrupt();

  // Initialize PDCA controller before starting a transfer
  local_pdca_init();


#if 1 
  
  fl_init();

  
  
}


#else

  ////////////////
  //////////////
  ///// LS  
  // Reset navigators .
  nav_reset();
  // Use the last drive available as default.
  nav_drive_set(nav_drive_nb() - 1);
  // Mount it.
  nav_partition_mount();

  // Get the volume name
  nav_dir_name((FS_STRING)str_buff, MAX_FILE_PATH_LENGTH);
  // Display general informations (drive letter and current path)
  print(DBG_USART, "\r\nVolume is ");
  print_char(DBG_USART, 'A' + nav_drive_get());
  print(DBG_USART, ":\r\nDir name is ");
  print(DBG_USART, str_buff);
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
      nav_file_name((FS_STRING)str_buff, MAX_FILE_PATH_LENGTH, FS_NAME_GET, true);
      print(DBG_USART, str_buff);
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

#endif
