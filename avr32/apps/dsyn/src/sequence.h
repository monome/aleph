/*
  sequencer.h
  dsyn
  
  defines sequencer logic operations / types
 */

#ifndef _ALEPH_APP_DSYN_SEQUENCE_H_
#define _ALEPH_APP_DSYN_SEQUENCE_H_

#include "types.h"

// FIXME: this should be the width of the grid, set on monome detection, so not a define
#define SEQ_NSTAGES 64
#define SEQ_NSTAGES_1 63

//-------------------------
//-------- functions

// initialize
extern void seq_init(void);
// advance to the next stage (all voices)
extern void seq_advance(void);

//---- setters
// set (jump to) the next stage (all voices)
extern void seq_set_next(u8 x);
// set start
extern void seq_set_start(u8 x);
// set end
extern void seq_set_end(u8 y);

// set length
extern void seq_set_len(u8 n);


// set stage value 
extern void seq_set_stage(u8 vid, u8 sid, u8 val);

// toggle stage value 
extern u8 seq_tog_stage(u8 vid, u8 sid);


//---- getters
// get pointer to stage data for given voice 
extern const u8* seq_get_voice_data(u8 vid);
// get current position
extern const u8 seq_get_pos(void);
// get start
extern const u8 seq_get_start(void);
// get end
extern const u8 seq_get_end(void);
// get length
extern const u8 seq_get_len(void);

// get start position
extern const u8 seq_get_start(void);

// get sequence length
extern const u8 seq_get_len(void);

// get current page
extern const u8 seq_get_page(void);

#endif // h guard
