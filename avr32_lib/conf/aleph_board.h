/* conf_aleph.h
   aleph-avr32

   device-specific hardware defines for aleph-avr32
 */

#ifndef _CONF_ALEPH_H_
#define _CONF_ALEPH_H_

/* //============================================== */
/* //==== SDRAM */
/* //define SDRAM_PART_HDR  "mt48lc16m16a2tg7e/mt48lc16m16a2tg7e.h" */

/* //! Part header file of used SDRAM(s). */
/* /// this is the actual part number, i attempted to customize, doesn't seem to matter */
/* #define SDRAM_PART_HDR  "mt48lc16m16a2tg75.h" */
/* ///// this is the defualt header for that part family from the EVK */
/* //#define SDRAM_PART_HDR  "mt48lc16m16a2tg7e.h" */

/* //! Data bus width to use the SDRAM(s) with (16 or 32 bits; always 16 bits on UC3). */
/* #define SDRAM_DBW 16 */

//==============================================
//==== clocks

#define FOSC32          32768                                   //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_PM_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.
#define FOSC0           12000000                                //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.
//#define PLL_OUTPUT_FREQ 132000000UL

#if 1
#define FMCK_HZ   		66000000   // master */
#define FCPU_HZ   		FMCK_HZ    // cpu */
#define FHSB_HZ   		FCPU_HZ    // highspeed bus */
#define FPBA_HZ   		FMCK_HZ    // peripheral bus A */
#define FPBB_HZ   		FMCK_HZ    // peripheral bus B */
#else
// 
///// using osc0
#define FMCK_HZ   		12000000   // master */
#define FCPU_HZ   		FMCK_HZ    // cpu */
#define FHSB_HZ   		FCPU_HZ    // highspeed bus */
#define FPBA_HZ   		FMCK_HZ    // peripheral bus A */
#define FPBB_HZ   		FMCK_HZ    // peripheral bus B */
#endif

// clock manager needs these
#define BOARD_OSC0_HZ           12000000
#define BOARD_OSC0_STARTUP_US   17000
#define BOARD_OSC0_IS_XTAL      true
#define BOARD_OSC32_HZ          32768
#define BOARD_OSC32_STARTUP_US  71000
#define BOARD_OSC32_IS_XTAL     true

//==============================================
//==== USART
// usart / device port
#define DEV_USART               (&AVR32_USART0)
#define DEV_USART_RX_PIN        AVR32_USART0_RXD_0_0_PIN
#define DEV_USART_RX_FUNCTION   AVR32_USART0_RXD_0_0_FUNCTION
#define DEV_USART_TX_PIN        AVR32_USART0_TXD_0_0_PIN
#define DEV_USART_TX_FUNCTION   AVR32_USART0_TXD_0_0_FUNCTION
/// define this from your application config
#ifndef DEV_USART_BAUDRATE
#define DEV_USART_BAUDRATE      57600
#endif

/// compatibility with ASF print funcs
#define DBG_USART              DEV_USART
#define DBG_USART_RX_PIN       DEV_USART_RX_PIN
#define DBG_USART_RX_FUNCTION  DEV_USART_RX_FUNCTION
#define DBG_USART_TX_PIN       DEV_USART_TX_PIN
#define DBG_USART_TX_FUNCTION  DEV_USART_TX_FUNCTION
#define DBG_USART_BAUDRATE     DEV_USART_BAUDRATE

//==============================================
//==== SPI


// oled SPI
#define OLED_SPI                   (&AVR32_SPI1)
#define OLED_SPI_SCK_PIN           AVR32_SPI1_SCK_0_0_PIN
#define OLED_SPI_SCK_FUNCTION      AVR32_SPI1_SCK_0_0_FUNCTION
#define OLED_SPI_MISO_PIN          AVR32_SPI1_MISO_0_0_PIN
#define OLED_SPI_MISO_FUNCTION     AVR32_SPI1_MISO_0_0_FUNCTION
#define OLED_SPI_MOSI_PIN          AVR32_SPI1_MOSI_0_0_PIN
#define OLED_SPI_MOSI_FUNCTION     AVR32_SPI1_MOSI_0_0_FUNCTION
#define OLED_SPI_NPCS0_PIN         AVR32_SPI1_NPCS_0_0_PIN
#define OLED_SPI_NPCS0_FUNCTION    AVR32_SPI1_NPCS_0_0_FUNCTION
#define OLED_SPI_NPCS2_PIN         AVR32_SPI1_NPCS_2_0_PIN
#define OLED_SPI_NPCS2_FUNCTION    AVR32_SPI1_NPCS_2_0_FUNCTION
#define OLED_SPI_NPCS1_PIN         AVR32_SPI1_NPCS_1_0_PIN
#define OLED_SPI_NPCS1_FUNCTION    AVR32_SPI1_NPCS_1_0_FUNCTION
#define OLED_SPI_NPCS              0

// adc, multiplexed
#define ADC_SPI                    OLED_SPI
#define ADC_SPI_NPCS               1

// sdcard, multiplexed
#define SD_MMC_SPI                 OLED_SPI
#define SD_MMC_SPI_NPCS            2

// blackfin SPI
#define BFIN_SPI                   (&AVR32_SPI0)
#define BFIN_SPI_NPCS              0
#define BFIN_SPI_SCK_PIN           AVR32_SPI0_SCK_0_0_PIN
#define BFIN_SPI_SCK_FUNCTION      AVR32_SPI0_SCK_0_0_FUNCTION
#define BFIN_SPI_MISO_PIN          AVR32_SPI0_MISO_0_0_PIN
#define BFIN_SPI_MISO_FUNCTION     AVR32_SPI0_MISO_0_0_FUNCTION
#define BFIN_SPI_MOSI_PIN          AVR32_SPI0_MOSI_0_0_PIN
#define BFIN_SPI_MOSI_FUNCTION     AVR32_SPI0_MOSI_0_0_FUNCTION
#define BFIN_SPI_NPCS0_PIN         AVR32_SPI0_NPCS_0_0_PIN
#define BFIN_SPI_NPCS0_FUNCTION    AVR32_SPI0_NPCS_0_0_FUNCTION

//==============================================
//==== peripheral DMA controller
// SDCARD
#define AVR32_PDCA_CHANNEL_USED_RX  AVR32_PDCA_PID_SPI1_RX
#define AVR32_PDCA_CHANNEL_USED_TX  AVR32_PDCA_PID_SPI1_TX
#define AVR32_PDCA_CHANNEL_SPI_RX   0 
#define AVR32_PDCA_CHANNEL_SPI_TX   1 

//==============================================
//==== GPIO

// CTS used for usb-usart enumeration detection
#define USART_USB_DETECT_PIN	AVR32_PIN_PA04

// sd card
#define SD_MMC_CARD_DETECT_PIN      AVR32_PIN_PB00
#define SD_MMC_WRITE_PROTECT_PIN    AVR32_PIN_PB01

// oled
#define OLED_RESET_PIN    AVR32_PIN_PA07
#define OLED_REGISTER_PIN AVR32_PIN_PA09

// bfin
#define BFIN_HWAIT_PIN    AVR32_PIN_PA05
#define BFIN_RESET_PIN    AVR32_PIN_PA06
#define BFIN_READY_PIN    AVR32_PIN_PB20
#define BFIN_REQUEST_PIN    AVR32_PIN_PB21

// encoders
// beware that the ISR code makes some assumpmtions
// bout which encoders are on which I/O ports.
// so if you change this, verify that the irq handler setup still works.
#define ENC3_S0_PIN		AVR32_PIN_PB02
#define ENC3_S1_PIN		AVR32_PIN_PB03
#define ENC2_S0_PIN		AVR32_PIN_PB04
#define ENC2_S1_PIN		AVR32_PIN_PB05
#define ENC1_S0_PIN		AVR32_PIN_PB06
#define ENC1_S1_PIN		AVR32_PIN_PB07
#define ENC0_S0_PIN		AVR32_PIN_PB08
#define ENC0_S1_PIN		AVR32_PIN_PB09

// switches
// fn switches
#define SW0_PIN		AVR32_PIN_PA25
#define SW1_PIN		AVR32_PIN_PA26
#define SW2_PIN		AVR32_PIN_PA27
#define SW3_PIN		AVR32_PIN_PA28
// footswitches
#define FS0_PIN         AVR32_PIN_PB30 
#define FS1_PIN         AVR32_PIN_PB31
// edit/play mode
#define LED_MODE_PIN    AVR32_PIN_PA23
#define SW_MODE_PIN     AVR32_PIN_PA24
// power-down control
#define SW_POWER_PIN    AVR32_PIN_PB16
#define POWER_CTL_PIN   AVR32_PIN_PB15


//TWI
#define TWI_DATA_PIN            AVR32_TWI_SDA_0_0_PIN
#define TWI_DATA_FUNCTION   AVR32_TWI_SDA_0_0_FUNCTION
#define TWI_CLOCK_PIN           AVR32_TWI_SCL_0_0_PIN
#define TWI_CLOCK_FUNCTION  AVR32_TWI_SCL_0_0_FUNCTION
//#define TWI_SPEED 50000;
#define TWI_SPEED 132000;

// USB pins

// vbof: pa22, pin 74
// oc:   pa20, pin 66
// id:   unused.... but lets put it on pa21 and keep that reserved

#define USB_ID                      AVR32_USBB_USB_ID_0_2 // PA21
//! Multiplexed pin used for USB_VBOF: AVR32_USBB_USB_VBOF_x_x.
//! To be selected according to the AVR32_USBB_USB_VBOF_x_x_PIN and
//! AVR32_USBB_USB_VBOF_x_x_FUNCTION definitions from <avr32/uc3axxxx.h>.
/// FIXME: what is this multiplexed with?
#define USB_VBOF                    AVR32_USBB_USB_VBOF_0_2 // 
//! Active level of the USB_VBOF output pin.
#define USB_VBOF_ACTIVE_LEVEL       LOW
//! USB overcurrent detection pin.
#define USB_OVERCURRENT_DETECT_PIN  AVR32_PIN_PX33

#endif // header guard
