#ifndef _ALEPH_AVR32_APP_DSYN_FILES_H_
#define _ALEPH_AVR32_APP_DSYN_FILES_H_

#define DSYN_PARAMS_SERIAL_BYTES (PARAM_VOICE_NPARAMS * DSYN_NVOICES)

// search for our .ldr file in the filesystem
u8 files_search_dsp(void);

// write parameter settings to file
void files_write_params(void);

// get parameter settings from file
void files_read_params(void);

#endif 


