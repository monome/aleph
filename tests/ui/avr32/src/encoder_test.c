#include <avr32/io.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "intc.h"

/////////// defines
#define ENC0_S0_PIN AVR32_PIN_PA06
#define ENC0_S1_PIN AVR32_PIN_PA07 

//////////// typedefs

// encoder struct
typedef struct {
  unsigned char pos_now, pos_old;
  int step;
} enc_t;

//////////// static vars
// for encoder position tracking
static const int enc_map[4][4] = { {0,1,-1,0}, {-1,0,0,1}, {1,0,0,-1}, {0,-1,1,0} };
static enc_t enc[1];
// test: value
static S8 encVal;
// test: LED map
static const U8 ledPins[] = {
  LED0_GPIO,
  LED1_GPIO, 
  LED2_GPIO, 
  LED3_GPIO, 
  LED4_GPIO, 
  LED5_GPIO,
  LED6_GPIO
 }; 

//////////// function declarations
static void register_interrupts( void );
static void init_control( void );
static void process_enc(U8 p1, U8 p2);

//////////// interrupts
__attribute__((__interrupt__))
static void int_handler_enc( void ) {
  //  gpio_tgl_gpio_pin(LED0_GPIO);

  if (gpio_get_pin_interrupt_flag(ENC0_S0_PIN)) {
    process_enc(ENC0_S0_PIN, ENC0_S1_PIN);
    gpio_clear_pin_interrupt_flag(ENC0_S0_PIN);
  } 

  if (gpio_get_pin_interrupt_flag(ENC0_S1_PIN)) {
    process_enc(ENC0_S0_PIN, ENC0_S1_PIN);
    gpio_clear_pin_interrupt_flag(ENC0_S1_PIN);
  }
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int main(void) {
  pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);

  gpio_enable_pin_pull_up(ENC0_S0_PIN);
  gpio_enable_pin_pull_up(ENC0_S1_PIN);
  init_control();

  Disable_global_interrupt();
  register_interrupts();
  Enable_global_interrupt();

  while(1) {
    ;;
  }
}



//////////// function definitions

// init control values
void init_control( void ) {
  enc[0].pos_old = gpio_get_pin_value(ENC0_S0_PIN) + (gpio_get_pin_value(ENC0_S1_PIN) << 1);
}


// encoder interrupt
void process_enc( U8 p1, U8 p2 )
{
  U8 e, i;

  e = 0;

  enc[e].pos_now = gpio_get_pin_value(p1) + (gpio_get_pin_value(p2) << 1);
  if(enc[e].pos_now != enc[e].pos_old) {
    encVal += enc_map[enc[e].pos_old][enc[e].pos_now];
    if (encVal > 6) encVal = 0;
    if (encVal < 0) encVal = 6;

    for (i=0; i<7; i++) { 
      if(encVal == i) {
	gpio_clr_gpio_pin(ledPins[i]);
      } else {
	gpio_set_gpio_pin(ledPins[i]);
      }
    }
    enc[e].pos_old = enc[e].pos_now;
  }
}

// register interrupts for needed pins
static void register_interrupts( void )
{
  gpio_enable_pin_interrupt( ENC0_S0_PIN,	GPIO_PIN_CHANGE);
  gpio_enable_pin_interrupt( ENC0_S1_PIN,	GPIO_PIN_CHANGE);

  INTC_register_interrupt( &int_handler_enc, AVR32_GPIO_IRQ_0 + (ENC0_S0_PIN/8), AVR32_INTC_INT1 );

}
