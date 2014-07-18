//prgm.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_PRGM_H_
#define _ALEPH_APP_PRGM_PRGM_H_

s32 Wave0;
s32 Wave1;
s32 Wave2;
s32 Wave3;

s32 Phase0; //convert to arrays, phase[4], blend[4] etc..
s32 Phase1;
s32 Phase2;
s32 Phase3;

s32 Blend0;
s32 Blend1;
s32 Blend2;
s32 Blend3;

char renderWave0[16];
char renderWave1[16];
char renderWave2[16];
char renderWave3[16];

char renderPhase0[16];
char renderPhase1[16];
char renderPhase2[16];
char renderPhase3[16];

char renderBlend0[16];
char renderBlend1[16];
char renderBlend2[16];
char renderBlend3[16];

u8 state_sw;

#endif 
