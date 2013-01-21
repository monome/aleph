/* filessystem.c
   avr32
   aleph

   lower level glue between filesystem (FAT) and hardware (PDCA)
*/

// asf
#include "compiler.h"
#include "pdca.h"
#include "print_funcs.h"
#include "spi.h"
#include "types.h"
// aleph
#include "fat_filelib.h"
#include "filesystem.h"


// PDCA Channel pointer
static volatile avr32_pdca_channel_t* pdca_channelrx ;
static volatile avr32_pdca_channel_t* pdca_channeltx ;

// Used to indicate the end of PDCA transfer
volatile u8 fsEndTransfer;

// sector data buffer
static volatile char ram_buffer[512];


///========================
//====  static low-level I/O

//static int media_init(const char* imgFileName) {
  /*
  imgFile = fopen(imgFileName, "r");  
  if (imgFile == 0) {
    printf("\r\n error opening image file.");
  }
  //  printf("\r\n image file pointer: %16x\r\n", (unsigned long int)imgFile);
  return 1;
  */
//  return 0;
//}

static int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count) {
  unsigned long i;

  for (i=0;i<sector_count;i++) {
      // ...
      // Add platform specific sector (512 bytes) read code here
      //..

    fsEndTransfer = false;

    spi_write(SD_MMC_SPI,0xFF); // Write a first dummy data to synchronise transfer
    pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
    pdca_channelrx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_RX); // get the correct PDCA channel pointer
    pdca_channeltx =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_TX); // get the correct PDCA channel pointer
    pdca_channelrx->cr = AVR32_PDCA_TEN_MASK; // Enable RX PDCA transfer first
    pdca_channeltx->cr = AVR32_PDCA_TEN_MASK; // and TX PDCA transfer
    // wait for signal from ISR
    while(!fsEndTransfer);
    // copy (FIXME ? )
    for(i=0; i<FS_BUF_SIZE; i++) {
      buffer[i] = pdcaRxBuf[i];
    }

    sector ++;
    buffer += FS_BUF_SIZE;
  }

  return 1;
}


static int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count)
{
    unsigned long i;

    for (i=0;i<sector_count;i++)
    {
        // ...
        // Add platform specific sector (512 bytes) write code here
        //..

        sector ++;
        buffer += 512;
    }

    return 1;
}


//=========
// extern

int fat_init(void) {
  int err;
  // Initialise File IO Library
  fl_init();
  
  // Attach media access functions to library
  err = ( fl_attach_media((fn_diskio_read)media_read, (fn_diskio_write)media_write) != FAT_INIT_OK             );

return err;
}

