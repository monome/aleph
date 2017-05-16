/*
  delayFade24_8.c
  aleph - audio

  implement delay line using buffer and tap objects

  this one uses no interpolation,
  and has two taps per read head to enable crossfading

 */

#include "conversion.h"
#include "delayFade24_8.h"
#include "pan.h"

// initialize with pointer to audio buffer
extern void delayFade24_8_init(delayFade24_8* dl, volatile fract16* data, u32 frames) {
  buffer16_init(&(dl->buffer), data, frames);

  buffer16Tap24_8_init(&(dl->tapRd[1]), &(dl->buffer));
  buffer16Tap24_8_init(&(dl->tapWr[1]), &(dl->buffer));

  buffer16Tap24_8_init(&(dl->tapRd[0]), &(dl->buffer));
  buffer16Tap24_8_init(&(dl->tapWr[0]), &(dl->buffer));

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
  //  dl->fadeRd = FR16_MAX;
  //  dl->fadeWr = FR16_MAX;
}

extern fract16 delayFade24_8_next(delayFade24_8* dl, fract16 in) {
  fract16 readVal;

  readVal = pan_lin_mix16( buffer16Tap24_8_read_bspline( &(dl->tapRd[0]) ) ,
			   buffer16Tap24_8_read_bspline( &(dl->tapRd[1]) ) ,
			   dl->fadeRd
			   );

  if(dl->write) {
      buffer16Tap24_8_mix(&(dl->tapWr[0]), in, dl->preLevel);
  }

  //DEBUG fix read/write head speeds
  /* dl->tapRd[0].inc = 256; */
  /* dl->tapRd[1].inc = 256; */
  /* dl->tapWr[0].inc = 128; */

  // advance the read phasors
  if(dl->runRd) {
    buffer16Tap24_8_next( &(dl->tapRd[0]) );
    buffer16Tap24_8_next( &(dl->tapRd[1]) );
  }

  // advance the write phasors
  if(dl->runWr) {
    buffer16Tap24_8_next( &(dl->tapWr[0]) );
  }

  return readVal;
}

// set loop endpoint in seconds
extern void delayFade24_8_set_loop_sec(delayFade24_8* dl, fix16 sec, u8 id) {
  u32 samps = sec_to_frames_trunc(sec);
  buffer16Tap24_8_set_loop(&(dl->tapRd[id]), (samps - 1));
  buffer16Tap24_8_set_loop(&(dl->tapWr[id]), (samps - 1));
}

// set loop endpoint in samples
extern void delayFade24_8_set_loop_samp(delayFade24_8* dl, u32 samps, u8 id) {
  dl->tapRd[id].loop = (samps - 1);
  dl->tapWr[id].loop = (samps - 1);
}

// set delayFade24_8 in seconds
extern void delayFade24_8_set_delay_sec(delayFade24_8* dl, fix16 sec, u8 id) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer16Tap24_8_sync(&(dl->tapRd[id]), &(dl->tapWr[0]), samp << 8);
}

// set delayFade24_8 in samples
extern void delayFade24_8_set_delay_samp(delayFade24_8* dl, u32 samp, u8 id) {
  buffer16Tap24_8_sync(&(dl->tapRd[id]), &(dl->tapWr[0]), samp << 8);
}

// set erase level
extern void delayFade24_8_set_pre(delayFade24_8* dl, fract16 pre) {
  dl->preLevel = pre;
}

// set write level
extern void delayFade24_8_set_write(delayFade24_8* dl, u8 write) {
  dl->write = write;
}

// set read head speed
extern void delayFade24_8_set_readSpeed(delayFade24_8* dl, fix16 rate) {
  dl->tapRd[0].inc = rate;
  dl->tapRd[1].inc = rate;
}

// set write head speed
extern void delayFade24_8_set_writeSpeed(delayFade24_8* dl, fix16 rate) {
  dl->tapWr[0].inc = rate;
}

// set read pos in seconds
extern void delayFade24_8_set_pos_read_sec(delayFade24_8* dl, fix16 sec, u8 id) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer16Tap24_8_set_pos(&(dl->tapRd[id]), samp << 8);
}

extern void delayFade24_8_set_pos_read_samp(delayFade24_8* dl, u32 samp, u8 id) {
  buffer16Tap24_8_set_pos(&(dl->tapRd[id]), samp << 8);
}

// set write pos in seconds
extern void delayFade24_8_set_pos_write_sec(delayFade24_8* dl, fix16 sec, u8 id) {
  u32 samp = sec_to_frames_trunc(sec);
  buffer16Tap24_8_set_pos(&(dl->tapWr[id]), samp << 8);
}

extern void delayFade24_8_set_pos_write_samp(delayFade24_8* dl, u32 samp, u8 id) {
  buffer16Tap24_8_set_pos(&(dl->tapWr[id]), samp << 8);
}

// set read run flag
extern void delayFade24_8_set_run_read(delayFade24_8* dl, u8 val) {
  dl->runRd = val;
}

// set write run flag
extern void delayFade24_8_set_run_write(delayFade24_8* dl, u8 val) {
  dl->runWr = val;
}
