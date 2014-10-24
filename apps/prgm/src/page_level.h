//page_level.h
//aleph-prgm-avr32

#ifndef _ALEPH_APP_PRGM_TRACKER_H_
#define _ALEPH_APP_PRGM_TRACKER_H_

#define F_STEPS_MAX 4

char renderCounter[16];

s32 Free0tmp;
s32 Free1tmp;
s32 Free2tmp;
s32 Free3tmp;
s32 Free0;
s32 Free1;
s32 Free2;
s32 Free3;
char renderFree0[16];
char renderFree1[16];
char renderFree2[16];
char renderFree3[16];

s32 Transposed0tmp;
s32 Transposed1tmp;
s32 Transposed2tmp;
s32 Transposed3tmp;
s32 Transposed0;
s32 Transposed1;
s32 Transposed2;
s32 Transposed3;
char renderTransposed0[16];
char renderTransposed1[16];
char renderTransposed2[16];
char renderTransposed3[16];

typedef struct _fstep *Steppointer_f;

typedef struct _fstep {
    Steppointer_f next;
//source
    s32 osc0_f;
    s32 osc1_f;
    s32 osc2_f;
    s32 osc3_f;
    s32 osc0_t;
    s32 osc1_t;
    s32 osc2_t;
    s32 osc3_t;
    
//time
    s32 osc0_ct;
    s32 osc1_ct;
    s32 osc2_ct;
    s32 osc3_ct;
    
//curve
    s32 osc0_c;
    s32 osc1_c;
    s32 osc2_c;
    s32 osc3_c;
    
//destination
    s32 osc0_d;
    s32 osc1_d;
    s32 osc2_d;
    s32 osc3_d;
    
} fstep;

extern void tracker_init(void);

#endif
