//prgm
//aleph-avr32

#ifndef _ALEPH_APP_PRGM_FILES_H_
#define _ALEPH_APP_PRGM_FILES_H_

#define DEFAULT_LDR "/mod/aleph-prgm.ldr"
#define DEFAULT_DSC "/mod/aleph-prgm.dsc"

// load aleph-prgm.ldr, return 1 on success, 0 on failure
extern u8 files_load_dsp(void);

//extern u8 files_load_wavetable(const char* name);

//extern u8 files_load_dsp_parameters(void);

 
#endif 


