/* filessystem.c
   avr32
   aleph

   lower level glue between filesystem (FAT) and hardware (PDCA)
*/


// stdlib
#include <string.h>
// asf
#include "compiler.h"
#include "delay.h"
#include "pdca.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"
#include "spi.h"
#include "types.h"
// aleph
#include "app.h"
#include "filesystem.h"


//----- extern 
// Local RAM buffer to store data  to/from the SD/MMC card
volatile U8 pdcaRxBuf[FS_BUF_SIZE];
volatile U8 pdcaTxBuf[FS_BUF_SIZE];

// Used to indicate the end of PDCA transfer
volatile u8 fsEndTransfer;

//---- static
// PDCA Channel pointer
volatile avr32_pdca_channel_t* pdcaRxChan ;
volatile avr32_pdca_channel_t* pdcaTxChan ;

///========================
//====  functions

//----------------------------------
//----- static
static u8 file_read_write_test(void);

//---------------------------
//--- extern

//---- low level i/o
int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count);
int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count) {
  unsigned long i;

  for (i=0;i<sector_count;i++) {
    pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_RX,
		       &pdcaRxBuf,
		       FS_BUF_SIZE);
    
    pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_TX,
		       (void *)&pdcaTxBuf,
		       FS_BUF_SIZE); //send dummy to activate the clock
    
    fsEndTransfer = false;
    
    if(sd_mmc_spi_read_open_PDCA (sector)) {

      spi_write(SD_MMC_SPI,0xFF); // dummy byte synchronizes transfer

      pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
      pdcaRxChan =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_RX);
      pdcaTxChan =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_TX);
      pdcaRxChan->cr = AVR32_PDCA_TEN_MASK; // Enable RX PDCA transfer first
      pdcaTxChan->cr = AVR32_PDCA_TEN_MASK; // and TX PDCA transfer
      // wait for signal from ISR
      while(!fsEndTransfer) { ;; }
      // copy FIXME: could optimize away
      for(i=0; i<FS_BUF_SIZE; i++) {
	buffer[i] = pdcaRxBuf[i];
      }
    } else {
      print_dbg("\r\n error opening PDCA at sector "); 
      print_dbg_ulong(sector);
    }
    sector ++;
    buffer += FS_BUF_SIZE;
  }
  return 1;
}


int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count);
int media_write(unsigned long sector, unsigned char *buffer, unsigned long sector_count) {
  unsigned long i;
  bool status;

  // PDCA write isn't implemented in ASF... 
  // for the moment use slower blocking write.

  status = sd_mmc_spi_write_open(sector);

  if(status == false) {
    print_dbg("\r\n error opening sd_mmc_spi, sector: ");
    print_dbg_hex(sector);
    return 0;
  }

  for (i=0;i<sector_count;i++) {
    status = sd_mmc_spi_write_sector_from_ram(buffer);
    /////////// dbg
    if(status == false) {
      print_dbg("\r\n error writing sd_mmc_spi, sector: ");
      print_dbg_hex(sector);
    }
    //////////////
    sector++;
    buffer += 512;
  }
  sd_mmc_spi_write_close();
  return 1;
}


//=========
// extern

int fat_init(void) {
  u8 ret = 0;
  // Initialise File IO Library
  print_dbg("\r\n beginning FAT library init.");
  fl_init();
  print_dbg("\r\n finished FAT library init.");

  // Attach media access functions to library
  if ( fl_attach_media((fn_diskio_read)media_read, (fn_diskio_write)media_write) != FAT_INIT_OK ) {
    print_dbg("\r\n failed to attach media access functions to fat_io_lib \r\n");
    //    return 1;
    ret = 1;
  } else {
    print_dbg("\r\n attached media access functions to fat_io_lib");
    //    return 0;
    ret = 0;
  }

  ret |= file_read_write_test();
  return ret;

}



/// sdcard test
static const char test_string_write[] = "ALEPH_TEST";
#define TESTN 10
static char test_string_read[TESTN];

static u8 file_read_write_test(void) {
  void* fp;
  u8 ret;
  u8 i;

  app_pause();

  //  delay_ms(10);

  fp = fl_fopen("/aleph_test.txt", "w");
  print_dbg("\r\n test sdcard write, fp: ");
  print_dbg_hex((u32)fp);
  fl_fputs(test_string_write, fp);
  fl_fclose(fp);

  print_dbg("\r\n waiting after write...");
  delay_ms(500);

  fp = fl_fopen("/aleph_test.txt", "r");
  print_dbg("\r\n test sdcard read, fp: ");
  print_dbg_hex((u32)fp);
  for(i=0; i<TESTN; i++) {
    test_string_read[i] = fl_fgetc(fp);
  }
  //  fl_fread(test_string_read, 1, 11, fp);
  print_dbg(" , read: ");
  print_dbg(test_string_read);

  if(strncmp(test_string_write, test_string_read, TESTN)) {
    ret = 1;
    print_dbg("\r\n ERROR: sdcard write / read mismatch");
  } else {
    ret = 0;
    print_dbg("\r\n OK: sdcard write / read match");
  }

  fl_fclose(fp);

  app_resume();

  print_dbg("\r\n finished sdcard write/read test.");
  return ret;
}
