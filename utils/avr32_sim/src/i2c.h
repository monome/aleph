#ifndef _I2C_H_
#define _I2C_H_

//#include "compiler.h"
#include "types.h"

// start up i2c (set address and enter slave mode)
extern void i2c_init(u8 address);
// set this unit's chip address
extern void i2c_set_address(u8 address);
// switch to master mode and send something, return to slave mode when done.
extern void i2c_tx(u8 chip, u32 addr, u8 addr_len, u32 data_len, void* data);

#endif // header guard
