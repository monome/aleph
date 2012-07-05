/* init.c
   aleph-avr32
   
   low-level peripheral  initialization.
*/

//ASF
#include "compiler.h"
#include "conf_sd_mmc_spi.h"
#include "gpio.h"
#include "pdca.h"
#include "sd_mmc_spi.h"
#include "spi.h"
#include "usart.h"
// aleph
#include "conf_aleph.h"
#include "init.h"


// Dummy char table
const char dummy_data[] =
#include "dummy.h"
  ;

// Local RAM buffer for the example to store data received from the SD/MMC card
volatile char ram_buffer[1000];

// PDCA Channel pointer
volatile avr32_pdca_channel_t* pdca_channelrx ;
volatile avr32_pdca_channel_t* pdca_channeltx ;

// initialize debug USART
void init_dbg_usart (long pba_hz) {
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
void init_sd_mmc_resources(void) {
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
void init_local_pdca(void)
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
}
