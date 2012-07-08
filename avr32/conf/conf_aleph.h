/* conf_aleph.h
   aleph-avr32

   device-specific hardware defines for aleph-avr32
 */

#ifndef _CONF_ALEPH_H_
#define _CONF_ALEPH_H_

//==============================================
//==== clocks

// target PBA clock frequency
//#define PBA_HZ                FOSC0
// PLL output frequency
#define PLL_OUTPUT_FREQ 132000000UL

//==============================================
//==== timers

// application timer
#define APP_TC                 (&AVR32_TC)
#define APP_TC_CHANNEL         0
#define APP_TC_IRQ             AVR32_TC_IRQ0
#define APP_TC_IRQ_GROUP       AVR32_TC_IRQ_GROUP
#define APP_TC_IRQ_PRIORITY    AVR32_INTC_INT0

//==============================================
//==== USART
// debug usart
#define DBG_USART               (&AVR32_USART1)
#define DBG_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#define DBG_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#define DBG_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#define DBG_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#define DBG_USART_BAUDRATE      57600

// oled usart (in spi mode)
#define OLED_USART_SPI                 (&AVR32_USART2)
#define OLED_USART_SPI_SCK_PIN         AVR32_USART2_CLK_0_PIN
#define OLED_USART_SPI_SCK_FUNCTION    AVR32_USART2_CLK_0_FUNCTION
#define OLED_USART_SPI_MISO_PIN        AVR32_USART2_RXD_0_0_PIN
#define OLED_USART_SPI_MISO_FUNCTION   AVR32_USART2_RXD_0_0_FUNCTION
#define OLED_USART_SPI_MOSI_PIN        AVR32_USART2_TXD_0_0_PIN
#define OLED_USART_SPI_MOSI_FUNCTION   AVR32_USART2_TXD_0_0_FUNCTION
#define OLED_USART_SPI_NSS_PIN         AVR32_USART2_RTS_0_0_PIN
#define OLED_USART_SPI_NSS_FUNCTION    AVR32_USART2_RTS_0_0_FUNCTION
//==============================================
//==== SPI
// blackfin SPI
#define BFIN_SPI                   (&AVR32_SPI0)
#define BFIN_SPI_NPCS              0
#define BFIN_SPI_SCK_PIN           AVR32_SPI0_SCK_0_0_PIN
#define BFIN_SPI_SCK_FUNCTION      AVR32_SPI0_SCK_0_0_FUNCTION
#define BFIN_SPI_MISO_PIN          AVR32_SPI0_MISO_0_0_PIN
#define BFIN_SPI_MISO_FUNCTION     AVR32_SPI0_MISO_0_0_FUNCTION
#define BFIN_SPI_MOSI_PIN          AVR32_SPI0_MOSI_0_0_PIN
#define BFIN_SPI_MOSI_FUNCTION     AVR32_SPI0_MOSI_0_0_FUNCTION
#define BFIN_SPI_NPCS_PIN          AVR32_SPI0_NPCS_0_0_PIN
#define BFIN_SPI_NPCS_FUNCTION     AVR32_SPI0_NPCS_0_0_FUNCTION

// sdcard SPI
#define SD_MMC_SPI                  (&AVR32_SPI1)
#define SD_MMC_SPI_NPCS             1
#define SD_MMC_SPI_SCK_PIN          AVR32_SPI1_SCK_0_0_PIN
#define SD_MMC_SPI_SCK_FUNCTION     AVR32_SPI1_SCK_0_0_FUNCTION
#define SD_MMC_SPI_MISO_PIN         AVR32_SPI1_MISO_0_0_PIN
#define SD_MMC_SPI_MISO_FUNCTION    AVR32_SPI1_MISO_0_0_FUNCTION
#define SD_MMC_SPI_MOSI_PIN         AVR32_SPI1_MOSI_0_0_PIN
#define SD_MMC_SPI_MOSI_FUNCTION    AVR32_SPI1_MOSI_0_0_FUNCTION
#define SD_MMC_SPI_NPCS_PIN         AVR32_SPI1_NPCS_1_0_PIN
#define SD_MMC_SPI_NPCS_FUNCTION    AVR32_SPI1_NPCS_1_0_FUNCTION


//==============================================
//==== peripheral DMA controller
// SDRAM
#define AVR32_PDCA_CHANNEL_USED_RX  AVR32_PDCA_PID_SPI1_RX
#define AVR32_PDCA_CHANNEL_USED_TX  AVR32_PDCA_PID_SPI1_TX
#define AVR32_PDCA_CHANNEL_SPI_RX   0 
#define AVR32_PDCA_CHANNEL_SPI_TX   1 

//==============================================
//==== GPIO
// sd card
#define SD_MMC_CARD_DETECT_PIN      AVR32_PIN_PA02
#define SD_MMC_WRITE_PROTECT_PIN    AVR32_PIN_PA07

// PA09 in GPIO for OLED command/data register select
#define OLED_REGISTER_PIN AVR32_PIN_PA23

// (input) receive HWAIT from the blackfin during boot process
//#define BFIN_HWAIT_PIN    AVR32_PIN_PB00
#define BFIN_HWAIT_PIN    AVR32_PIN_PA03
// (output) pull low to reset the blackfin
#define BFIN_RESET_PIN    AVR32_PIN_PB01

// encoders (8 pins)
#define ENC0_S0_PIN		AVR32_PIN_PA23
#define ENC0_S1_PIN		AVR32_PIN_PA24
#define ENC1_S0_PIN		AVR32_PIN_PA25
#define ENC1_S1_PIN		AVR32_PIN_PA26
#define ENC2_S0_PIN		AVR32_PIN_PA27
#define ENC2_S1_PIN		AVR32_PIN_PA28
#define ENC3_S0_PIN		AVR32_PIN_PA30
#define ENC3_S1_PIN		AVR32_PIN_PB02

// switches (6 pins)
#define SW0_PIN		AVR32_PIN_PB03
#define SW1_PIN		AVR32_PIN_PB04
#define SW2_PIN		AVR32_PIN_PB05
#define SW3_PIN		AVR32_PIN_PB06
#define SW_EDIT_PIN     AVR32_PIN_PB07

#endif 
