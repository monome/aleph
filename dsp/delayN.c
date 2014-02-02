/*
  delayN.c
  aleph - audio

  implement delay line using buffer and tap objects. non-interpolated version.
  
 */

#include "conversion.h"
#include "delayN.h"

// initialize with pointer to audio buffer
extern void delayN_init(delayN* dl, fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  buffer_tapN_init(&(dl->tapRd), &(dl->buffer));
  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));

  dl->tapWr.idx = 0;
  dl->tapRd.idx = 0;

  dl->tapWr.loop = frames;
  dl->tapRd.loop = frames;

  dl->preLevel = 0;
  dl->write = 1;
}

extern fract32 delayN_next(delayN* dl, fract32 in) {
  fract32 readVal;

  // get read value first.
  // so, setting loop == delayNtime gives sensible results (???)
  readVal = buffer_tapN_read( &(dl->tapRd) );

  // figure out what to write
  if(dl->preLevel == 0) {
    if(dl->write) {
      // write and replace
      buffer_tapN_write(&(dl->tapWr), in);
    }
  } else if(dl->preLevel < 0) { // consider <0 to be == 1
    if(dl->write) {
      // overdub
      buffer_tapN_add(&(dl->tapWr), in);
    }
  } else { // prelevel is non-zero, non-full
    if(dl->write) {
      // write mix
      buffer_tapN_mix(&(dl->tapWr), in, dl->preLevel);
    }
  }

  // advance the read phasor 
  if(dl->runRd) {
    buffer_tapN_next( &(dl->tapRd) );
  }

  // advance the write phasor
  if(dl->runWr) {
    buffer_tapN_next( &(dl->tapWr) );
  }
  
  return readVal;
}

// set loop endpoint in seconds
extern void delayN_set_loop_sec(delayN* dl, fix16 sec) {
  u32 samps = sec_to_frames_trunc(sec);
  buffer_tapN_set_loop(&(dl->tapRd), samps - 1);
  buffer_tapN_set_loop(&(dl->tapWr), samps - 1);
}

// set loop endpoint in samples
extern void delayN_set_loop_samp(delayN* dl, u32 samps) {
  dl->tapRd.loop = samps - 1;
  dl->tapWr.loop = samps - 1;
}

// set delayN in seconds
extern void delayN_set_delayN_sec(delayN* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tapN_sync(&(dl->tapRd), &(dl->tapWr), samp);
}

// set delayN in samples
extern void delayN_set_delayN_samp(delayN* dl, u32 samp) {
  buffer_tapN_sync(&(dl->tapRd), &(dl->tapWr), samp);
}

// set erase level
extern void delayN_set_pre(delayN* dl, fract32 pre) {
  dl->preLevel = pre;
}

// set write level
extern void delayN_set_write(delayN* dl, u8 write) {
  dl->write = write;
}

// set read pos in seconds
extern void delayN_set_pos_read_sec(delayN* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tapN_set_pos(&(dl->tapRd), samp);
}
extern void delayN_set_pos_read_samp(delayN* dl, u32 samp) {
  buffer_tapN_set_pos(&(dl->tapRd), samp);
}

// set write pos in seconds
extern void delayN_set_pos_write_sec(delayN* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tapN_set_pos(&(dl->tapWr), samp);
}

extern void delayN_set_pos_write_samp(delayN* dl, u32 samp) {
  buffer_tapN_set_pos(&(dl->tapWr), samp);
}

// set read run flag 
extern void delayN_set_run_read(delayN* dl, u8 val) {
  dl->runRd = val;
}

// set write run flag
extern void delayN_set_run_write(delayN* dl, u8 val) {
  dl->runWr = val;
}

// set read-head rate multiplier
void delayN_set_mul(delayN* dl, u32 val, u8 id) {
  // different terms, dumb...
  buffer_tapN_set_inc( &(dl->tapRd[id]), val );

}

// set read-head rate divider
void delayN_set_div(delayN* dl, u32 val, u8 id) {
  buffer_tapN_set_div( &(dl->tapRd[id]), val );
}
