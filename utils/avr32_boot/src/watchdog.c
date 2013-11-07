#include "compiler.h"
#include "wdt.h"
#include "watchdog.h"

void watchdog_reset(void) {
  wdt_opt_t opt;
    opt.us_timeout_period = 1000000;
    wdt_enable(&opt); 
    while (1);

}
