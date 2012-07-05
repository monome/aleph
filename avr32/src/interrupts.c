// ASF
#include "compiler.h"
#include "intc.h"
#include "pdca.h"
#include "sd_mmc_spi.h"
// aleph
#include "conf_aleph.h"
#include "interrupts.h"


// Used to indicate the end of PDCA transfer
volatile bool end_of_transfer;

// Software wait
static void wait(void);
void wait(void)
{
  volatile int i;
  for(i = 0 ; i < 5000; i++);
}

//// interrupts
__attribute__((__interrupt__))
static void pdca_int_handler(void)
{
  // Disable all interrupts.
  Disable_global_interrupt();

  // Disable interrupt channel.
  pdca_disable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);

  sd_mmc_spi_read_close_PDCA();//unselects the SD/MMC memory.
  wait();
  // Disable unnecessary channel
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_TX);
  pdca_disable(AVR32_PDCA_CHANNEL_SPI_RX);

  // Enable all interrupts.
  Enable_global_interrupt();

  end_of_transfer = true;
}

void register_interrupts(void) {
  INTC_register_interrupt(&pdca_int_handler, AVR32_PDCA_IRQ_0, AVR32_INTC_INT1);  // pdca_channel_spi1_RX = 0
}
