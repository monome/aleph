/* ini.c
   aleph-avr32

   low-level peripheral  initialization.
*/

// ASF
#include <flashc.h>
#include <pll.h>
#include <sysclk.h>

#include "compiler.h"
#include "conf_sd_mmc_spi.h"
//#include "util.h"
#include "gpio.h"
#include "pdca.h"
#include "power_clocks_lib.h"
#include "sd_mmc_spi.h"
#include "spi.h"
#include "tc.h"
#include "uhc.h"
#include "usart.h"
// aleph
//#include "conf_aleph.h"
#include "debug.h"
#include "conf_board.h"
#include "conf_tc_irq.h"
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

    // no power-switch pullup on board
    gpio_enable_pin_pull_up(SW_POWER_PIN);
}

// initialize application timer
extern void init_tc(void) {
    volatile avr32_tc_t *tc = APP_TC;

    // waveform options
    static const tc_waveform_opt_t waveform_opt = {
        .channel = APP_TC_CHANNEL,     // channel
        .bswtrg = TC_EVT_EFFECT_NOOP,  // software trigger action on TIOB
        .beevt = TC_EVT_EFFECT_NOOP,   // external event action
        .bcpc = TC_EVT_EFFECT_NOOP,    // rc compare action
        .bcpb = TC_EVT_EFFECT_NOOP,    // rb compare
        .aswtrg = TC_EVT_EFFECT_NOOP,  // soft trig on TIOA
        .aeevt = TC_EVT_EFFECT_NOOP,   // etc
        .acpc = TC_EVT_EFFECT_NOOP,
        .acpa = TC_EVT_EFFECT_NOOP,
        // Waveform selection: Up mode with automatic trigger(reset) on RC
        // compare.
        .wavsel = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,
        .enetrg = false,            // external event trig
        .eevt = 0,                  // extern event select
        .eevtedg = TC_SEL_NO_EDGE,  // extern event edge
        .cpcdis = false,            // counter disable when rc compare
        .cpcstop = false,           // counter stopped when rc compare
        .burst = false,
        .clki = false,
        // Internal source clock 5, connected to fPBA / 128.
        .tcclks = TC_CLOCK_SOURCE_TC5
    };

    // Options for enabling TC interrupts
    static const tc_interrupt_t tc_interrupt = {
        .etrgs = 0,
        .ldrbs = 0,
        .ldras = 0,
        .cpcs = 1,  // Enable interrupt on RC compare alone
        .cpbs = 0,
        .cpas = 0,
        .lovrs = 0,
        .covfs = 0
    };
    // Initialize the timer/counter.
    tc_init_waveform(tc, &waveform_opt);

    // set timer compare trigger.
    // we want it to overflow and generate an interrupt every 1 ms
    // so (1 / fPBA / 128) * RC = 0.001
    // so RC = fPBA / 128 / 1000

    // undoing the kludge because whtever problem was necessitating it seems
    // gone now.
    //  tc_write_rc(tc, APP_TC_CHANNEL, (FPBA_HZ / 128000));

    /// this gives me most accurate results with current testing proto..
    tc_write_rc(tc, APP_TC_CHANNEL, (FPBA_HZ / 126336));

    ///// FIXME: kludge because the timing is slow somehow... ?
    ///// this constant is experimentally determined...
    //  tc_write_rc(tc, APP_TC_CHANNEL, (FPBA_HZ / 149707));

    // configure the timer interrupt
    tc_configure_interrupts(tc, APP_TC_CHANNEL, &tc_interrupt);
    // Start the timer/counter.
    tc_start(tc, APP_TC_CHANNEL);
}

// initialize usb USART
void init_ftdi_usart(void) {
    // GPIO map for USART.
    static const gpio_map_t DEV_USART_GPIO_MAP = {
        { DEV_USART_RX_PIN, DEV_USART_RX_FUNCTION },
        { DEV_USART_TX_PIN, DEV_USART_TX_FUNCTION }
    };

    // Options for USART.
    static const usart_options_t DEV_USART_OPTIONS = {
        .baudrate = DEV_USART_BAUDRATE,
        .charlength = 8,
        .paritytype = USART_NO_PARITY,
        .stopbits = USART_1_STOPBIT,
        .channelmode = USART_NORMAL_CHMODE
    };

    // Set up GPIO for DEV_USART
    gpio_enable_module(DEV_USART_GPIO_MAP, sizeof(DEV_USART_GPIO_MAP) /
                                               sizeof(DEV_USART_GPIO_MAP[0]));

    // Initialize in RS232 mode.
    usart_init_rs232(DEV_USART, &DEV_USART_OPTIONS, FPBA_HZ);

    /// FIXME: we really should set this *after* registering the interrupt
    /// handler.
    DEV_USART->ier = AVR32_USART_IER_RXRDY_MASK;
}

// initialize spi1: OLED, ADC, SD/MMC
extern void init_spi1(void) {
    static const gpio_map_t OLED_SPI_GPIO_MAP = {
        { OLED_SPI_SCK_PIN, OLED_SPI_SCK_FUNCTION },
        { OLED_SPI_MISO_PIN, OLED_SPI_MISO_FUNCTION },
        { OLED_SPI_MOSI_PIN, OLED_SPI_MOSI_FUNCTION },
        { OLED_SPI_NPCS0_PIN, OLED_SPI_NPCS0_FUNCTION },
        { OLED_SPI_NPCS1_PIN, OLED_SPI_NPCS1_FUNCTION },
        { OLED_SPI_NPCS2_PIN, OLED_SPI_NPCS2_FUNCTION },
    };

    // SPI options for OLED
    spi_options_t spiOptions = {.reg = OLED_SPI_NPCS,
                                .baudrate = 40000000,
                                .bits = 8,
                                .trans_delay = 0,
                                .spck_delay = 0,
                                .stay_act = 1,
                                .spi_mode = 3,
                                .modfdis = 1 };

    // Assign GPIO to SPI.
    gpio_enable_module(OLED_SPI_GPIO_MAP, sizeof(OLED_SPI_GPIO_MAP) /
                                              sizeof(OLED_SPI_GPIO_MAP[0]));
    // Initialize as master.
    spi_initMaster(OLED_SPI, &spiOptions);
    // Set SPI selection mode: variable_ps, pcs_decode, delay.
    spi_selectionMode(OLED_SPI, 0, 0, 0);
    // Enable SPI module.
    spi_enable(OLED_SPI);

    // setup chip register for OLED
    spi_setupChipReg(OLED_SPI, &spiOptions, FPBA_HZ);

    // add ADC chip register
    spiOptions.reg = ADC_SPI_NPCS;
    spiOptions.baudrate = 20000000;
    spiOptions.bits = 16;
    spiOptions.spi_mode = 2;
    spiOptions.spck_delay = 0;
    spiOptions.trans_delay = 5;
    spiOptions.stay_act = 0;
    spiOptions.modfdis = 0;

    spi_setupChipReg(ADC_SPI, &spiOptions, FPBA_HZ);

    // add SD/MMC chip register
    spiOptions.reg = SD_MMC_SPI_NPCS;
    spiOptions.baudrate =
        SD_MMC_SPI_MASTER_SPEED;        // Defined in conf_sd_mmc_spi.h;
    spiOptions.bits = SD_MMC_SPI_BITS;  // Defined in conf_sd_mmc_spi.h;
    spiOptions.spck_delay = 0;
    spiOptions.trans_delay = 0;
    spiOptions.stay_act = 1;
    spiOptions.spi_mode = 0;
    spiOptions.modfdis = 1;

    spi_setupChipReg(SD_MMC_SPI, &spiOptions, FPBA_HZ);
    // Initialize SD/MMC driver with SPI clock (PBA).
    sd_mmc_spi_init(spiOptions, FPBA_HZ);
}

// init PDCA (Peripheral DMA Controller A) resources for the SPI transfer and
// start a dummy transfer
void init_local_pdca(void) {
    // PDCA channel for SPI RX
    pdca_channel_options_t pdca_options_SPI_RX = {
        // pdca channel options
        .addr = (void *)&pdcaRxBuf,
        .size = FS_BUF_SIZE,  // transfer size
        .r_addr = NULL,       // next memory address after 1st transfer complete
        .r_size = 0,          // next transfer counter not used here
        .pid = AVR32_PDCA_CHANNEL_USED_RX,  // select peripheral ID - SPI1 RX
        .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the
                                                  // transfer: 8,16,32 bits
    };

    // PDCA channel for SPI TX
    pdca_channel_options_t pdca_options_SPI_TX = {
        // pdca channel options
        .addr = (void *)&pdcaTxBuf,  // memory address.
        .size = FS_BUF_SIZE,         // transfer size
        .r_addr = NULL,  // next memory address after 1st transfer complete
        .r_size = 0,     // next transfer counter not used here
        .pid = AVR32_PDCA_CHANNEL_USED_TX,  // select peripheral ID - SPI1 TX
        .transfer_size = PDCA_TRANSFER_SIZE_BYTE  // select size of the
                                                  // transfer: 8,16,32 bits
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
        .reg = BFIN_SPI_NPCS,
        //// FIXME:
        //// would prefer fast baudrate / lower trans delay during boot,
        //// but need multiple registers for boot (fast) and run (slow)
        //// investigate if this is possible...
        .baudrate = 20000000,
        .bits = 8,
        .spck_delay = 0,
        //    .trans_delay  = 0,
        //// try and reduce this...
        .trans_delay = 20,
        .stay_act = 1,
        .spi_mode = 1,
        .modfdis = 1
    };

    // assign pins to SPI.
    gpio_enable_module(BFIN_SPI_GPIO_MAP, sizeof(BFIN_SPI_GPIO_MAP) /
                                              sizeof(BFIN_SPI_GPIO_MAP[0]));

    // intialize as master
    spi_initMaster(BFIN_SPI, &spiOptions);

    // set selection mode: variable_ps, pcs_decode, delay.
    spi_selectionMode(BFIN_SPI, 0, 0, 0);

    // enable SPI.
    spi_enable(BFIN_SPI);

    // intialize the chip register
    spi_setupChipReg(BFIN_SPI, &spiOptions, FPBA_HZ);
    // we would do an internal pulldown on HWAIT here,
    // but this avr32 part family doesn't have it.
    // so there is an external pulldown instead.

    // enable pullup on bfin RESET line
    gpio_enable_pin_pull_up(BFIN_RESET_PIN);
}

// intialize two-wire interface
void init_twi(void) {
    // TWI/I2C GPIO map
    static const gpio_map_t TWI_GPIO_MAP = {
        { TWI_DATA_PIN, TWI_DATA_FUNCTION },
        { TWI_CLOCK_PIN, TWI_CLOCK_FUNCTION }
    };
    gpio_enable_module(TWI_GPIO_MAP,
                       sizeof(TWI_GPIO_MAP) / sizeof(TWI_GPIO_MAP[0]));
}

// initialize USB host stack
void init_usb_host(void) { uhc_start(); }
