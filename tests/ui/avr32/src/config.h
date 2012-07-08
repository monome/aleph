#ifndef _CONFIG_H_
#define _CONFIG_H_

//--------------------------------------------------------------
// SYSTEM CLOCKS:

//#define FOSC32          32768                                   //!< Osc32 frequency: Hz.
//#define OSC32_STARTUP   AVR32_PM_OSCCTRL32_STARTUP_8192_RCOSC //!< Osc32 startup time: RCOsc periods.

//#define FOSC0           12000000                                //!< Osc0 frequency: Hz.
//#define OSC0_STARTUP    AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC  //!< Osc0 startup time: RCOsc periods.



// PLL output frequency
#define PLL_OUTPUT_FREQ 132000000UL

/*
// System Clock Frequencies
#define FMCK_HZ   		66000000L
#define FCPU_HZ   		FMCK_HZ
#define FHSB_HZ   		FCPU_HZ
#define FPBA_HZ   		FMCK_HZ
#define FPBB_HZ   		FMCK_HZ
*/

// application timer
#define APP_TC                 (&AVR32_TC)
#define APP_TC_CHANNEL         0
#define APP_TC_IRQ             AVR32_TC_IRQ0
#define APP_TC_IRQ_GROUP       AVR32_TC_IRQ_GROUP
#define APP_TC_IRQ_PRIORITY    AVR32_INTC_INT0

// USART in SPI master mode for OLED
// FIXME: only works with USART2 on the EVK1100.. 
#define OLED_USART_SPI                 (&AVR32_USART2)
#define OLED_USART_SPI_SCK_PIN         AVR32_USART2_CLK_0_PIN
#define OLED_USART_SPI_SCK_FUNCTION    AVR32_USART2_CLK_0_FUNCTION
#define OLED_USART_SPI_MISO_PIN        AVR32_USART2_RXD_0_0_PIN
#define OLED_USART_SPI_MISO_FUNCTION   AVR32_USART2_RXD_0_0_FUNCTION
#define OLED_USART_SPI_MOSI_PIN        AVR32_USART2_TXD_0_0_PIN
#define OLED_USART_SPI_MOSI_FUNCTION   AVR32_USART2_TXD_0_0_FUNCTION
#define OLED_USART_SPI_NSS_PIN         AVR32_USART2_RTS_0_0_PIN
#define OLED_USART_SPI_NSS_FUNCTION    AVR32_USART2_RTS_0_0_FUNCTION
// PA09 in GPIO for OLED command/data register select
#define OLED_REGISTER_PIN AVR32_PIN_PA23

// PB00 - PB07 : encoders
/*
#define ENC0_S0_PIN		AVR32_PIN_PB00
#define ENC0_S1_PIN		AVR32_PIN_PB01
*/
//// TEST
#define ENC0_S0_PIN		AVR32_PIN_PA24
#define ENC0_S1_PIN		AVR32_PIN_PA25
////
#define ENC1_S0_PIN		AVR32_PIN_PB02
#define ENC1_S1_PIN		AVR32_PIN_PB03
#define ENC2_S0_PIN		AVR32_PIN_PB04
#define ENC2_S1_PIN		AVR32_PIN_PB05
#define ENC3_S0_PIN		AVR32_PIN_PB06
#define ENC3_S1_PIN		AVR32_PIN_PB07

// PA26 - PA29 : switches
#define SW0_PIN		AVR32_PIN_PA26
#define SW1_PIN		AVR32_PIN_PA27
#define SW2_PIN		AVR32_PIN_PA28
// for now, PA29 is CS for OLED-SPI
// #define SW3_PIN		AVR32_PIN_PA29
#define SW3_PIN		AVR32_PIN_PA30

#endif // header guard
