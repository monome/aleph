 /* bfin.c
   aleph-avr32

   routines to communicate with bf533 DSP over SPI
   follow protocol at aleph/common/protocol.h
*/

//ASF
#include "compiler.h"
#include "delay.h"
#include "gpio.h"
#include "spi.h"
#include "print_funcs.h"

// aleph/common
#include "param_common.h"
#include "protocol.h"

// aleph/avr32
#include "aleph_board.h"
#include "filesystem.h"
#include "flash.h"
#include "global.h"
#include "types.h"
#include "util.h"
#include "bfin.h"

//--------------------------------------
//--- static

static void bfin_start_transfer(void);
static void bfin_end_transfer(void); 
static void bfin_transfer_byte(u8 data);

static void bfin_transfer_byte(u8 data) {
    while (gpio_get_pin_value(BFIN_HWAIT_PIN) > 0) { 
      print_dbg("\r\n HWAIT asserted..."); 
    }
    spi_write(BFIN_SPI, data);
}

void bfin_start_transfer(void) {
  //  volatile u64 delay;
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  //  delay = 30; while (--delay > 0) {;;}
  delay_ms(1);
  gpio_clr_gpio_pin(BFIN_RESET_PIN);
  //  delay = 30; while (--delay > 0) {;;}
  delay_ms(1);
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  //  delay = 3000; while (--delay > 0) {;;}
  delay_ms(1);
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
}

void bfin_end_transfer(void) {
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  print_dbg("\r\n done loading; waiting... ");
  delay_ms(100);
  print_dbg("\r\n done waiting; reporting... ");
  bfin_report_params();
}

//---------------------------------------
//--- extern

// load bfin executable from the RAM buffer
void bfin_load_buf(void) {
  u64 i; /// byte index in .ldr

  if(bfinLdrSize > BFIN_LDR_MAX_BYTES) {
    print_dbg("\r\n bfin load error: size : "); print_dbg_hex(bfinLdrSize);
    return;
  }

  bfin_start_transfer();

  for(i=0; i<bfinLdrSize; i++) {
    bfin_transfer_byte(bfinLdrData[i]);
  }

  bfin_end_transfer();
}

//void bfin_set_param(u8 idx, f32 x ) {
void bfin_set_param(u8 idx, fix16_t x ) {

  static ParamValue pval;
  pval.asInt = (s32)x;

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
  pDesc->min = pval.asInt;
  // read max
  for(i=0; i<4; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    pval.asByte[i] = (u8)(x & 0xff);
  }
  pDesc->max = pval.asInt;
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

// get module version
void bfin_get_module_version(moduleVersion_t* vers) {
  //  u16 x;
  
  /* // command  */
  /* spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* spi_write(BFIN_SPI, MSG_GET_MODULE_VERSION_COM); */
  /* spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* // major */
  /* spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* spi_read(BFIN_SPI, &x); */
  /* spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* vers->maj = x; */
  /* // minor */
  /* spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* spi_read(BFIN_SPI, &x); */
  /* spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* vers->min = x; */
  /*   // rev high */
  /* spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* spi_read(BFIN_SPI, &x); */
  /* spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* vers->rev |= (x << 8) & 0xff; */
  /*   // rev low */
  /* spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* spi_read(BFIN_SPI, &x); */
  /* spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS); */
  /* vers->rev |= x & 0xff; */
}


// clear and add params to ctl network
void bfin_report_params(void) {
  volatile ParamDesc pdesc;
  u32 numParams;
  u8 i;

  bfin_get_num_params(&numParams);
  print_dbg("\r\nnumparams: ");
  print_dbg_ulong(numParams);

  if(numParams > 0) {
    net_clear_params();
    for(i=0; i<numParams; i++) {
      bfin_get_param_desc(i, &pdesc);

      net_add_param(i, &pdesc);

      print_dbg("\r\n got pdesc : ");
      print_dbg((const char* )pdesc.label);
    }
  }
}
