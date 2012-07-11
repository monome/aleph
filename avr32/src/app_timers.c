// ASF
#include "compiler.h"
// aleph
#include "events.h"
#include "eventTypes.h"
#include "timers.h"
#include "app_timers,h"

// refresh the screen periodically
static apptimer_t screen_timer;
// poll encoder accumulators periodically
static apptimer_t encoder_timer;
// debounce switches
static apptimer_t switch_timer;

static void screen_timer_callback(void) {
  
}

static void encoder_timer_callback(void) {
}

static void switch_timer_callback(void) {
}

void init_app_timers(void) {
 
  SetTimer( &adcUI_Timer, TMRTAG_START_ADC_UI, TMR_ADC_UI_INTERVAL, ADC_UI_TimerCallback, TRUE ); 
}
