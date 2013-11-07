/* init.c
   aleph-avr32
   
   low-level peripheral  initialization.
*/

//ASF
#include <flashc.h>
#include <pll.h>
#include <sysclk.h>

#include "compiler.h"
#include "conf_sd_mmc_spi.h"
#include "util.h"
#include "gpio.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "sd_mmc_spi.h"
#include "spi.h"
#include "tc.h"
// aleph
//#include "conf_aleph.h"
#include "aleph_board.h"
#include "filesystem.h"
#include "init.h"

//===========================
//==== static variables
// ...

//===================================
//==== external functions

// initialize non-peripheral GPIO
void init_gpio(void) {
  
  gpio_enable_pin_pull_up(ENC0_S0_PIN);
  gpio_enable_pin_pull_up(ENC0_S1_PIN);
  gpio_enable_pin_pull_up(ENC1_S0_PIN);
  gpio_enable_pin_pull_up(ENC1_S1_PIN);
  gpio_enable_pin_pull_up(ENC2_S0_PIN);
  gpio_enable_pin_pull_up(ENC2_S1_PIN);
  gpio_enable_pin_pull_up(ENC3_S0_PIN);
  gpio_enable_pin_pull_up(ENC3_S1_PIN);

#if 0
  gpio_enable_pin_pull_up(SW0_PIN);
  gpio_enable_pin_pull_up(SW1_PIN);
  gpio_enable_pin_pull_up(SW2_PIN);
  gpio_enable_pin_pull_up(SW3_PIN);
  gpio_enable_pin_pull_up(SW_MODE_PIN);
#endif

  gpio_enable_pin_pull_up(SW_POWER_PIN);


  // test: 
  // getting some weird noise on the USART during certain peripheral uses.
  // don't really want to use the usart at all in bootloader. 
  // i am wondering if leaving the pins floating is causing spurious noise.
  // so here, hackishly treat them as GPIO and pull them to ground.

  gpio_clr_gpio_pin(AVR32_USART0_TXD_0_0_PIN);
  gpio_clr_gpio_pin(AVR32_USART0_RXD_0_0_PIN);

}

// INITIALIZE application timer
extern void init_tc (volatile avr32_tc_t *tc) {
  // waveform options
  static const tc_waveform_opt_t waveform_opt = {
    .channel  = APP_TC_CHANNEL,  // channel
    .bswtrg   = TC_EVT_EFFECT_NOOP, // software trigger action on TIOB
    .beevt    = TC_EVT_EFFECT_NOOP, // external event action
    .bcpc     = TC_EVT_EFFECT_NOOP, // rc compare action
    .bcpb     = TC_EVT_EFFECT_NOOP, // rb compare
    .aswtrg   = TC_EVT_EFFECT_NOOP, // soft trig on TIOA
    .aeevt    = TC_EVT_EFFECT_NOOP, // etc
    .acpc     = TC_EVT_EFFECT_NOOP,
    .acpa     = TC_EVT_EFFECT_NOOP,
    // Waveform selection: Up mode with automatic trigger(reset) on RC compare.
    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
    .enetrg   = false,             // external event trig
    .eevt     = 0,                 // extern event select
    .eevtedg  = TC_SEL_NO_EDGE,    // extern event edge
    .cpcdis   = false,             // counter disable when rc compare
    .cpcstop  = false,            // counter stopped when rc compare
    .burst    = false,
    .clki     = false,
    // Internal source clock 5, connected to fPBA / 128.
    .tcclks   = TC_CLOCK_SOURCE_TC5
  };

  // Options for enabling TC interrupts
  static const tc_interrupt_t tc_interrupt = {
    .etrgs = 0,
    .ldrbs = 0,
    .ldras = 0,
    .cpcs  = 1, // Enable interrupt on RC compare alone
    .cpbs  = 0,
    .cpas  = 0,
    .lovrs = 0,
    .covfs = 0
  };
  // Initialize the timer/counter.
  tc_init_waveform(tc, &waveform_opt);

  // set timer compare trigger.
  // we want it to overflow and generate an interrupt every 1 ms
  // so (1 / fPBA / 128) * RC = 0.001
  // so RC = fPBA / 128 / 1000
  tc_write_rc(tc, APP_TC_CHANNEL, (FPBA_HZ / 128 / 1000));
  // configure the timer interrupt
  tc_configure_interrupts(tc, APP_TC_CHANNEL, &tc_interrupt);
  // Start the timer/counter.
  tc_start(tc, APP_TC_CHANNEL);
}

// initialize spi1: OLED, ADC, SD/MMC
extern void init_spi1 (void) {
  
  static const gpio_map_t OLED_SPI_GPIO_MAP = {
    {OLED_SPI_SCK_PIN,  OLED_SPI_SCK_FUNCTION },
    {OLED_SPI_MISO_PIN, OLED_SPI_MISO_FUNCTION},
    {OLED_SPI_MOSI_PIN, OLED_SPI_MOSI_FUNCTION},
    {OLED_SPI_NPCS0_PIN,  OLED_SPI_NPCS0_FUNCTION },
    {OLED_SPI_NPCS1_PIN,  OLED_SPI_NPCS1_FUNCTION },
    {OLED_SPI_NPCS2_PIN,  OLED_SPI_NPCS2_FUNCTION },
  };

  // SPI options for OLED
  spi_options_t spiOptions = {
    .reg = OLED_SPI_NPCS,
    .baudrate     = 40000000,
    .bits = 8,
    .trans_delay = 0,
    .spck_delay = 0,
    .stay_act = 1,
    .spi_mode = 3,
    .modfdis = 1
  };

  // Assign GPIO to SPI.
  gpio_enable_module(OLED_SPI_GPIO_MAP,
		     sizeof(OLED_SPI_GPIO_MAP) / sizeof(OLED_SPI_GPIO_MAP[0]));
  // Initialize as master.
  spi_initMaster(OLED_SPI, &spiOptions);

  // Set SPI selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(OLED_SPI, 0, 0, 0);

  // Enable SPI module.
  spi_enable(OLED_SPI);

  // setup chip register for OLED
  spi_setupChipReg( OLED_SPI, &spiOptions, FPBA_HZ );

  // add SD/MMC chip register
  spiOptions.reg         = SD_MMC_SPI_NPCS;
  spiOptions.baudrate    = SD_MMC_SPI_MASTER_SPEED; // Defined in conf_sd_mmc_spi.h;
  spiOptions.bits        = SD_MMC_SPI_BITS; // Defined in conf_sd_mmc_spi.h;
  spiOptions.spck_delay  = 0;
  spiOptions.trans_delay = 0;
  spiOptions.stay_act    = 1;
  spiOptions.spi_mode    = 0;
  spiOptions.modfdis     = 1;

  // Initialize SD/MMC driver with SPI clock (PBA).
  sd_mmc_spi_init(spiOptions, FPBA_HZ);
}


// init PDCA (Peripheral DMA Controller A) resources for the SPI transfer and start a dummy transfer
void init_local_pdca(void)
{
  // PDCA channel for SPI RX
  pdca_channel_options_t pdca_options_SPI_RX ={ // pdca channel options
    .addr = (void *)&pdcaRxBuf,
    .size = FS_BUF_SIZE,                      // transfer size
    .r_addr = NULL,                           // next memory address after 1st transfer complete
    .r_size = 0,                              // next transfer counter not used here
    .pid = AVR32_PDCA_CHANNEL_USED_RX,        // select peripheral ID - SPI1 RX
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer: 8,16,32 bits
  };

  // PDCA channel for SPI TX
  pdca_channel_options_t pdca_options_SPI_TX ={ // pdca channel options
    .addr = (void *)&pdcaTxBuf,               // memory address.
    .size = FS_BUF_SIZE,                      // transfer size
    .r_addr = NULL,                           // next memory address after 1st transfer complete
    .r_size = 0,                              // next transfer counter not used here
    .pid = AVR32_PDCA_CHANNEL_USED_TX,        // select peripheral ID - SPI1 TX
    .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the transfer: 8,16,32 bits
  };

  // Init PDCA transmission channel
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_TX, &pdca_options_SPI_TX);

  // Init PDCA Reception channel
  pdca_init_channel(AVR32_PDCA_CHANNEL_SPI_RX, &pdca_options_SPI_RX);
}

// intialize resources for bf533 communication: SPI, GPIO
void init_bfin_resources(void) {
  static const gpio_map_t BFIN_SPI_GPIO_MAP = {
    { BFIN_SPI_SCK_PIN, BFIN_SPI_SCK_FUNCTION },
    { BFIN_SPI_MISO_PIN, BFIN_SPI_MISO_FUNCTION },
    { BFIN_SPI_MOSI_PIN, BFIN_SPI_MOSI_FUNCTION },
    { BFIN_SPI_NPCS0_PIN, BFIN_SPI_NPCS0_FUNCTION },
  };
  
  spi_options_t spiOptions = {
    .reg          = BFIN_SPI_NPCS,
     .baudrate     = 20000000,
    .bits         = 8,
    .spck_delay   = 0,
    .trans_delay = 20,
    .stay_act     = 1,
    .spi_mode     = 1,
    .modfdis      = 1
  };

  // assign pins to SPI.
  gpio_enable_module(BFIN_SPI_GPIO_MAP,
		     sizeof(BFIN_SPI_GPIO_MAP) / sizeof(BFIN_SPI_GPIO_MAP[0]));

  // intialize as master
  spi_initMaster(BFIN_SPI, &spiOptions);

  // set selection mode: variable_ps, pcs_decode, delay.
  spi_selectionMode(BFIN_SPI, 0, 0, 0);

  // enable SPI.
  spi_enable(BFIN_SPI);

  // intialize the chip register
  spi_setupChipReg(BFIN_SPI, &spiOptions, FPBA_HZ);
  // enable pulldown on bfin HWAIT line
  //// shit! not implemented... 
  // gpio_enable_pin_pull_down(BFIN_HWAIT_PIN);
  
  // enable pullup on bfin RESET line
  gpio_enable_pin_pull_up(BFIN_RESET_PIN);
}
