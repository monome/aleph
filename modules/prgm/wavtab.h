//wavtab_common.h

#ifndef _ALEPH_COMMON_WAVTAB_H_
#define _ALEPH_COMMON_WAVTAB_H_

#include "wavtab_common.h"

/*
typedef ...... wavtab
 
wavtab *wavtabpointer
 
wavtab A
wavtab B
 
set to first:
wavtabpointer = &A

*wavtabpointer = FIRST TABLE
 
 
set to new:
wavtabpointer = &B
 
*wavtabpointer = NEW WAVETABLE (B)

A = *wavtabpointer //A is now B

 
 
//wavetable data
typedef struct _wavtabData {
    //wavetable name
    char label[WAVTAB_LABEL_LEN];
    //wavetable data
    u8 waveshapenum;
    u8 wavetabsize;
    u8 wavetabsize_1;    
    u8 shapeshift;
    u8 shapemask;
    u8 waveshift;
    u8 wavemulshift;
    u8 wavemask;
    u8 waveipsnorm;
} WavtabData;

//function protorypes
extern void wavtab_init(void);

extern void wavtab_deinit(void);
*/

#endif

