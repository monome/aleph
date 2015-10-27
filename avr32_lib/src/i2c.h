#ifndef _I2C_H_
#define _I2C_H_

#include "compiler.h"
#include "types.h"


extern void i2c_master_tx(uint8_t, uint8_t, int);
extern void twi_slave_rx( U8 u8_value );
extern U8 twi_slave_tx( void );
extern void twi_slave_stop( void );

/*
extern void i2c_master_tx(void);
extern void i2c_master_tx(void);

extern void twi_slave_rx( U8 u8_value );
extern U8 twi_slave_tx( void );
extern void twi_slave_stop( void );

typedef void (*process_ii_t)(uint8_t, int);
extern volatile process_ii_t process_ii;
*/
/*
extern void i2c_master_tx(uint8_t, uint8_t, int);
extern void twi_slave_rx( U8 u8_value );
extern U8 twi_slave_tx( void );
extern void twi_slave_stop( void );
*/

/*
// start up i2c (set address and enter slave mode)
extern void i2c_init(u8 address);
// set this unit's chip address
extern void i2c_set_address(u8 address);
// switch to master mode and send something, return to slave mode when done.
extern void i2c_tx(u8 chip, u32 addr, u8 addr_len, u32 data_len, void* data);
*/
 
#endif // header guard
