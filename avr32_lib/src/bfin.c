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
#include "module_common.h"
#include "param_common.h"
#include "protocol.h"

// aleph/avr32
#include "aleph_board.h"
#include "app.h"
#include "filesystem.h"
//#include "flash.h"
#include "global.h"
#include "interrupts.h"
#include "screen.h"
#include "types.h"
//#include "util.h"
#include "bfin.h"

//--------------------------------------
//--- static fuction declaration

static void bfin_start_transfer(void);
static void bfin_end_transfer(void); 
static void bfin_transfer_byte(u8 data);
static u32 ldrCurrentByte = 0;

//---------------------------------------
//--- external function definition

// wait for busy pin to clear
void bfin_wait(void) {
  //  print_dbg("\r\n hwait: ");
  //  print_dbg_ulong(gpio_get_pin_value(BFIN_HWAIT_PIN));
    while (gpio_get_pin_value(BFIN_HWAIT_PIN) > 0) { 
      ;;
      //      print_dbg("\r\n HWAIT asserted..."); 
	    //            delay_ms(1);
    }
    delay_us(50);
}

// load bfin executable from the RAM buffer
void bfin_load_buf(const u8* data, const u32 size) {
  u64 i; 

  app_pause();

  ldrCurrentByte = 0;
  bfin_start_transfer();

  for(i=0; i<size; i++) {
    //    bfin_transfer_byte(bfinLdrData[i]);
    bfin_transfer_byte(data[ldrCurrentByte]);
    ldrCurrentByte++;
  }

  bfin_end_transfer();
 
  app_resume();
}

//void bfin_set_param(u8 idx, f32 x ) {
void bfin_set_param(u8 idx, fix16_t x ) {
  //static u32 ticks = 0;
  ParamValueSwap pval;
  pval.asInt = (s32)x;

  //  print_dbg("\r\n bfin_set_param, idx: ");
  //  print_dbg_ulong(idx);
  //  print_dbg(",\t val: 0x");
  //  print_dbg_hex((u32)x);
  
  /*
    print_dbg(", \t elapsed ms: ");
    print_dbg_ulong(tcTicks - ticks);
    print_dbg("\r\n");
    ticks = tcTicks;    
  */

  //  app_pause();

  // command
  bfin_wait();
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_SET_PARAM_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  //idx
  bfin_wait();
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, idx);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // val0
  bfin_wait();
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, pval.asByte[0]);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  // val1
  bfin_wait();
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, pval.asByte[1]);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  //val2
  bfin_wait();
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, pval.asByte[2]);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  //val3
  bfin_wait();
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, pval.asByte[3]);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  //  app_resume();
}

void bfin_get_num_params(volatile u32* num) {
  u16 x;

  app_pause();

  // command 
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_GET_NUM_PARAMS_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  print_dbg("\r\n : spi_write MSG_GET_NUM_PARAMS");

  // read num
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, 0); //dont care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);  
  *num = (u8)(x & 0xff);

  print_dbg("\r\n : spi_read numparams: ");
  print_dbg_ulong(*num);

  app_resume();

}



/// moving param descriptor offline
#if 1
//void bfin_get_param_desc(u16 paramIdx, volatile ParamDesc* pDesc) {
  //...
//}
#else 
void bfin_get_param_desc(u16 paramIdx, volatile ParamDesc* pDesc) {
  ParamValueSwap pval;
  u16 x; // u16 for spi_read()
  u8 i;

  app_pause();
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
  /*
    //// don't need with new type system... didn't exactly need anyways
  // read unit
  for(i=0; i<PARAM_UNIT_LEN; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    pDesc->unit[i] = (char)(x & 0xff);
  }
  */
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

  // read radix
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, 0); //dont care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  pDesc->radix = (u8)(x & 0xff);

  app_resume();
}
#endif


// get module name
void bfin_get_module_name(volatile char* buf) {
  u16 x; // u16 for spi_read()
  u8 i;

  app_pause();

  // command 
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_GET_MODULE_NAME_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  for(i=0; i<MODULE_NAME_LEN; i++) {
    spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
    spi_read(BFIN_SPI, &x);
    spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
    buf[i] = (char)(x & 0xff);
  }

  app_resume();
}

// get module version
void bfin_get_module_version(ModuleVersion* vers) {
  u16 x;
  
  app_pause();

  // command
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_GET_MODULE_VERSION_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  // major
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  vers->maj = x;

  // minor
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  vers->min = x;

  // rev
  vers->rev = 0;
  // rev high
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  vers->rev |= ((x << 8) & 0xff00);

    // rev low
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
    spi_write(BFIN_SPI, 0); //dont care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  vers->rev |= (x & 0x00ff);

  app_resume();
}


void bfin_enable(void) {
  // enable audio processing
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_ENABLE_AUDIO);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
}

void bfin_disable(void) {
  // disable audio processing
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_DISABLE_AUDIO);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
}

//---------------------------------------------
//------ static function definition

static void bfin_transfer_byte(u8 data) {
  bfin_wait();
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
}

// wait for ready status (e.g. after module init)
void bfin_wait_ready(void) {
  // use ready pin
  while( !gpio_get_pin_value(BFIN_READY_PIN) ) { 
    ;;    //    print_dbg("\r\n wait bfin_ready ");
  }
  //  print_dbg("... waited");
}

// get parameter value
s32 bfin_get_param(u8 idx) {
  ParamValueSwap pval;
  u16 x;
  
  app_pause();

  // command
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_GET_PARAM_COM);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  // idx
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, idx);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  /// read value
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, 0); // don't care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  pval.asByte[0] = (u8)x;

  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, 0); // don't care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  pval.asByte[1] = (u8)x;

  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, 0); // don't care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  pval.asByte[2] = (u8)x;

  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, 0); // don't care
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);
  pval.asByte[3] = (u8)x;

  app_resume();

  return pval.asInt;
  
}

// fill a buffer on the blackfin with arbitrary data
void bfin_fill_buffer(const s32* src, u32 bytes) {
  u16 x;

  app_pause();
  spi_selectChip(BFIN_SPI, BFIN_SPI_NPCS);
  spi_write(BFIN_SPI, MSG_FILL_BUFFER_COM);
  spi_read(BFIN_SPI, &x);
  spi_unselectChip(BFIN_SPI, BFIN_SPI_NPCS);

  app_resume();
}
