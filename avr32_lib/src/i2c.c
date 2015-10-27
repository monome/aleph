#include "print_funcs.h"
#include "twi.h"
#include "i2c.h"
//#include "events.h"

static twi_package_t packet;

void i2c_master_tx(uint8_t addr, uint8_t idx, int data) {
    int status;
    uint8_t d[3];
    
    d[0] = idx;
    d[1] = data >> 8;
    d[2] = data & 0xff;
    
    // TWI chip address to communicate with
    // packet.chip = EEPROM_ADDRESS;
    packet.chip = addr;
    // TWI address/commands to issue to the other chip (node)
    // packet.addr[0] = VIRTUALMEM_ADDR_START >> 16;
    // packet.addr[1] = VIRTUALMEM_ADDR_START >> 8;
    // packet.addr[2] = VIRTUALMEM_ADDR_START;
    // Length of the TWI data address segment (1-3 bytes)
    // packet.addr_length = EEPROM_ADDR_LGT;
    packet.addr_length = 0;
    // Where to find the data to be written
    packet.buffer = &d;
    // How many bytes do we want to write
    packet.length = 3;
    
    // perform a write access
    status = twi_master_write(TWI, &packet);
    
    // // check write result
    // if (status == TWI_SUCCESS)
    //   print_dbg("\r\nWrite test:\tPASS");
    // else
    //   print_dbg("\r\nWrite test:\tFAIL");
    
    
    /*
     // TWI chip address to communicate with
     packet_received.chip = EEPROM_ADDRESS ;
     // Length of the TWI data address segment (1-3 bytes)
     packet_received.addr_length = EEPROM_ADDR_LGT;
     // How many bytes do we want to write
     packet_received.length = 6;
     // TWI address/commands to issue to the other chip (node)
     packet_received.addr[0] = VIRTUALMEM_ADDR_START >> 16;
     packet_received.addr[1] = VIRTUALMEM_ADDR_START >> 8;
     packet_received.addr[2] = VIRTUALMEM_ADDR_START;
     // Where to find the data to be written
     packet_received.buffer = data_received;
     // perform a read access
     status = twi_master_read(TWI, &packet_received);
     // check read result
     if (status == TWI_SUCCESS)
     print_dbg("\r\nRead Test:\tPASS");
     else
     print_dbg("\r\nRead test:\tFAIL");
     // check received data against sent data
     for (int i = 0 ; i < 6; i++)
     {
     if (data_received[i] != test_pattern[i])
     print_dbg("\r\nCheck Read:\tFAIL");
     }
     */
    
}

void twi_slave_rx( U8 u8_value )
{
    print_dbg("\r\ntwi_rx: ");
    print_dbg_hex(u8_value);
}

U8 twi_slave_tx( void )
{
    print_dbg("\r\ntwi_tx");
    return 0;
}

void twi_slave_stop( void )
{
    print_dbg("\r\ntwi_stop");
}

// this module's chip address.
static u8 addr;
// twi options
static twi_options_t opt;
// status flag
static u8 status;

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
    // return twi_master_write_ex(&AVR32_TWI, &pac);
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

//==========================================
//==== function definitions

/*
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
*/
