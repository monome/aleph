#ifndef _ALEPH_APP_DSYN_SEQUENCE_H_
#define _ALEPH_APP_DSYN_SEQUENCE_H_

//-------------------------
//-------- functions

// init
extern void seq_init(void);

// advance to the next stage
extern void seq_next(void);

// set stage value (on/off + ?)
extern void seq_set_stage(u8 id);

#endif // h guard
