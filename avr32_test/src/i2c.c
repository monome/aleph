// asf
#include "print_funcs.h"
#include "twi.h"
// aleph
#include "conf_aleph.h"
#include "i2c.h"

//=========================================
//==== static variables

// this module's chip address.
static u8 addr;
// twi options
static twi_options_t opt;
// status flag
static u8 status;

//======================================
//=== static function declarations
// become master
static u8 init_master(void);
// tx as master
static u8 send_master(u8 chip, u32 addr, u8 addr_len, u32 data_len, void* data);
// become slave (listen)
static u8 init_slave(void);
// slave handlers
static void slave_rx(u8 val);
static u8 slave_tx(void);
static void slave_stop(void);

//==========================================
//==== function definitions

// start up i2c (set address and enter slave mode)
extern void i2c_init(u8 add) {
  i2c_set_address(add);
  status = init_slave();
  print_dbg("\r\nI2C init (slave) : ");
  if(status==TWI_SUCCESS) { print_dbg("SUCCESS"); } else { print_dbg("FAIL: "); print_dbg_hex(status); }   
  print_dbg("\r\nslave address: ");
  print_dbg_hex(addr);
}

// set this unit's chip address
extern void i2c_set_address(u8 address) {
  addr = address;
  //// TODO: deal with it if we're already doing something
}

// switch to master mode and send something, return to slave mode when done.
extern void i2c_tx(u8 chip, u32 addr, u8 addr_len, u32 data_len, void* data) {
  while( twi_is_busy() ) {;;}
  status = init_master();
  print_dbg("\r\nI2C init (master) : ");
  if(status==TWI_SUCCESS) { print_dbg("SUCCESS"); } else { print_dbg("FAIL: "); print_dbg_hex(status); }   
  print_dbg("\r\n chip addr: ");
  print_dbg_hex(chip);
  print_dbg(", mem addr: ");
  print_dbg_hex(addr);
  print_dbg(", addr len: ");
  print_dbg_hex(addr_len);
  print_dbg(", data len: ");
  print_dbg_hex(data_len);
  print_dbg(", data (1st 4 bytes): ");
  print_dbg_hex(*((u32*)(data)));

  status = send_master(chip, addr, addr_len, data_len, data);
  print_dbg("\r\nI2C tx (master) : ");
  if(status==TWI_SUCCESS) { print_dbg("SUCCESS"); } else { print_dbg("FAIL: "); print_dbg_hex(status); }   
  while( twi_is_busy() ) {;;}
  status = init_slave();
  print_dbg("\r\nI2C init (slave) : ");
  if(status==TWI_SUCCESS) { print_dbg("SUCCESS"); } else { print_dbg("FAIL: "); print_dbg_hex(status);}   
}

//---------------------------------------
//----  static function defs

u8 init_master() {
  // options settings
  opt.pba_hz = FPBA_HZ;
  opt.speed = TWI_SPEED;
  opt.chip = addr;
  // initialize TWI driver with options
  return twi_master_init(&AVR32_TWI, &opt);
}

u8 send_master(u8 chip, u32 addr, u8 addr_len, u32 data_len, void* data) {
  static twi_package_t pac;
  // TWI chip address to communicate with
  pac.chip = chip;
  pac.addr_length = addr_len;
  pac.length = data_len;
  pac.addr[0] = addr >> 16;
  pac.addr[1] = addr >> 8;
  pac.addr[2] = addr;
  pac.buffer = data;
  // perform a write access and return status
  return twi_master_write(&AVR32_TWI, &pac);
  // non-blocking...
  //  return twi_master_write_ex(&AVR32_TWI, &pac);
} 

u8 init_slave(void) {
  static twi_slave_fct_t twi_slave_fct;
  opt.pba_hz = FPBA_HZ;
  opt.speed = TWI_SPEED;
  opt.chip = addr;
  twi_slave_fct.rx = &slave_rx;
  twi_slave_fct.tx = &slave_tx;
  twi_slave_fct.stop = &slave_stop;
  return twi_slave_init(&AVR32_TWI, &opt, &twi_slave_fct );
}

void slave_rx(u8 val) {
  print_dbg("\r\n i2c slave receive: ");
  print_dbg_char_hex(val);
}

u8 slave_tx(void) {
  //...
  return 0;
}

void slave_stop(void) {
  //...  
}
