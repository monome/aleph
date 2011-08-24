/* module.h
 * bfin-alpeh core audio system
 * 
 * declares prototypes for functinos to be defined in custom modules,
 * and called from the core audio system.
 */

#ifndef _BF533_CORE_MODULE_H_
#define _BF533_CORE_MODULE_H_

#define SDRAM_ADDRESS 0x00000000
#define SDRAM_SIZE    0x07ffffff

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ------ defined in custom module sources!

// intialize
void init_module(void);
// frame processing function
void process_frame(void);
// set parameter functions
// scaled to linear range with unsigned int
void request_param_set_int(unsigned short idx, unsigned long val);
// direct set with float
void request_param_set_float(unsigned short idx, float val, char ramp, char sync);
// get float value
float request_param_get_float(unsigned short index);
// get int value
float request_param_get_int(unsigned short index);

#endif // header guard
