//tracker.h

#ifndef _ALEPH_APP_PRGM_TRACKER_H_
#define _ALEPH_APP_PRGM_TRACKER_H_

#define N_STEPS 8

s32 Freq0;
s32 Freq1;
s32 Freq2;
s32 Freq3;

char renderFreq0[16];
char renderFreq1[16];
char renderFreq2[16];
char renderFreq3[16];

extern void tracker_init(void);

typedef struct _tstep *Steppointer;

typedef struct _tstep {
    Steppointer next;
    s32 osc0_f;
    s32 osc1_f;
    s32 osc2_f;
    s32 osc3_f;
} tstep;

#endif
