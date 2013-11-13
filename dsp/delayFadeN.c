/*
  delayFadeN.c
  aleph - audio

  implement delay line using buffer and tap objects

  this one uses no interpolation, 
  and has two taps per read head to enable crossfading
  
 */

#include "conversion.h"
#include "delayFadeN.h"

// initialize with pointer to audio buffer
extern void delayFadeN_init(delayFadeN* dl, fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  buffer_tapN_init(&(dl->tapRd), &(dl->buffer));
  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));

  dl->tapWr.idx = 0;
  dl->tapRdA.idx = 0;
  dl->tapRdB.idx = 0;

  dl->tapWr.loop = frames;
  dl->tapRdA.loop = frames;
  dl->tapRdB.loop = frames;

  dl->preLevel = 0;
  dl->write = 1;
}

extern fract32 delayFadeN_next(delayFadeN* dl, fract32 in) {
  fract32 readVal;

  // get read value first.
  // so, setting loop == delayFadeNtime gives sensible results (???)
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
extern void delayFadeN_set_loop_sec(delayFadeN* dl, fix16 sec) {
  u32 samps = sec_to_frames_trunc(sec);
  buffer_tapN_set_loop(&(dl->tapRd), samps - 1);
  buffer_tapN_set_loop(&(dl->tapWr), samps - 1);
}

// set loop endpoint in samples
extern void delayFadeN_set_loop_samp(delayFadeN* dl, u32 samps) {
  dl->tapRd.loop = samps - 1;
  dl->tapWr.loop = samps - 1;
}

// set delayFadeN in seconds
extern void delayFadeN_set_delayFadeN_sec(delayFadeN* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tapN_sync(&(dl->tapRd), &(dl->tapWr), samp);
}

// set delayFadeN in samples
extern void delayFadeN_set_delayFadeN_samp(delayFadeN* dl, u32 samp) {
  buffer_tapN_sync(&(dl->tapRd), &(dl->tapWr), samp);
}

// set erase level
extern void delayFadeN_set_pre(delayFadeN* dl, fract32 pre) {
  dl->preLevel = pre;
}

// set write level
extern void delayFadeN_set_write(delayFadeN* dl, u8 write) {
  dl->write = write;
}

// set read head rate
extern void delayFadeN_set_rate(delayFadeN* dl, fix16 rate) {
  ///...
}

// set read pos in seconds
extern void delayFadeN_set_pos_read_sec(delayFadeN* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tapN_set_pos(&(dl->tapRd), samp);
}
extern void delayFadeN_set_pos_read_samp(delayFadeN* dl, u32 samp) {
  buffer_tapN_set_pos(&(dl->tapRd), samp);
}

// set write pos in seconds
extern void delayFadeN_set_pos_write_sec(delayFadeN* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tapN_set_pos(&(dl->tapWr), samp);
}

extern void delayFadeN_set_pos_write_samp(delayFadeN* dl, u32 samp) {
  buffer_tapN_set_pos(&(dl->tapWr), samp);
}


// set read run flag 
extern void delayFadeN_set_run_read(delayFadeN* dl, u8 val) {
  dl->runRd = val;
}

// set write run flag
extern void delayFadeN_set_run_write(delayFadeN* dl, u8 val) {
  dl->runWr = val;
}
