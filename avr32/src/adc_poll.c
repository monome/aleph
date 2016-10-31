// libavr32
#include "adc.h"

#include "adc_poll.h"
#include "events.h"

// adc events
static const etype adctypes[4] = { 
  kEventAdc0 ,
  kEventAdc1 ,
  kEventAdc2 ,
  kEventAdc3 ,
};

// poll all ADC channels and store the data
void adc_poll() {
  
  static u16 adcVal[4] = {0, 0, 0, 0};
  static event_t e;
  u8 i;

  adc_convert(&adcVal);

  /// test with no filtering.. maybe this is fine
  for(i=0; i<4; i++) {        
    e.type = adctypes[i];
    e.data = (S16)(adcVal[i]);
    event_post(&e);
  }

}
