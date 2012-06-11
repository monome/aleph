/* encoders.c
 * aleph
 */

// asf
#include "compiler.h"
#include "gpio.h"
// aleph
#include "config.h"
#include "events.h"
#include "eventTypes.h"
#include "encoders.h"

//--------------------------
//---- external variables
const U16 encPins[NUM_ENC_x2] = {
  ENC0_S0_PIN, ENC0_S1_PIN,
  ENC1_S0_PIN, ENC1_S1_PIN,
  ENC2_S0_PIN, ENC2_S1_PIN,
  ENC3_S0_PIN, ENC3_S1_PIN,
};

//---------------------------
//---- static variables
static const eEventType encEvents[NUM_ENC_x2] = {
  kEventEncoder0,
  kEventEncoder1,
  kEventEncoder2,
  kEventEncoder3,
};

// encoder movement map
static const S8 enc_map[4][4] = { {0,1,-1,0}, {-1,0,0,1}, {1,0,0,-1}, {0,-1,1,0} };

// current encoder positions
static U8 enc_pos[NUM_ENC];


//------------------------------
//----- external functions

// initialize encoder positions
void init_encoders(void) {
  U8 i;
  for(i=0; i<NUM_ENC; i++) {
    enc_pos[i] = gpio_get_pin_value(encPins[i*2]) + (gpio_get_pin_value(encPins[i*2 + 1]) << 1);
  }
}

volatile U8 pos;

// post events based on encoder movements
void handle_enc( const U8 idx, const U8 p0, const U8 p1 ) {
  event_t e;
  
  S8 val = 0;
  pos = gpio_get_pin_value(p0) + (gpio_get_pin_value(p1) << 1);
  if (pos != enc_pos[idx]) {
    val = enc_map[enc_pos[idx]][pos];
    enc_pos[idx] = pos;
  }
  if (val != 0) {
    e.eventNum = encEvents[idx];
    e.eventData = val;
    post_event(&e);
  }
}


