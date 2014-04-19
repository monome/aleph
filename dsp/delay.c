/*
  delay.c
  aleph - audio

  implement delay line using buffer and tap objects

  single uninterpolated write head, single 24.8 interpolated read head

 */

#include "conversion.h"
#include "delay.h"


// initialize with pointer to audio buffer
void delay_init(delayLine* dl, fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));
  bufferTap24_8_init(&(dl->tapRd), &(dl->buffer));


  /*
  fix32 single_speed;
  single_speed.i = 1;
  single_speed.fr = 0;
  delay_set_rate(&(dl->tapRd), single_speed);
  delay_set_rate(&(dl->tapWr), single_speed);
  dl->preLevel = 0;
  dl->write = 1;
  */
}

fract32 delay_next(delayLine* dl, fract32 in) {
  //If you return in, we have a delay of zero length - works!
  //return in;

  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  readVal = bufferTap24_8_read( &(dl->tapRd) );


  // advance the write phasor
  //if(dl->runWr) {
    buffer_tapN_next( &(dl->tapWr) );
  //}

  // advance the read phasor
  //if(dl->runRd) {
    bufferTap24_8_next( &(dl->tapRd) );
  //}

//For now the write head always writes over any contents...

//so this is commented
/*
  // figure out what to write
  if(dl->preLevel == 0) {
    if(dl->write) {
      // write and replace
      buffer_tap_write(&(dl->tapWr), in);
    }
  } else if(dl->preLevel < 0) { // consider <0 to be == 1
    if(dl->write) {
      // overdub
      buffer_tap_add(&(dl->tapWr), in);
    }
  } else { // prelevel is non-zero, non-full
    if(dl->write) {
      // write mix
      buffer_tap_mix(&(dl->tapWr), in, dl->preLevel);
    }
  }
  */

  return readVal;
}

/*
// set loop endpoint in seconds
 void delay_set_loop_sec(delayLine* dl, fix16 sec) {
  u32 samps = sec_to_frames_trunc(sec);
  buffer_tap_set_loop(&(dl->tapRd), samps - 1);
  buffer_tap_set_loop(&(dl->tapWr), samps - 1);
}

// set loop endpoint in samples
 void delay_set_loop_samp(delayLine* dl, s32 samps) {
  dl->tapRd.loop = samps;
  dl->tapWr.loop = samps;
}
*/

/*
// set delay in seconds
 void delay_set_delay_sec(delayLine* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  fix32 time ;
  time.i = samp;
  time.fr = 0;
  buffer_tap_sync(&(dl->tapRd), &(dl->tapWr), time);
}

*/
void delay_set_delay_24_8(delayLine* dl, s32 subsamples) {
  //this sets a fractional delay in samples/256
  bufferTap24_8_syncN(&(dl->tapRd), &(dl->tapWr), subsamples);
}
// set delay in samples
 void delay_set_delay_samp(delayLine* dl, s32 samples) {
  s32 subsamples = samples * 256;
  bufferTap24_8_syncN(&(dl->tapRd), &(dl->tapWr), subsamples);
}
/*

// set erase level
 void delay_set_pre(delayLine* dl, fract32 pre) {
  dl->preLevel = pre;
}

// set write level
void delay_set_write(delayLine* dl, u8 write) {
  dl->write = write;
}

// set tap rate
void delay_set_rate(bufferTap* tap, fix32 rate) {
    tap->inc = rate;
}

// set read pos in seconds
 void delay_set_pos_read_sec(delayLine* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tap_set_pos(&(dl->tapRd), samp);
}
 void delay_set_pos_read_samp(delayLine* dl, u32 samples) {
  buffer_tap_set_pos(&(dl->tapRd), samples);
}
// set write pos in seconds
 void delay_set_pos_write_sec(delayLine* dl, fix16 sec) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer_tap_set_pos(&(dl->tapWr), samp);
}
*/

void delay_set_pos_write_samp(delayLine* dl, u32 samp) {
  buffer_tapN_set_pos(&(dl->tapWr), samp);
}
void delay_set_pos_read_samp(delayLine* dl, u32 samp) {
  bufferTap24_8_set_pos(&(dl->tapRd), samp*256);
}
/*
// set read run flag
 void delay_set_run_read(delayLine* dl, u8 val) {
  dl->runRd = val;
}

// set write run flag
 void delay_set_run_write(delayLine* dl, u8 val) {
  dl->runWr = val;
}*/
