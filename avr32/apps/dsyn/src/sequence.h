/*
  sequencer.h
  dsyn
  
  defines sequencer logic operations / types
 */

#ifndef _ALEPH_APP_DSYN_SEQUENCE_H_
#define _ALEPH_APP_DSYN_SEQUENCE_H_

#include "types.h"

#define SEQ_NSTAGES 64
#define SEQ_NSTAGES_1 63

//-------------------------
//-------- functions

// initialize
extern void seq_init(void);

// advance to the next stage (all voices)
extern void seq_advance(void);

// set (jump to) the next stage (all voices)
extern void seq_set_next(u8 x);

// set stage value 
extern void seq_set_stage(u8 vid, u8 sid, u8 val);

// toggle stage value 
extern u8 seq_tog_stage(u8 vid, u8 sid);

// get pointer to stage data for given voice 
extern const u8* seq_get_voice_data(u8 vid);

// get current position
extern const u8 seq_get_pos(void);

#endif // h guard
