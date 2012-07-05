/* conf_aleph.h
   aleph-avr32

   device-specific hardware defines for aleph-avr32
 */

#ifndef _CONF_ALEPH_H_
#define _CONF_ALEPH_H_


// debug with USART1
#define DBG_USART               (&AVR32_USART1)
#define DBG_USART_RX_PIN        AVR32_USART1_RXD_0_0_PIN
#define DBG_USART_RX_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#define DBG_USART_TX_PIN        AVR32_USART1_TXD_0_0_PIN
#define DBG_USART_TX_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#define DBG_USART_BAUDRATE      57600

#define AVR32_PDCA_CHANNEL_USED_RX AVR32_PDCA_PID_SPI1_RX
#define AVR32_PDCA_CHANNEL_USED_TX AVR32_PDCA_PID_SPI1_TX

// target PBA clock frequency (Hz)
#define PBA_HZ                FOSC0

#define AVR32_PDCA_CHANNEL_SPI_RX 0 // In the example we will use the pdca channel 0.
#define AVR32_PDCA_CHANNEL_SPI_TX 1 // In the example we will use the 

#endif 
