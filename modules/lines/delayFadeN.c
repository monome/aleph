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
  
  buffer_tapN_init(&(dl->tapRd[0]), &(dl->buffer));
  buffer_tapN_init(&(dl->tapRd[1]), &(dl->buffer));

  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));

  dl->tapWr.idx = 0;
  dl->tapRd[0].idx = 0;
  dl->tapRd[1].idx = 0;

  dl->tapWr.loop = frames;
  dl->tapRd[0].loop = frames;
  dl->tapRd[1].loop = frames;

  dl->preLevel = 0;
  dl->write = 1;

  dl->fadeRd = 0;
  dl->fadeWr = 0;
}
static inline s32 equalPower_xfade (fract32 x, fract32 y, fract32 pos) {
  fract32 pos_2 = mult_fr1x32x32(pos, pos);
  fract32 amp_x = sub_fr1x32(FR32_MAX,  pos_2);
  fract32 pos_bar = sub_fr1x32(FR32_MAX,  pos);
  fract32 amp_y = sub_fr1x32(FR32_MAX,  mult_fr1x32x32(pos_bar, pos_bar));
  return add_fr1x32(mult_fr1x32x32(amp_x, x),
		    mult_fr1x32x32(amp_y, y));
}

extern fract32 delayFadeN_next(delayFadeN* dl, fract32 in) {
  fract32 readVal;

  /// equal-power read-head crossfade
  /* readVal = equalPower_xfade( buffer_tapN_read( &(dl->tapRd[0]) ) , */
  /* 			      buffer_tapN_read( &(dl->tapRd[1]) ) , */
  /* 			      dl->fadeRd); */

  /// linear read-head crossfade
  readVal = pan_lin_mix( buffer_tapN_read( &(dl->tapRd[0]) ) ,
			 buffer_tapN_read( &(dl->tapRd[1]) ) ,
			 dl->fadeRd);

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

  // advance the read phasors
  if(dl->runRd) {
    buffer_tapN_next( &(dl->tapRd[0]) );
    buffer_tapN_next( &(dl->tapRd[1]) );
  }
  // advance the write phasors
  if(dl->runWr) {
    buffer_tapN_next( &(dl->tapWr) );
  }
  return readVal;
}

// set loop endpoint in seconds
extern void delayFadeN_set_loop_ms(delayFadeN* dl, fract32 ms) {
  u32 samps = ms * 48;
  dl->tapRd[0].loop = samps;
  dl->tapRd[1].loop = samps;
  dl->tapWr.loop = samps;
}

extern void delayFadeN_set_delay_ms(delayFadeN* dl, fract32 ms, u8 id) {
  u32 samp = ms * 48;
  buffer_tapN_sync(&(dl->tapRd[id]), &(dl->tapWr), samp);
}

// set delayFadeN in samples
extern void delayFadeN_set_delay_samp(delayFadeN* dl, u32 samp, u8 id) {
  buffer_tapN_sync(&(dl->tapRd[id]), &(dl->tapWr), samp);
}

// set erase level
extern void delayFadeN_set_pre(delayFadeN* dl, fract32 pre) {
  dl->preLevel = pre;
}

// set write level
extern void delayFadeN_set_write(delayFadeN* dl, u8 write) {
  dl->write = write;
}

extern void delayFadeN_set_pos_read_ms(delayFadeN* dl, fract32 ms, u8 id) {
  u32 samp = ms * 48;
  buffer_tapN_set_pos(&(dl->tapRd[id]), samp);
}

extern void delayFadeN_set_pos_write_ms(delayFadeN* dl, fract32 ms) {
  u32 samp = 48 * ms;
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

// set read-head rate multiplier
void delayFadeN_set_mul(delayFadeN* dl, u32 val) {
  // different terms, dumb...
  buffer_tapN_set_inc( &(dl->tapRd[0]), val );
  buffer_tapN_set_inc( &(dl->tapRd[1]), val );

}

// set read-head rate divider
void delayFadeN_set_div(delayFadeN* dl, u32 val) {
  buffer_tapN_set_div( &(dl->tapRd[0]), val );
  buffer_tapN_set_div( &(dl->tapRd[1]), val );
}
