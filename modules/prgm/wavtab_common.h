//wavtab_common.h

#ifndef _ALEPH_COMMON_WAVTAB_H_
#define _ALEPH_COMMON_WAVTAB_H_

//max characters in wavetable name
#define WAVTAB_LABEL_LEN 8

#define WAVE_SHAPE_NUM 2
#define WAVE_TAB_SIZE 1024

/*
 __attribute__((__packed__)) marks a variable or struct element for packing into the smallest space available, omitting any alignment bytes usually added between fields to optimize field accesses.
 */

typedef const fract32 (*WavtabData)[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];

//static const fract32 wavtab[WAVE_SHAPE_NUM][WAVE_TAB_SIZE] = { 
//#include "wavtab_wf0.c"
//};


//typedef wavtab (*wavtab_t);

//wavtabData = &wavtab;

//static const fract32 wavtab[WAVE_SHAPE_NUM][WAVE_TAB_SIZE] = { 
//#include "wavtab_wf0.c"
//};

//wavetable data
//typedef struct __attribute__((__packed__)) WavtabDataStruct {
    //wavetable name
//    char label[WAVTAB_LABEL_LEN];
    //wavetable data
//    wavtab_t tab;
//    u8 waveshapenum;
//    u8 wavetabsize;
//    u8 wavetabsize_1;    
//    u8 shapeshift;
//    u8 shapemask;
//    u8 waveshift;
//    u8 wavemulshift;
//    u8 wavemask;
//    u8 waveipsnorm;
//} WavtabData;

#endif
