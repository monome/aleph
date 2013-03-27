/* bfin.c
   aleph-avr32

   routines to communicate with the bf533 DSP
*/
//ASF
#include "compiler.h"
#include "gpio.h"
#include "spi.h"
///// DEBUG
#include "print_funcs.h"
/////

// aleph
#include "conf_aleph.h"
#include "filesystem.h"
#include "global.h"
#include "param.h"
#include "protocol.h"
#include "types.h"
#include "util.h"
#include "bfin.h"

// load a blackfin executable
//// need to buffer the LDR file to save memory.
//// so, alas, we must use file I/O from here
#ifdef USE_LDR_RAM_BUFFER

void bfin_load(U32 size, volatile u8 * data) {
  volatile U64 delay;
  U64 byte;
  
  print_dbg("\r\n loading bfin: ");
  print_dbg_ulong(size);
  print_dbg(" bytes...");

  // fixme:
  //// need to use interrupt priorities
  // Disable_global_interrupt();

  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  delay = 1000; while (--delay > 0) {;;}

  gpio_clr_gpio_pin(BFIN_RESET_PIN);
  delay = 30; while (--delay > 0) {;;}

  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  delay = 3000; while (--delay > 0) {;;}
  
  // loop over .ldr data
  byte = 0;
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
   
  while(byte < size) {
    // pause if hwait is held high by the blackfin
    //    while(hwait > 0) { ;; }
    while (gpio_get_pin_value(BFIN_HWAIT_PIN) > 0) { ;; }

    spi_write(BFIN_SPI, data[byte]);

    byte++;
  }
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  /// enable app, ui, 
  // Enable_global_interrupt();

  print_dbg("\r\ndone loading bfin. \r\n");

}

#else

void bfin_load(U32 size, void* fp) {
  u64 i; /// byte index in .ldr
  u8 data;
  volatile U64 delay;
  // FIXME: (?)
  //  Disable_global_interrupt();
  // reset bfin
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  delay = 30; while (--delay > 0) {;;}
  gpio_clr_gpio_pin(BFIN_RESET_PIN);
  delay = 30; while (--delay > 0) {;;}
  gpio_set_gpio_pin(BFIN_RESET_PIN);  
  delay = 3000; while (--delay > 0) {;;}
  // send the .ldr data
  i = 0;
  print_dbg("\r\n loading bfin, ");
  print_dbg_ulong(size);
  print_dbg(" bytes, FP: ");
  print_dbg_hex(fp);
  print_dbg("\r\n");


  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  while(i<size) {
    data = fl_fgetc(fp);

    /* print_dbg_hex(data); */
    /* print_dbg(" "); */
    /* if((i % 16) == 15) { print_dbg("\r\n"); } */

    while (gpio_get_pin_value(BFIN_HWAIT_PIN) > 0) { 
      print_dbg("\r\n HWAIT asserted..."); 
    }
    spi_write(BFIN_SPI, data);
    i++;
  }
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // FIXME: (?)
  //  Enable_global_interrupt();
}

#endif // LDR RAM buffer

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
