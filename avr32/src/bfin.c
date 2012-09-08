/* bfin.c
   aleph-avr32

   routines to communicate with the bf533 DSP
*/
//ASF
#include "compiler.h"
#include "util.h"
#include "gpio.h"
#include "spi.h"
///// DEBUG
#include "print_funcs.h"
/////
// aleph
#include "conf_aleph.h"
#include "global.h"
#include "param.h"
#include "protocol.h"
#include "types.h"
#include "bfin.h"

// HWAIT status from blackfin
volatile U8 hwait = 0;
// message data structures for param handling
//static msg_t rxMsg;
//static msg_t txMsg;

// load a blackfin executable
void bfin_load(U32 size, volatile u8 * data) {
  U64 delay;
  U64 byte;

  print_dbg("\r\n loading bfin: ");
  print_dbg_ulong(size);
  print_dbg(" bytes...");

  Disable_global_interrupt();

  
  // reset the blackfin
  gpio_clr_gpio_pin(BFIN_RESET_PIN);
  // wait a little
  delay = 100; while (--delay > 0) {;;}
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  // wait a little more
  delay = 20000; while (--delay > 0) {;;}
  
  // loop over .ldr data
  byte = 0;
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);


   print_dbg("\r\n");

  while(byte < size) {
    // pause if hwait is held high by the blackfin
    while(hwait > 0) { ;; }

    spi_write(BFIN_SPI, data[byte]);

    /////// test
    /*
    if(byte < 2048) {
      print_dbg_ulong((u32)data[byte]);
      print_dbg(" ");
    }
    */  
    //    delay = 10; while (--delay > 0) {;;}
    /////////
    //    delay = 100; while (--delay > 0) {;;}

    byte++;
  }
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  Enable_global_interrupt();
}

void bfin_set_param(u8 idx, f32 x ) {
  static ParamValue pval;
  pval.asFloat = x;
  // command
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_SET_PARAM_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  //idx
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, idx);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  //val0
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, pval.asByte[0]);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // val1
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, pval.asByte[1]);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  //val2
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, pval.asByte[2]);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  //val3
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, pval.asByte[3]);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
}

void bfin_get_num_params(volatile u32* num) {
  u16 x;
  // command 
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_GET_NUM_PARAMS_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // read num
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, 0); //dont care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  *num = (u8)(x & 0xff);
}

void bfin_get_param_desc(u16 paramIdx, volatile ParamDesc* pDesc) {
  ParamValue pval;
  u16 x; // u16 for spi_read()
  u8 i;
  // command 
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_GET_PARAM_DESC_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // idx
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, paramIdx);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // read label
  for(i=0; i<PARAM_LABEL_LEN; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    pDesc->label[i] = (char)(x & 0xff);
  }
  // read unit
  for(i=0; i<PARAM_UNIT_LEN; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    pDesc->unit[i] = (char)(x & 0xff);
  }
  // read type
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, 0); //dont care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  pDesc->type = (U8)(x & 0xff);
  // read min
  for(i=0; i<4; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    pval.asByte[i] = (u8)(x & 0xff);
  }
  pDesc->min = pval.asFloat;
  // read max
  for(i=0; i<4; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    pval.asByte[i] = (u8)(x & 0xff);
  }
  pDesc->max = pval.asFloat;
}

// get module name
void bfin_get_module_name(volatile char* buf) {
  char name[MODULE_NAME_LEN];
  u16 x; // u16 for spi_read()
  u8 i;
  // command 
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_GET_MODULE_NAME_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  for(i=0; i<MODULE_NAME_LEN; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    name[i] = (char)(x & 0xff);
  }
}

/*
// set a parameter
void bfin_set_param(U32 idx, F32 val) {
  U8 i;
  // U16 tmp;
  txMsg.generic.command = MSG_SET_PARAM_COM;
  txMsg.setParam.idx = idx;
  txMsg.setParam.value.asFloat = val;

  for(i=0; i<sizeof(msgSetParam_t); i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, txMsg.raw[i]);
    //// TEST
    // spi_read(BFIN_SPI, &tmp);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  }
}

// get number of parameters 
U16 bfin_get_num_params(void) {
  u8 i;
  // annnoying; spi_read requires pointer to u16
  u16 tmp;
  u8 bytes = sizeof(msgGetNumParams_t);

  // set command
  txMsg.getNumParams.command = MSG_GET_NUM_PARAMS_COM;

  // read/write
  for(i=0; i<bytes; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    // each read needs a dummy write...
    spi_write(BFIN_SPI, txMsg.raw[i]);
    spi_read(BFIN_SPI, &tmp);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    print_dbg("\r\n getNumParams got u16 value: ");
    print_dbg_ulong(tmp);
    rxMsg.raw[i]= (U8)(tmp & 0xff);
  }
  print_dbg("\r\n\r\n");
  return rxMsg.getNumParams.value;
}

void bfin_get_param_desc(u16 paramIdx, ParamDesc* pDesc) {
  u8 i;
  u16 tmp;
  u8 bytes = sizeof(msgGetParamDesc_t);
  
  print_dbg("\r\n requesting parameter descrtiptor at idx ");
  print_dbg_ulong(paramIdx);

  // set command and idx
  txMsg.getParamDesc.command = MSG_GET_PARAM_DESC_COM;
  txMsg.getParamDesc.idx = paramIdx;

  // write/read
  for(i=0; i<bytes; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, txMsg.raw[i]);
    spi_read(BFIN_SPI, &tmp);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

    print_dbg("\r\n getParamDesc got u16 value: ");
    print_dbg_ulong(tmp);

    rxMsg.raw[i]= (U8)(tmp & 0xff);
  }
  *(pDesc) = rxMsg.getParamDesc.desc;
  print_dbg("\r\n\r\n");
}

*/

/*
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
*/


/*
//// arrg
void bfin_hack(U8 num) {
  U8 i;
  U16 x;
  /// send some dummy bytes over SPI for no god damn fucking reason
  for(i=0; i<num; i++) {        
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, (U8)i);
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    print_dbg("\r\n bfin_hack got u16 value: ");
    print_dbg_ulong(x);
  }
}


*/


// get parameter descriptor
//void bfin_get_param_desc(u16 paramIdx, ParamDesc* pDesc);

