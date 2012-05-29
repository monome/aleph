#ifndef _CONFIG_H_
#define _CONFIG_H_

//--------------------------------------------------------------
// SYSTEM CLOCKS:

#define FOSC32          32768                                   //!< Osc32 frequency: Hz.
#define OSC32_STARTUP   AVR32_PM_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

#define FOSC0           12000000                                //!< Osc0 frequency: Hz.
#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.

// System Clock Frequencies
#define FMCK_HZ   		66000000L
#define FCPU_HZ   		FMCK_HZ
#define FHSB_HZ   		FCPU_HZ
#define FPBA_HZ   		FMCK_HZ
#define FPBB_HZ   		FMCK_HZ

// UART0 : FTDI, USB device
#define FTDI_UART               (&AVR32_UART0)
#define FTDI_UART_IRQ   
// rx pin is  PA00
#define FTDI_UART_RX_PIN       	AVR32_USART0_RXD_0_0_PIN
#define FTDI_UART_RX_FUNCTION   AVR32_USART0_RXD_0_0_FUNCTION
// tx pin is PA01
#define FTDI_UART_TX_PIN        AVR32_USART0_TXD_0_0_PIN
#define FTDI_UART_TX_FUNCTION   AVR32_USART0_TXD_0_0_FUNCTION


// UART1 : OLED, SPI mode
#define OLED_SPI_UART               (&AVR32_UART1)
#define OLED_SPI_UART_IRQ   
// rx pin is  PA05
#define OLED_SPI_UART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#define OLED_SPI_UART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
// tx pin is PA06
#define OLED_SPI_UART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#define OLED_SPI_UART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
// clk pin is PA07
#define OLED_SPI_UART_CLC_PIN        AVR32_USART1_CLK_0_0_PIN
#define OLED_SPI_UART_CLK_FUNCTION   AVR32_USART1_CLK_0_0_FUNCTION

// SPI0 : blackfin
#define BFIN_3SPI                   (&AVR32_SPI0)
#define BFIN_SPI_NPCS              0
#define BFIN_SPI_SCK_PIN           AVR32_SPI0_SCK_0_0_PIN
#define BFIN_SPI_SCK_FUNCTION      AVR32_SPI0_SCK_0_0_FUNCTION
#define BFIN_SPI_MISO_PIN          AVR32_SPI0_MISO_0_0_PIN
#define BFIN_SPI_MISO_FUNCTION     AVR32_SPI0_MISO_0_0_FUNCTION
#define BFIN_SPI_MOSI_PIN          AVR32_SPI0_MOSI_0_0_PIN
#define BFIN_SPI_MOSI_FUNCTION     AVR32_SPI0_MOSI_0_0_FUNCTION
#define BFIN_SPI_NPCS_PIN          AVR32_SPI0_NPCS_0_0_PIN
#define BFIN_SPI_NPCS_FUNCTION     AVR32_SPI0_NPCS_0_0_FUNCTION

// SPI1 : ADC
#define ADC_SPI                   (&AVR32_SPI1)
#define ADC_SPI_NPCS              0
#define ADC_SPI_SCK_PIN           AVR32_SPI1_SCK_0_0_PIN
#define ADC_SPI_SCK_FUNCTION      AVR32_SPI1_SCK_0_0_FUNCTION
#define ADC_SPI_MISO_PIN          AVR32_SPI1_MISO_0_0_PIN
#define ADC_SPI_MISO_FUNCTION     AVR32_SPI1_MISO_0_0_FUNCTION
#define ADC_SPI_MOSI_PIN          AVR32_SPI1_MOSI_0_0_PIN
#define ADC_SPI_MOSI_FUNCTION     AVR32_SPI1_MOSI_0_0_FUNCTION
#define ADC_SPI_NPCS_PIN          AVR32_SPI1_NPCS_0_0_PIN
#define ADC_SPI_NPCS_FUNCTION     AVR32_SPI1_NPCS_0_0_FUNCTION

// PA21 : usb host ID
#define USB_HOST_ID_PIN         AVR32_PIN_PA21
// PA22 : usb host VBOF
#define USB_HOST_VBOF_PIN       AVR32_PIN_PA22

// PA24 : edit mode LED
#define EDIT_LED_PIN		AVR32_PIN_PA24
// PA25 : edit mode switch
#define EDIT_SW_PIN		AVR32_PIN_PA25

// PA26 - PA29 : switches
#define SW0_PIN		AVR32_PIN_PA26
#define SW1_PIN		AVR32_PIN_PA27
#define SW2_PIN		AVR32_PIN_PA28
#define SW3_PIN		AVR32_PIN_PA29

// PB00 - PB07 : encoders
#define ENC0_S0_PIN		AVR32_PIN_PB00
#define ENC0_S1_PIN		AVR32_PIN_PB01
#define ENC1_S0_PIN		AVR32_PIN_PB02
#define ENC1_S1_PIN		AVR32_PIN_PB03
#define ENC2_S0_PIN		AVR32_PIN_PB04
#define ENC2_S1_PIN		AVR32_PIN_PB05
#define ENC3_S0_PIN		AVR32_PIN_PB06
#define ENC3_S1_PIN		AVR32_PIN_PB07

#endif // header guard
