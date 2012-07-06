/* bfin.c
   aleph-avr32

   routines to communicate with the bf533 DSP
*/
//ASF
#include "compiler.h"
#include "gpio.h"
#include "spi.h"
// aleph
#include "conf_aleph.h"
#include "bfin.h"


// HWAIT status from blackfin
volatile U8 hwait = 0;

void bfin_load(U32 size, char * data) {
  U64 delay;
  U64 byte;

  // reset the blackfin
  gpio_clr_gpio_pin(BFIN_RESET_PIN);
  // wait a little
  delay = 10; while (--delay > 0) {;;}
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  // wait a little more
  delay = 1000; while (--delay > 0) {;;}

  // loop over .ldr data
  byte = 0;
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  while(byte < size) {
    // pause if hwait is held high by the blackfin
    while(hwait > 0) {
      ;;
    }
    spi_write(BFIN_SPI, data[byte]);
    byte++;
  }
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
}

void bfin_param(U32 idx, F32 val) {
}
