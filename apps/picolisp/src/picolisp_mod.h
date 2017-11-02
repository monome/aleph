
/**
 * PicoLisp's platform agnostic modules.
 * 
 * All of them are declared here; Platform specific
 * modules are defined in platform/xxx/picolisp_platform.h
 */
#ifndef __PICOLISP_MOD_H__
#define __PICOLISP_MOD_H__

#include "pico.h"

// Helper macros.

#define PICOLISP_LIB_DEFINE(fun, mod)\
  {fun, #mod}

// platform data module.
#define PICOLISP_MOD_PD\
  PICOLISP_LIB_DEFINE(pd_platform, pd-platform),\
  PICOLISP_LIB_DEFINE(pd_cpu, pd-cpu),\
  PICOLISP_LIB_DEFINE(pd_board, pd-board),

// terminal module.
#define PICOLISP_MOD_TERM\
  PICOLISP_LIB_DEFINE(plisp_term_clrscr, term-clrscr),\
  PICOLISP_LIB_DEFINE(plisp_term_clreol, term-clreol),\
  PICOLISP_LIB_DEFINE(plisp_term_moveto, term-moveto),\
  PICOLISP_LIB_DEFINE(plisp_term_moveup, term-moveup),\
  PICOLISP_LIB_DEFINE(plisp_term_movedown, term-movedown),\
  PICOLISP_LIB_DEFINE(plisp_term_moveleft, term-moveleft),\
  PICOLISP_LIB_DEFINE(plisp_term_moveright, term-moveright),\
  PICOLISP_LIB_DEFINE(plisp_term_getlines, term-getlines),\
  PICOLISP_LIB_DEFINE(plisp_term_getcols, term-getcols),\
  PICOLISP_LIB_DEFINE(plisp_term_prinl, term-prinl),\
  PICOLISP_LIB_DEFINE(plisp_term_getcx, term-getcx),\
  PICOLISP_LIB_DEFINE(plisp_term_getcy, term-getcy),\
  PICOLISP_LIB_DEFINE(plisp_term_getchar, term-getchar),\
  PICOLISP_LIB_DEFINE(plisp_term_decode, term-decode),

// eLua module.
#define PICOLISP_MOD_ELUA\
  PICOLISP_LIB_DEFINE(plisp_elua_version, elua-version),\
  PICOLISP_LIB_DEFINE(plisp_elua_save_history, elua-save-history),\
  PICOLISP_LIB_DEFINE(plisp_elua_shell, elua-shell),

// cpu module.
#define PICOLISP_MOD_CPU\
  PICOLISP_LIB_DEFINE(plisp_cpu_w32, cpu-w32),\
  PICOLISP_LIB_DEFINE(plisp_cpu_r32, cpu-r32),\
  PICOLISP_LIB_DEFINE(plisp_cpu_w16, cpu-w16),\
  PICOLISP_LIB_DEFINE(plisp_cpu_r16, cpu-r16),\
  PICOLISP_LIB_DEFINE(plisp_cpu_w8, cpu-w8),\
  PICOLISP_LIB_DEFINE(plisp_cpu_r8, cpu-r8),\
  PICOLISP_LIB_DEFINE(plisp_cpu_clock, cpu-clock),

// timer module.
#define PICOLISP_MOD_TIMER\
  PICOLISP_LIB_DEFINE(tmr_delay, tmr-delay),\
  PICOLISP_LIB_DEFINE(tmr_read, tmr-read),\
  PICOLISP_LIB_DEFINE(tmr_start, tmr-start),\
  PICOLISP_LIB_DEFINE(tmr_gettimediff, tmr-gettimediff),\
  PICOLISP_LIB_DEFINE(tmr_getdiffnow, tmr-getdiffnow),\
  PICOLISP_LIB_DEFINE(tmr_getmindelay, tmr-getmindelay),\
  PICOLISP_LIB_DEFINE(tmr_getmaxdelay, tmr-getmaxdelay),\
  PICOLISP_LIB_DEFINE(tmr_setclock, tmr-setclock),\
  PICOLISP_LIB_DEFINE(tmr_getclock, tmr-getclock),\
  PICOLISP_LIB_DEFINE(tmr_decode, tmr-decode),

// i2c module.
#define PICOLISP_MOD_I2C\
  PICOLISP_LIB_DEFINE(plisp_i2c_setup, i2c-setup),\
  PICOLISP_LIB_DEFINE(plisp_i2c_start, i2c-start),\
  PICOLISP_LIB_DEFINE(plisp_i2c_stop, i2c-stop),\
  PICOLISP_LIB_DEFINE(plisp_i2c_address, i2c-address),\
  PICOLISP_LIB_DEFINE(plisp_i2c_write, i2c-write),\
  PICOLISP_LIB_DEFINE(plisp_i2c_read, i2c-read),

// pwm module.
#define PICOLISP_MOD_PWM\
  PICOLISP_LIB_DEFINE(plisp_pwm_setup, pwm-setup),\
  PICOLISP_LIB_DEFINE(plisp_pwm_start, pwm-start),\
  PICOLISP_LIB_DEFINE(plisp_pwm_stop, pwm-stop),\
  PICOLISP_LIB_DEFINE(plisp_pwm_setclock, pwm-setclock),\
  PICOLISP_LIB_DEFINE(plisp_pwm_getclock, pwm-getclock),

// spi module.
#define PICOLISP_MOD_SPI\
  PICOLISP_LIB_DEFINE(plisp_spi_sson, spi-sson),\
  PICOLISP_LIB_DEFINE(plisp_spi_ssoff, spi-ssoff),\
  PICOLISP_LIB_DEFINE(plisp_spi_setup, spi-setup),\
  PICOLISP_LIB_DEFINE(plisp_spi_write, spi-write),

// gpio module.
#define PICOLISP_MOD_PIO\
  PICOLISP_LIB_DEFINE(plisp_pio_pin_setdir, pio-pin-setdir),\
  PICOLISP_LIB_DEFINE(plisp_pio_pin_setpull, pio-pin-setpull),\
  PICOLISP_LIB_DEFINE(plisp_pio_pin_setval, pio-pin-setval),\
  PICOLISP_LIB_DEFINE(plisp_pio_pin_sethigh, pio-pin-sethigh),\
  PICOLISP_LIB_DEFINE(plisp_pio_pin_setlow, pio-pin-setlow),\
  PICOLISP_LIB_DEFINE(plisp_pio_pin_getval, pio-pin-getval),\
  PICOLISP_LIB_DEFINE(plisp_pio_port_setdir, pio-port-setdir),\
  PICOLISP_LIB_DEFINE(plisp_pio_port_setpull, pio-port-setpull),\
  PICOLISP_LIB_DEFINE(plisp_pio_port_setval, pio-port-setval),\
  PICOLISP_LIB_DEFINE(plisp_pio_port_sethigh, pio-port-sethigh),\
  PICOLISP_LIB_DEFINE(plisp_pio_port_setlow, pio-port-setlow),\
  PICOLISP_LIB_DEFINE(plisp_pio_port_getval, pio-port-getval),\
  PICOLISP_LIB_DEFINE(plisp_pio_decode, pio-decode),

// uart module.
#define PICOLISP_MOD_UART\
  PICOLISP_LIB_DEFINE(plisp_uart_setup, uart-setup),\
  PICOLISP_LIB_DEFINE(plisp_uart_write, uart-write),\
  PICOLISP_LIB_DEFINE(plisp_uart_set_flow_control, uart-set-flow-control),\
  PICOLISP_LIB_DEFINE(plisp_uart_set_buffer, uart-set-buffer),\
  PICOLISP_LIB_DEFINE(plisp_uart_getchar, uart-getchar),\
  PICOLISP_LIB_DEFINE(plisp_uart_vuart_tmr_ident, uart-vuart-tmr-ident),\
  PICOLISP_LIB_DEFINE(plisp_uart_read, uart-read),

// can module.
#define PICOLISP_MOD_CAN\
  PICOLISP_LIB_DEFINE(plisp_can_setup, can-setup),\
  PICOLISP_LIB_DEFINE(plisp_can_send, can-send),\
  PICOLISP_LIB_DEFINE(plisp_can_recv, can-recv),

// adc module
#define PICOLISP_MOD_ADC\
  PICOLISP_LIB_DEFINE(plisp_adc_maxval, adc-maxval),\
  PICOLISP_LIB_DEFINE(plisp_adc_setclock, adc-setclock),\
  PICOLISP_LIB_DEFINE(plisp_adc_isdone, adc-isdone),\
  PICOLISP_LIB_DEFINE(plisp_adc_setblocking, adc-setblocking),\
  PICOLISP_LIB_DEFINE(plisp_adc_setsmoothing, adc-setsmoothing),\
  PICOLISP_LIB_DEFINE(plisp_adc_sample, adc-sample),\
  PICOLISP_LIB_DEFINE(plisp_adc_getsample, adc-getsample),\
  PICOLISP_LIB_DEFINE(plisp_adc_getsamples, adc-getsamples),\
  PICOLISP_LIB_DEFINE(plisp_adc_insertsamples, adc-insertsamples),

// ks0108b glcd module.
#define PICOLISP_MOD_KS0108B\
  PICOLISP_LIB_DEFINE(plisp_ks0108b_init, glcd-init),\
  PICOLISP_LIB_DEFINE(plisp_ks0108b_setall, glcd-setall),\
  PICOLISP_LIB_DEFINE(plisp_ks0108b_clear, glcd-clear),\
  PICOLISP_LIB_DEFINE(plisp_ks0108b_on, glcd-on),\
  PICOLISP_LIB_DEFINE(plisp_ks0108b_off, glcd-off),\
  PICOLISP_LIB_DEFINE(plisp_ks0108b_gotoxy, glcd-gotoxy),\
  PICOLISP_LIB_DEFINE(plisp_ks0108b_prinl_small, glcd-prinl-small),\
  PICOLISP_LIB_DEFINE(plisp_ks0108b_prinl_big, glcd-prinl-big),  

#define MOD_CHECK_ID(pvar, mod, id)\
  if (!platform_ ## mod ## _exists(id))\
    err(pvar, NULL, #mod " %d does not exist", (unsigned int)id)

#define MOD_CHECK_RES_ID(pvar, mod, id, resmod, resid)\
  if (!platform_ ## mod ## _check_ ## resmod ## _id(id, resid))\
    err(pvar, NULL, #resmod" %d not valid with " #mod " %d",\
	(unsigned)resid, (unsigned)id)

#define MOD_CHECK_TIMER(pvar, id)\
  if (id == PLATFORM_TIMER_SYS_ID && !platform_timer_sys_available())\
    err(pvar, NULL, "the system timer is not available on this platform");\
  if (!platform_timer_exists(id))\
    err(pvar, NULL, "timer %d does not exist", (unsigned)id)\

#endif
