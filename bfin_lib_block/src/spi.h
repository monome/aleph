/**
 * spi.h
 *
 * handle data from SPI peripheral
 */

#ifndef _ALEPH_BFIN_SPI_H_
#define _ALEPH_BFIN_SPI_H_

#include "types.h"

//! handle a received byte, return next byte for transmission
extern u8 spi_handle_byte(u8 rx);

#endif
