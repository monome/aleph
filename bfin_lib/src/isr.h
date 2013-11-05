/* isr.h*
 * aleph-blackfin audio core
 *
 * function prototypes for interrupt service routines
 */

#ifndef _ALEPH_BFIN_ISR_H_
#define _ALEPH_BFIN_ISR_H_

// sport0 receive interrupt (ad1836)
void sport0_rx_isr() __attribute((interrupt_handler));
// sport1 transmit interrupt (ad5686R)
void sport1_tx_isr() __attribute((interrupt_handler));

// spi data receive interrupt (avr32)
void spi_rx_isr() __attribute__((interrupt_handler));
// programmable flag interrupt (buttons)
void pf_isr() __attribute__((interrupt_handler));

#endif // header guard
