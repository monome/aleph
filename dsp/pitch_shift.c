/*
  pitch_shift.c
  aleph - audio

  implement pitch shift using buffer, tap and echoTap data types

  single uninterpolated write head, dual 24.8 interpolated echoTap read heads

 */

#include "conversion.h"
#include "pitch_shift.h"


// initialize with pointer to audio buffer
void delay_init(delayLine* dl, fract32* data, u32 frames) {
  buffer_init(&(dl->buffer), data, frames);
  buffer_tapN_init(&(dl->tapWr), &(dl->buffer));

  echoTap24_8_init(&(dl->tapRd0), &(dl->tapWr));
  dl->tapRd0.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd0.shape = SHAPE_FATLUMP;
  dl->tapRd0.echoMin = 0;
  dl->tapRd0.echoMax = 256 * 1020;
  dl->tapRd0.echoTime = 0;

  echoTap24_8_init(&(dl->tapRd1), &(dl->tapWr));
  dl->tapRd1.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd1.shape = SHAPE_FATLUMP;
  dl->tapRd1.echoMin = 0;
  dl->tapRd1.echoMax = 256 * 1012;
  dl->tapRd1.echoTime= 256 * 2500;

  echoTap24_8_init(&(dl->tapRd2), &(dl->tapWr));
  dl->tapRd2.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd2.shape = SHAPE_FATLUMP;
  dl->tapRd2.echoMin = 0;
  dl->tapRd2.echoMax = 256 * 954;
  dl->tapRd2.echoTime= 256 * 2500;

  echoTap24_8_init(&(dl->tapRd3), &(dl->tapWr));
  dl->tapRd3.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd3.shape = SHAPE_FATLUMP;
  dl->tapRd3.echoMin = 0;
  dl->tapRd3.echoMax = 256 * 800;
  dl->tapRd3.echoTime= 256 * 2500;

  echoTap24_8_init(&(dl->tapRd4), &(dl->tapWr));
  dl->tapRd4.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd4.shape = SHAPE_FATLUMP;
  dl->tapRd4.echoMin = 0;
  dl->tapRd4.echoMax = 256 * 700;
  dl->tapRd4.echoTime= 256 * 2500;

  echoTap24_8_init(&(dl->tapRd5), &(dl->tapWr));
  dl->tapRd5.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd5.shape = SHAPE_FATLUMP;
  dl->tapRd5.echoMin = 0;
  dl->tapRd5.echoMax = 256 * 500;
  dl->tapRd5.echoTime= 256 * 2500;

  echoTap24_8_init(&(dl->tapRd6), &(dl->tapWr));
  dl->tapRd6.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd6.shape = SHAPE_FATLUMP;
  dl->tapRd6.echoMin = 0;
  dl->tapRd6.echoMax = 256 * 794;
  dl->tapRd6.echoTime= 256 * 2500;

  echoTap24_8_init(&(dl->tapRd7), &(dl->tapWr));
  dl->tapRd7.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd7.shape = SHAPE_FATLUMP;
  dl->tapRd7.echoMin = 0;
  dl->tapRd7.echoMax = 256 * 1001;
  dl->tapRd7.echoTime= 256 * 2500;

  echoTap24_8_init(&(dl->tapRd8), &(dl->tapWr));
  dl->tapRd8.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd8.shape = SHAPE_FATLUMP;
  dl->tapRd8.echoMin = 0;
  dl->tapRd8.echoMax = 256 * 960;
  dl->tapRd8.echoTime= 256 * 2500;

  echoTap24_8_init(&(dl->tapRd9), &(dl->tapWr));
  dl->tapRd9.edge_behaviour = EDGE_BOUNCE;
  dl->tapRd9.shape = SHAPE_FATLUMP;
  dl->tapRd9.echoMin = 0;
  dl->tapRd9.echoMax = 256 * 1124;
  dl->tapRd9.echoTime= 256 * 2500;

}

fract32 delay_next(delayLine* dl, fract32 in) {
  //DEBUG uncomment this line to check plumbing this far...
  //return in;

  buffer_tapN_write(&(dl->tapWr), in);

  fract32 readVal;
  fract32 mix_factor = FR32_MAX / 3;
  readVal = mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd0) ),mix_factor);
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd1) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd2) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd3) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd4) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd5) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd6) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd7) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd8) ),mix_factor));
  readVal = add_fr1x32(readVal, mult_fr1x32x32(echoTap24_8_read( &(dl->tapRd9) ),mix_factor));

  buffer_tapN_next( &(dl->tapWr) );

  //FIXME at the moment all the pitch shift logic happens in the echoTap functions.
  //That's no good! Should keep echoTap general to pitch shifter, sampler, chorus
  //(and any other short-delay type modules we can dream up)

  //FIXME so the pitch shift-specific logic should go here
  // basically when one read head passes the halfway point
  // kick the other one
  echoTap24_8_next( &(dl->tapRd0) );
  echoTap24_8_next( &(dl->tapRd1) );
  echoTap24_8_next( &(dl->tapRd2) );
  echoTap24_8_next( &(dl->tapRd3) );
  echoTap24_8_next( &(dl->tapRd4) );
  echoTap24_8_next( &(dl->tapRd5) );
  echoTap24_8_next( &(dl->tapRd6) );
  echoTap24_8_next( &(dl->tapRd7) );
  echoTap24_8_next( &(dl->tapRd8) );
  echoTap24_8_next( &(dl->tapRd9) );

  return readVal;
}

void delay_set_rate(delayLine* dl, s32 subsamples) {
  dl->tapRd0.playback_speed = subsamples + 3;
  dl->tapRd1.playback_speed = subsamples + 2;
  dl->tapRd2.playback_speed = subsamples + 4;
  dl->tapRd3.playback_speed = subsamples + 1;
  dl->tapRd4.playback_speed = subsamples + 0;
  dl->tapRd5.playback_speed = subsamples - 2;
  dl->tapRd6.playback_speed = subsamples - 4;
  dl->tapRd7.playback_speed = subsamples - 3;
  dl->tapRd8.playback_speed = subsamples - 2;
  dl->tapRd9.playback_speed = subsamples + 3;
}
void delay_set_pos_write_samp(delayLine* dl, u32 samp) {
  buffer_tapN_set_pos(&(dl->tapWr), samp);
}
