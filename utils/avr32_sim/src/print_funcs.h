#ifndef _ALEPH_AVR32_SIM_PRINT_DBG_H_
#define _ALEPH_AVR32_SIM_PRINT_DBG_H_

#include "types.h"

/*! \brief Prints a string of characters to DBG_USART.
 *
 * \param str The string of characters to print.
 */
extern void print_dbg(const char *str);

/*! \brief Prints a character to DBG_USART.
 *
 * \param c The character to print.
 */
extern void print_dbg_char(int c);

/*! \brief Prints an integer to DBG_USART in a decimal representation.
 *
 * \param n The integer to print.
 */
extern void print_dbg_ulong(unsigned long n);

/*! \brief Prints a char to DBG_USART in an hexadecimal representation.
 *
 * \param n The char to print.
 */
extern void print_dbg_char_hex(unsigned char n);

/*! \brief Prints a short integer to DBG_USART in an hexadecimal representation.
 *
 * \param n The short integer to print.
 */
extern void print_dbg_short_hex(unsigned short n);

/*! \brief Prints an integer to DBG_USART in an hexadecimal representation.
 *
 * \param n The integer to print.
 */
extern void print_dbg_hex(unsigned long n);

/*! \brief Prints a string of characters to a given USART.
 *
 * \param usart Base address of the USART instance to print to.
 * \param str The string of characters to print.
 */
/* extern void print(volatile avr32_usart_t *usart, const char *str); */

/* /\*! \brief Prints a character to a given USART. */
/*  * */
/*  * \param usart Base address of the USART instance to print to. */
/*  * \param c The character to print. */
/*  *\/ */
/* extern void print_char(volatile avr32_usart_t *usart, int c); */

/* /\*! \brief Prints an integer to a given USART in a decimal representation. */
/*  * */
/*  * \param usart Base address of the USART instance to print to. */
/*  * \param n The integer to print. */
/*  *\/ */
/* extern void print_ulong(volatile avr32_usart_t *usart, unsigned long n); */

/* /\*! \brief Prints a char to a given USART in an hexadecimal representation. */
/*  * */
/*  * \param usart Base address of the USART instance to print to. */
/*  * \param n The char to print. */
/*  *\/ */
/* extern void print_char_hex(volatile avr32_usart_t *usart, unsigned char n); */

/* /\*! \brief Prints a short integer to a given USART in an hexadecimal */
/*  *         representation. */
/*  * */
/*  * \param usart Base address of the USART instance to print to. */
/*  * \param n The short integer to print. */
/*  *\/ */
/* extern void print_short_hex(volatile avr32_usart_t *usart, unsigned short n); */

/* /\*! \brief Prints an integer to a given USART in an hexadecimal representation. */
/*  * */
/*  * \param usart Base address of the USART instance to print to. */
/*  * \param n The integer to print. */
/*  *\/ */
/* extern void print_hex(volatile avr32_usart_t *usart, unsigned long n); */

// print an array of bytes
extern void print_byte_array (u8* data, u32 size, u32 linebreak);

// print a unicode string (stripping to ascii)
void print_unicode_string(char* str, u32 len);


#endif
