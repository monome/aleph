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
#include "delay.h"
#include "gpio.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "sd_mmc_spi.h"
#include "spi.h"
#include "tc.h"
#include "usart.h"
// aleph
#include "conf_aleph.h"
#include "init.h"

#define FASTCLOCK 1

//===========================
//==== static variables

// Dummy char table
const char dummy_data[] =
#include "dummy.h"
  ;

// Local RAM buffer for the example to store data received from the SD/MMC card
volatile char ram_buffer[1000];

// PDCA Channel pointer
volatile avr32_pdca_channel_t* pdca_channelrx ;
volatile avr32_pdca_channel_t* pdca_channeltx ;

//===================================
//==== external functions

// initialize other GPIO
void init_gpio(void) {
  gpio_enable_pin_pull_up(ENC0_S0_PIN);
  gpio_enable_pin_pull_up(ENC0_S1_PIN);
  gpio_enable_pin_pull_up(ENC1_S0_PIN);
  gpio_enable_pin_pull_up(ENC1_S1_PIN);
  gpio_enable_pin_pull_up(ENC2_S0_PIN);
  gpio_enable_pin_pull_up(ENC2_S1_PIN);
  gpio_enable_pin_pull_up(ENC3_S0_PIN);
  gpio_enable_pin_pull_up(ENC3_S1_PIN);

  gpio_enable_pin_pull_up(SW0_PIN);
  gpio_enable_pin_pull_up(SW1_PIN);
  gpio_enable_pin_pull_up(SW2_PIN);
  gpio_enable_pin_pull_up(SW3_PIN);
}

// initialize debug USART
void init_dbg_usart (void) {
  // GPIO map for USART.
  static const gpio_map_t DBG_USART_GPIO_MAP = {
    { DBG_USART_RX_PIN, DBG_USART_RX_FUNCTION },
    { DBG_USART_TX_PIN, DBG_USART_TX_FUNCTION }
  };
  
  // Options for USART.
  static const usart_options_t DBG_USART_OPTIONS = {
    .baudrate = DBG_USART_BAUDRATE,
    .charlength = 8,
    .paritytype = USART_NO_PARITY,
    .stopbits = USART_1_STOPBIT,
    .channelmode = USART_NORMAL_CHMODE
  };

  // Set up GPIO for DBG_USART
  gpio_enable_module(DBG_USART_GPIO_MAP,
                     sizeof(DBG_USART_GPIO_MAP) / sizeof(DBG_USART_GPIO_MAP[0]));

  // Initialize in RS232 mode.
  usart_init_rs232(DBG_USART, &DBG_USART_OPTIONS, FPBA_HZ);
}

// initialize OLED USART in SPI mode
extern void init_oled_usart (void) {
  static const gpio_map_t USART_SPI_GPIO_MAP = {
    {OLED_USART_SPI_SCK_PIN,  OLED_USART_SPI_SCK_FUNCTION },
    {OLED_USART_SPI_MISO_PIN, OLED_USART_SPI_MISO_FUNCTION},
    {OLED_USART_SPI_MOSI_PIN, OLED_USART_SPI_MOSI_FUNCTION},
    {OLED_USART_SPI_NSS_PIN,  OLED_USART_SPI_NSS_FUNCTION }
  };

  // SPI options for OLED
  static const usart_spi_options_t USART_SPI_OPTIONS = {
    .baudrate     = 3300000, // seems about maximum
    .charlength   = 8,
    .spimode      = 3, // clock starts high, sample on rising edge
    .channelmode  = USART_NORMAL_CHMODE
  };

  // Assign GPIO to SPI.
  gpio_enable_module(USART_SPI_GPIO_MAP,
		     sizeof(USART_SPI_GPIO_MAP) / sizeof(USART_SPI_GPIO_MAP[0]));
  // Initialize USART in SPI mode from OSC0
  usart_init_spi_master(OLED_USART_SPI, &USART_SPI_OPTIONS, FOSC0);
  delay_ms(10);

}

// initialize application timer
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

// initialize sd/mms resources: SPI. GPIO, SD_MMC
void init_sd_mmc_resources(void) {
  // GPIO pins used for SD/MMC interface
  static const gpio_map_t SD_MMC_SPI_GPIO_MAP = {
    { SD_MMC_SPI_SCK_PIN, SD_MMC_SPI_SCK_FUNCTION }, // SPI Clock.
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
  sd_mmc_spi_init(spiOptions, FPBA_HZ);
}

// init PDCA (Peripheral DMA Controller A) resources for the SPI transfer and start a dummy transfer
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

// intialize resources for bf533 communication: SPI, GPIO
void init_bfin_resources(void) {
  static const gpio_map_t BFIN_SPI_GPIO_MAP = {
    { BFIN_SPI_SCK_PIN, BFIN_SPI_SCK_FUNCTION },
    { BFIN_SPI_MISO_PIN, BFIN_SPI_MISO_FUNCTION },
    { BFIN_SPI_MOSI_PIN, BFIN_SPI_MOSI_FUNCTION },
    { BFIN_SPI_NPCS_PIN, BFIN_SPI_NPCS_FUNCTION }
  };
  
  spi_options_t spiOptions = {
    .reg          = BFIN_SPI_NPCS,
    .baudrate     = 2000000,
    .bits         = 8,
    .spck_delay   = 0,
    //  .trans_delay  = 0,
    .trans_delay  = 1,
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


// initialize clocks
void init_clocks(void) {
  /// from 251e...
  // Switch to OSC0 to speed up  booting
  // Configure Osc0 in crystal mode (i.e. use of an external crystal source, with
  // frequency FOSC0) with an appropriate startup time then switch the main clock
  // source to Osc0.
  pm_switch_to_osc0( &AVR32_PM, FOSC0, OSC0_STARTUP );

  // Set PLL0 (fed from OSC0 = 12 MHz) to 132 MHz
  pm_pll_setup( &AVR32_PM,
		0,  // pll.
		10,  // mul.
		1,   // div.
		0,   // osc.
		16 ); // lockcount.

  // Set PLL operating range and divider (fpll = fvco/2)
  // -> PLL0 output = 66 MHz
  pm_pll_set_option( &AVR32_PM,
		     0, // pll.
		     1,  // pll_freq.
		     1,  // pll_div2.
		     0 ); // pll_wbwdisable.

  // start PLL0 and wait for the lock
  pm_pll_enable( &AVR32_PM, 0 );
  pm_wait_for_pll0_locked( &AVR32_PM );

  // By default, all peripheral clocks to run at master clock rate

  // Set one waitstate for the flash.  Necessary for > 33MHz CPU freq.
  flashc_set_wait_state( 1 );

  // Switch to PLL0 as the master clock
  pm_switch_to_clock( &AVR32_PM, AVR32_PM_MCCTRL_MCSEL_PLL0) ;
  

  /*
#if FASTCLOCK
  struct pll_config pllcfg;
#endif
  // default: conf_clock.h
  // also disables non-essential peripheral clocks
  sysclk_init();
  
  ///// re-enable desired peripheral clocks
  // interrupt controller
  sysclk_enable_pba_module(SYSCLK_INTC);
  // gpio
  sysclk_enable_pba_module(SYSCLK_GPIO);
  // PDCA
  sysclk_enable_pba_module(SYSCLK_PDCA_PB);
  // SPI
  sysclk_enable_pba_module(SYSCLK_SPI0);
  sysclk_enable_pba_module(SYSCLK_SPI1);
  // tc
  sysclk_enable_peripheral_clock(APP_TC);
  // usart
  sysclk_enable_pba_module(SYSCLK_USART1);
  sysclk_enable_pba_module(SYSCLK_USART2);

#if FASTCLOCK
  // use phase-lock-loop for master clock source
  
  // PLL src : OSC0
  // div     : 1
  // mul     : PLL_OUTPUT_FREQ / BOARD_OSC0_HZ
  pll_config_init(&pllcfg, PLL_SRC_OSC0, 1,
		  PLL_OUTPUT_FREQ / BOARD_OSC0_HZ);
  pll_enable(&pllcfg, 0);
  pll_wait_for_lock(0);
  // prescalers:
  // CPU and HSB : PLL / 2
  // PBA         : PLL / 4
  // PBB         : PLL / 2
  sysclk_set_prescalers(1, 2, 1);
  
  // set flash wait before changing main clock
  flash_set_bus_freq(PLL_OUTPUT_FREQ / 2);
  
  // Switch the main clock to PLL0.
  sysclk_set_source(SYSCLK_SRC_PLL0);
#endif

  // intitialize millisecond delay engine
  delay_init(sysclk_get_cpu_hz());
  */
}
