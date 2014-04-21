/*
  delay.c
  aleph - audio

  implement delay line using buffer and tap objects

  single uninterpolated write head, single 24.8 interpolated read head

 */

#include "conversion.h"
#include "pitch_shift.h"


// initialize with pointer to audio buffer
void delay_init(delayLine* dl, fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));
  echoTap24_8_init(&(dl->tapRd), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd1), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd2), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd3), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd4), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd5), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd6), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd7), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd8), &(dl->tapWr));
  echoTap24_8_init(&(dl->tapRd9), &(dl->tapWr));

  echoTap24_8_set_pos(&(dl->tapRd),256 * 990);
  echoTap24_8_set_pos(&(dl->tapRd1),256 * 10);
  echoTap24_8_set_pos(&(dl->tapRd2),256 * 80);
  echoTap24_8_set_pos(&(dl->tapRd3),256 * 500);
  echoTap24_8_set_pos(&(dl->tapRd4),256 * 800);
  echoTap24_8_set_pos(&(dl->tapRd5),256 * 780);
  echoTap24_8_set_pos(&(dl->tapRd6),256 * 590);
  echoTap24_8_set_pos(&(dl->tapRd7),256 * 280);
  echoTap24_8_set_pos(&(dl->tapRd8),256 * 400);
  echoTap24_8_set_pos(&(dl->tapRd9),256 * 840);

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
  //readVal = echoTap24_8_read( &(dl->tapRd) );
  fract32 mix_factor = FR32_MAX;
  readVal = mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd) ),mix_factor);
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd1) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd2) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd3) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd4) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd5) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd6) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd7) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd8) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd9) ),mix_factor));


  // advance the write phasor
  //if(dl->runWr) {
    buffer_tapN_next( &(dl->tapWr) );
  //}

  // advance the read phasor
  //if(dl->runRd) {
    echoTap24_8_next( &(dl->tapRd) );
    echoTap24_8_next( &(dl->tapRd1) );
    echoTap24_8_next( &(dl->tapRd2) );
    echoTap24_8_next( &(dl->tapRd3) );
    echoTap24_8_next( &(dl->tapRd4) );
    echoTap24_8_next( &(dl->tapRd5) );
    echoTap24_8_next( &(dl->tapRd6) );
    echoTap24_8_next( &(dl->tapRd7) );
    echoTap24_8_next( &(dl->tapRd8) );
    echoTap24_8_next( &(dl->tapRd9) );
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
void delay_set_rate(delayLine* dl, s32 subsamples) {
  echoTap24_8_set_rate(&(dl->tapRd), subsamples);
}
void delay_set_delay_24_8(delayLine* dl, s32 subsamples) {
  echoTap24_8_set_pos(&(dl->tapRd),subsamples);
}
// set delay in samples
 void delay_set_delay_samp(delayLine* dl, s32 samples) {
  echoTap24_8_set_pos(&(dl->tapRd),samples * 256);
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
/*
// set read run flag
 void delay_set_run_read(delayLine* dl, u8 val) {
  dl->runRd = val;
}

// set write run flag
 void delay_set_run_write(delayLine* dl, u8 val) {
  dl->runWr = val;
}*/
