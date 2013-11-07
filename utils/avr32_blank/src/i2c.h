#ifndef _I2C_H_
#define _I2C_H_

#include "compiler.h"
#include "types.h"

// transmit data in slave mode
extern u8 i2c_slave_tx(void);
// handler for slave rx events
extern void i2c_slave_rx(u8 value);

// slave stop function
extern void i2c_slave_stop(void);

// master send
extern void i2c_master_tx(u8* tx);
// master receive
extern void i2c_master_rx(u8* rx);


#endif // header guard
