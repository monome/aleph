//tracker.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_TRACKER_H_
#define _ALEPH_APP_PRGM_TRACKER_H_

#define F_STEPS_MAX 4
#define T_STEPS_MAX 4

s32 Freq0tmp;
s32 Freq1tmp;
s32 Freq2tmp;
s32 Freq3tmp;
s32 Freq0;
s32 Freq1;
s32 Freq2;
s32 Freq3;
char renderFreq0[16];
char renderFreq1[16];
char renderFreq2[16];
char renderFreq3[16];

s32 Transpose0tmp;
s32 Transpose1tmp;
s32 Transpose2tmp;
s32 Transpose3tmp;
s32 Transpose0;
s32 Transpose1;
s32 Transpose2;
s32 Transpose3;
char renderTranspose0[16];
char renderTranspose1[16];
char renderTranspose2[16];
char renderTranspose3[16];

typedef struct _fstep *Steppointer_f;
typedef struct _tstep *Steppointer_t;

typedef struct _fstep {
    Steppointer_f next;
    s32 osc0_f;
    s32 osc1_f;
    s32 osc2_f;
    s32 osc3_f;
} fstep;

typedef struct _tstep {
    Steppointer_t next;
    s32 osc0_t;
    s32 osc1_t;
    s32 osc2_t;
    s32 osc3_t;
} tstep;

extern void tracker_init(void);

#endif
