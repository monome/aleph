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

// load a blackfin executable
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

// set a parameter
void bfin_set_param(U32 idx, F32 val) {
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

// get number of parameters 
u16 bfin_get_num_params(void) {
  u8 i;
  // annnoying; spi_read requires pointer to u16
  u16 tmp;
  /// TEST: fill with an unacceptable value to catch byte order problems
  tmp = 0xFFFF;
  // send command word
  msg.generic.command = COM_GET_NUM_PARAMS;
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, msg.generic.command);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // read data bytes
  for(i=1; i<MSG_BYTES; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_read(BFIN_SPI, &tmp);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    msg.raw[i]= (U8)(tmp & 0xff);
  }
  return msg.numParams.value;
}

// get a parameter name
void bfin_get_param_name(u16 paramIdx, char * name) {
  u8 i;  
  u8 strIdx = 0;
  // annnoying; spi_read requires pointer to u16
  u16 tmp;
  /// TEST: fill with an unacceptable value to catch byte order problems
  tmp = 0xFFFF;
  /// name is split into 2 messages...
  // send command word
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, COM_GET_PARAM_LABEL_1);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // read data bytes
  for(i=1; i<MSG_BYTES; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_read(BFIN_SPI, &tmp); 
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    name[strIdx] = (char)(tmp & 0xff);
    strIdx++;
  }
  // send command word
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, COM_GET_PARAM_LABEL_2);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  strIdx++;
  // read data bytes
  for(i=1; i<MSG_BYTES; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_read(BFIN_SPI, &tmp); 
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    name[strIdx] = (char)(tmp & 0xff);
    strIdx++;
  }
}


// get parameter descriptor
//void bfin_get_param_name(u16 paramIdx, ParamDesc* pDesc);

