/* isr.h*
 * aleph-blackfin audio core
 *
 * function prototypes for interrupt service routines
 */

#ifndef _ALEPH_BFIN_ISR_H_
#define _ALEPH_BFIN_ISR_H_

#if 0
// sport1 transmit interrupt (dacs)
void sport1_tx_isr() __attribute__((interrupt_handler));
// error handler
void error_isr() __attribute__((interrupt_handler));
#endif

void spi_rx_isr() __attribute__((interrupt_handler));

#endif // header guard
