/*
  sequencer.h
  dsyn
  
  defines sequencer logic operations / types
 */

#ifndef _ALEPH_APP_DSYN_SEQUENCE_H_
#define _ALEPH_APP_DSYN_SEQUENCE_H_

#define SEQ_NSTAGES 64

//----- datatypes
typedef struct _seq {
  //...
} seq;
//-------------------------
//-------- functions

// init
extern void seq_init(void);

// advance to the next stage for all voices
extern void seq_next(void);

// set stage value 
extern void seq_set_stage(uvid, 8u8 sid);




#endif // h guard
