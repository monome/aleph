//wavtab_common.h

#ifndef _ALEPH_COMMON_WAVTAB_H_
#define _ALEPH_COMMON_WAVTAB_H_

//max characters in wavetable name
#define WAVTAB_LABEL_LEN 8

#define WAVE_SHAPE_NUM 2
#define WAVE_TAB_SIZE 512

#define WAVE_TAB_SIZE_1 (WAVE_TAB_SIZE - 1)

#define WAVE_SHAPE_IDX_SHIFT 16
#define WAVE_SHAPE_MASK (1 << (WAVE_SHAPE_IDX_SHIFT)) - 1 //mask to get interpolation multiplier

#define WAVE_IDX_SHIFT 21 // rshift to get table index
#define WAVE_IDX_MUL_SHIFT (WAVE_IDX_SHIFT) - 15 // rshift multiplier to fract16
#define WAVE_IDX_MASK (1 << (WAVE_IDX_SHIFT)) - 1 // mask to get interpolation multiplier

#define WAVE_IPS_NORM 0xae3c //44604, normalized IPS for unipolar 1.31 phase, 48k (0xbb80) sampling rate


typedef fract32 (*WavtabData)[WAVE_SHAPE_NUM][WAVE_TAB_SIZE];

/*
typedef struct _wavtab {
    char name[WAVTAB_LABEL_LEN];
    WavtabData wavtabData;
    
} Wavtab;
*/
 
//typedef struct __attribute__((__packed__)) _wavtabs {
typedef struct _wavtabs {
    WavtabData wavtab0;
    WavtabData wavtab1;
    WavtabData wavtab2;
    WavtabData wavtab3;

} Wavtabs;

#endif

/*
__attribute__((__packed__)) marks a variable or struct element for packing into the smallest space available, omitting any alignment bytes usually added between fields to optimize field accesses.
*/

//    u8 waveshapenum;
//    u8 wavetabsize;
//    u8 wavetabsize_1;    
//    u8 shapeshift;
//    u8 shapemask;
//    u8 waveshift;
//    u8 wavemulshift;
//    u8 wavemask;
//    u8 waveipsnorm;

/*
init_wavtab(WavtabData mWavtabData[i]) {
int s = 0;

while (s < WAVE_TAB_SIZE) {
    mWavtabData[0][s] = 0x00000000;
    s++;
}

while (s < WAVE_TAB_SIZE) {
    mWavtabData[1][s] = 0x00000000;
    s++;
}
*/

