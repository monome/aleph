#include <stdio.h>

#include "print_funcs.h"
 
void print_dbg(const char* str) {
  printf("%s",str);
}

void print_dbg_char(int c) { ;; }

void print_dbg_ulong(unsigned long n) { 
  printf("%d", n);
}

void print_dbg_char_hex(unsigned char n) { ;; }


void print_dbg_short_hex(unsigned short n) { ;; }

void print_dbg_hex(unsigned long n) { 
  printf("%08x", n);
 }

/* void print(volatile avr32_usart_t *usart, const char *str) { ;; } */

/* void print_char(volatile avr32_usart_t *usart, int c) { ;; } */

/* void print_ulong(volatile avr32_usart_t *usart, unsigned long n) { ;; } */

/* void print_char_hex(volatile avr32_usart_t *usart, unsigned char n) { ;; } */

/* void print_short_hex(volatile avr32_usart_t *usart, unsigned short n) { ;; } */

/* void print_hex(volatile avr32_usart_t *usart, unsigned long n) { ;; } */

void print_byte_array (u8* data, u32 size, u32 linebreak) { ;; }
