/*
  delayFadeN.c
  aleph - audio

  implement delay line using buffer and tap objects

  this one uses no interpolation, 
  and has two taps per read head to enable crossfading
  
 */

#include "conversion.h"
#include "delayFadeN.h"
#include "pan.h"

// initialize with pointer to audio buffer
extern void delayFadeN_init(delayFadeN* dl, volatile fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  
  buffer_tapN_init(&(dl->tapRd[1]), &(dl->buffer));
  buffer_tapN_init(&(dl->tapWr[1]), &(dl->buffer));

  buffer_tapN_init(&(dl->tapRd[0]), &(dl->buffer));
  buffer_tapN_init(&(dl->tapWr[0]), &(dl->buffer));

  dl->tapWr[0].idx = 0;
  dl->tapWr[1].idx = 0;
  dl->tapRd[0].idx = 0;
  dl->tapRd[1].idx = 0;

  dl->tapWr[0].loop = frames;
  dl->tapWr[1].loop = frames;
  dl->tapRd[0].loop = frames;
  dl->tapRd[1].loop = frames;

  dl->preLevel = 0;
  dl->write = 1;


  //??
  dl->fadeRd = 0;
  dl->fadeWr = 0;
  //  dl->fadeRd = FR32_MAX;
  //  dl->fadeWr = FR32_MAX;
  
  
}

extern fract32 delayFadeN_next(delayFadeN* dl, fract32 in) {
  fract32 readVal;
  fract32 pan[2] = { 0, 0 };
  fract32 valWr[2] = { 0, 0 };

  // get read value first.
  // so, setting loop == delayFadeNtime gives "sensible" results...

  //  readVal = buffer_tapN_read( &(dl->tapRd) );
  /// balanced mix given current pan  
  readVal = pan_lin_mix( buffer_tapN_read( &(dl->tapRd[0]) ) ,
		     buffer_tapN_read( &(dl->tapRd[1]) ) ,
		     dl->fadeRd
		     );

  // get mix amounts for crossfaded write heads
  ///  pan_lin_coeff( &(pan[0]), &(pan[1]), dl->fadeWr );

  //  valWr[0] = mult_fr1x32x32(in, pan[0]);
  //  valWr[1] = mult_fr1x32x32(in, pan[1]);

  /// FIXME: use single write head for now.
  /// need to make them toggle run/stop depending on fade level.
  valWr[0] = in;
  

  // figure out how to write/add/mix
  if(dl->preLevel == 0) {
    if(dl->write) {
      // write and replace
      buffer_tapN_write(&(dl->tapWr[0]), valWr[0]);
      // FIXME
      //      buffer_tapN_write(&(dl->tapWr[1]), valWr[1]);
    }
  } else if(dl->preLevel < 0) { // consider <0 to be == 1
    if(dl->write) {
      // overdub
      buffer_tapN_add(&(dl->tapWr[0]), valWr[0]);
      // FIXME
      //      buffer_tapN_add(&(dl->tapWr[1]), valWr[1]);
    }
  } else { // prelevel is non-zero, non-full
    if(dl->write) {
      // write mix
      buffer_tapN_mix(&(dl->tapWr[0]), valWr[0], dl->preLevel);
      // FIXME
      //      buffer_tapN_mix(&(dl->tapWr[1]), valWr[1], dl->preLevel);
    }
  }

  // advance the read phasors
  if(dl->runRd) {
    buffer_tapN_next( &(dl->tapRd[0]) );
    buffer_tapN_next( &(dl->tapRd[1]) );
  }

  // advance the write phasors
  if(dl->runWr) {
    buffer_tapN_next( &(dl->tapWr[0]) );
    /// FIXME
    //    buffer_tapN_next( &(dl->tapWr[1]) );
  }
  
  return readVal;
}

// set loop endpoint in seconds
extern void delayFadeN_set_loop_sec(delayFadeN* dl, fix16 sec, u8 id) {
  u32 samps = sec_to_frames_trunc(sec);
  buffer_tapN_set_loop(&(dl->tapRd[id]), samps - 1);
  buffer_tapN_set_loop(&(dl->tapWr[id]), samps - 1);

}

// set loop endpoint in samples
extern void delayFadeN_set_loop_samp(delayFadeN* dl, u32 samps, u8 id) {
  dl->tapRd[id].loop = samps - 1;
  dl->tapWr[id].loop = samps - 1;

}

// set delayFadeN in seconds
extern void delayFadeN_set_delay_sec(delayFadeN* dl, fix16 sec, u8 id) {
  u32 samp = sec_to_frames_trunc(sec);
  // FIXME (why?)
  // -- something fucks up with i think delay > looptime... infinite wrap or something
  //  buffer_tapN_sync(&(dl->tapRd[id]), &(dl->tapWr[id]), samp);
  //// FIXME: only one write head for now
  buffer_tapN_sync(&(dl->tapRd[id]), &(dl->tapWr[0]), samp);
}

// set delayFadeN in samples
extern void delayFadeN_set_delay_samp(delayFadeN* dl, u32 samp, u8 id) {
  //  buffer_tapN_sync(&(dl->tapRd[id]), &(dl->tapWr[id]), samp);
  //// FIXME: only one write head for now
  buffer_tapN_sync(&(dl->tapRd[id]), &(dl->tapWr[0]), samp);
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
extern void delayFadeN_set_pos_read_sec(delayFadeN* dl, fix16 sec, u8 id) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tapN_set_pos(&(dl->tapRd[id]), samp);
}

extern void delayFadeN_set_pos_read_samp(delayFadeN* dl, u32 samp, u8 id) {
  buffer_tapN_set_pos(&(dl->tapRd[id]), samp);
}

// set write pos in seconds
extern void delayFadeN_set_pos_write_sec(delayFadeN* dl, fix16 sec, u8 id) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tapN_set_pos(&(dl->tapWr[id]), samp);
}

extern void delayFadeN_set_pos_write_samp(delayFadeN* dl, u32 samp, u8 id) {
  buffer_tapN_set_pos(&(dl->tapWr[id]), samp);
}

// set read run flag 
extern void delayFadeN_set_run_read(delayFadeN* dl, u8 val) {
  dl->runRd = val;
}

// set write run flag
extern void delayFadeN_set_run_write(delayFadeN* dl, u8 val) {
  dl->runWr = val;
}

// set read-head rate multiplier
void delayFadeN_set_mul(delayFadeN* dl, u32 val, u8 id) {
  // different terms, dumb...
  buffer_tapN_set_inc( &(dl->tapRd[id]), val );

}

// set read-head rate divider
void delayFadeN_set_div(delayFadeN* dl, u32 val, u8 id) {
  buffer_tapN_set_div( &(dl->tapRd[id]), val );
}
