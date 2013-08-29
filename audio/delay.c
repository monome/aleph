/*
  delayline.c
  aleph - audio

  implement delay line using buffer and tap objects
  
 */

#include "conversion.h"
#include "delay.h"


// initialize with pointer to audio buffer
extern void delay_init(delayLine* dl, fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  buffer_tapN_init(&(dl->tapRd), &(dl->buffer));
  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));
  dl->tapWr.idx = 10000; // ???
  dl->tapRd.idx = 0;
  dl->preLevel = 0;
  dl->write = 1;
}

extern fract32 delay_next(delayLine* dl, fract32 in) {
  fract32 readVal;

  // get read value first.
  // so, setting loop == delaytime gives sensible results.
  readVal = buffer_tapN_read( &(dl->tapRd) );

  // figure out what to write
  if(dl->preLevel == 0) {
    if(dl->write) {
      // write and replace
      buffer_tapN_write(&(dl->tapWr), in);
    } else {
      // clear
      buffer_tapN_write(&(dl->tapWr), 0);
    }
  } else if(dl->preLevel < 0) { // consider <0 to be == 1
    if(dl->write) {
      // overdub
      buffer_tapN_add(&dl->tapWr, in);
    } else {
      // no change
      ;;
    }
  } else { // prelevel is non-zero, non-full
    if(dl->write) {
      // write mix
      buffer_tapN_mix(&(dl->tapWr), in, dl->preLevel);
    } else {
      // attenuate only
      buffer_tapN_mix(&(dl->tapWr), 0, dl->preLevel);

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
extern void delay_set_loop(delayLine* dl, fix16 sec) {
  u32 samps = sec_to_frames_trunc(sec);
  dl->tapRd.loop = samps - 1;
  dl->tapWr.loop = samps - 1;
}

extern void delay_set_delay(delayLine* dl, fix16 sec) {
  buffer_tapN_sync(&(dl->tapRd), &(dl->tapWr), sec);
}

// set erase level
extern void delay_set_pre(delayLine* dl, fract32 pre) {
  dl->preLevel = pre;
}

// set write level
extern void delay_set_write(delayLine* dl, u8 write) {
  dl->write = write;
}

// set read head rate
extern void delay_set_rate(delayLine* dl, fix16 rate) {
  ///...
}


// set read pos in seconds
extern void delay_set_pos_read(delayLine* dl, fix16 sec) {
  buffer_tapN_set_pos(&(dl->tapRd), sec);
}

// set write pos in seconds
extern void delay_set_pos_write(delayLine* dl, fix16 sec) {
  buffer_tapN_set_pos(&(dl->tapWr), sec);
}

// set read run flag in seconds
extern void delay_set_run_read(delayLine* dl, u8 val) {
  dl->runRd = val;
}

// set write run flag in seconds
extern void delay_set_run_write(delayLine* dl, u8 val) {
  dl->runWr = val;
}

