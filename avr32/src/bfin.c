/* bfin.c
   aleph-avr32

   routines to communicate with the bf533 DSP
*/
//ASF
#include "compiler.h"
#include "delay.h"
#include "gpio.h"
#include "spi.h"
// aleph
#include "conf_aleph.h"
#include "global.h"
#include "param.h"
#include "protocol.h"
#include "types.h"
#include "bfin.h"

// HWAIT status from blackfin
volatile U8 hwait = 0;
// message data structure for param handling
static msg_t msg;

void bfin_load(U32 size, char * data) {
  U64 delay;
  U64 byte;

  Disable_global_interrupt();
  
  // reset the blackfin
  gpio_clr_gpio_pin(BFIN_RESET_PIN);
  // wait a little
  delay = 50; while (--delay > 0) {;;}
    //  delay_ms(1);
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  // wait a little more
  //  delay = 6000; while (--delay > 0) {;;}
  delay_ms(1);
  // loop over .ldr data
  byte = 0;
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  while(byte < size) {
    // pause if hwait is held high by the blackfin
    while(hwait > 0) {
      delay++;
    }
    spi_write(BFIN_SPI, data[byte]);
    byte++;
  }
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  Enable_global_interrupt();
}

void bfin_param(U32 idx, F32 val) {
  U8 i;
  msg.generic.command = COM_SET_PARAM;
  msg.setParam.idx = idx;
  msg.setParam.value.asFloat = val;

  for(i=0; i<MSG_BYTES; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, msg.raw[i]);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  }
}
