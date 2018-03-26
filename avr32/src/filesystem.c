/* filessystem.c
   avr32
   aleph

   lower level glue between filesystem (FAT) and hardware (PDCA)
*/

// asf
#include "compiler.h"
#include "pdca.h"
#include "print_funcs.h"
#include "sd_mmc_spi.h"
#include "spi.h"
#include "types.h"
// aleph
#include "filesystem.h"

//----- extern
// Local RAM buffer to store data  to/from the SD/MMC card
volatile U8 pdcaRxBuf[FS_BUF_SIZE];
volatile U8 pdcaTxBuf[FS_BUF_SIZE];

// Used to indicate the end of PDCA transfer
//volatile u8 fsEndTransfer;

//---- static
// PDCA Channel pointer
volatile avr32_pdca_channel_t *pdcaRxChan;
volatile avr32_pdca_channel_t *pdcaTxChan;

///========================
//====  functions

//---- low level i/o
int media_read(unsigned long sector, unsigned char *buffer,
               unsigned long sector_count);
int media_read(unsigned long sector, unsigned char *buffer, unsigned long sector_count) {
    unsigned long i;
    
    for (i=0;i<sector_count;i++) {
        pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_RX,
                          &pdcaRxBuf,
                          FS_BUF_SIZE);
        
        pdca_load_channel( AVR32_PDCA_CHANNEL_SPI_TX,
                          (void *)&pdcaTxBuf,
                          FS_BUF_SIZE); //send dummy to activate the clock
        
        if(sd_mmc_spi_read_open_PDCA (sector)) {
            
            spi_write(SD_MMC_SPI,0xFF); // dummy byte synchronizes transfer
            
            pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
            pdcaRxChan =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_RX);
            pdcaTxChan =(volatile avr32_pdca_channel_t*) pdca_get_handler(AVR32_PDCA_CHANNEL_SPI_TX);
            pdcaRxChan->cr = AVR32_PDCA_TEN_MASK; // Enable RX PDCA transfer first
            pdcaTxChan->cr = AVR32_PDCA_TEN_MASK; // and TX PDCA transfer
            // wait for signal from ISR
            while (!(pdcaRxChan->isr & AVR32_PDCA_ISR_TRC_MASK));
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

int media_write(unsigned long sector, unsigned char *buffer,
                unsigned long sector_count);
int media_write(unsigned long sector, unsigned char *buffer,
                unsigned long sector_count) {
    unsigned long i;
    bool status;

    // PDCA write isn't implemented in ASF... ! dang
    // for the moment use slower blocking write.

    status = sd_mmc_spi_write_open(sector);

    if (status == false) {
        print_dbg("\r\n error opening sd_mmc_spi, sector: ");
        print_dbg_hex(sector);
        return 0;
    }

    for (i = 0; i < sector_count; i++) {
        status = sd_mmc_spi_write_sector_from_ram(buffer);
        /////////// dbg
        if (status == false) {
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
    // Initialise File IO Library
    print_dbg("\r\n beginning FAT library init.");
    fl_init();
    print_dbg("\r\n finished FAT library init.");
    // Attach media access functions to library
    if (fl_attach_media((fn_diskio_read)media_read,
                        (fn_diskio_write)media_write) != FAT_INIT_OK) {
        print_dbg(
            "\r\n failed to attach media access functions to fat_io_lib \r\n");
        return 1;
    } else {
        print_dbg("\r\n attached media access functions to fat_io_lib");
        return 0;
    }
}

int bfin_sample_transfer(unsigned long sector, unsigned long bytes) {
    unsigned long i;
    
    if (sd_mmc_spi_read_open_PDCA(sector))
    {
        pdca_load_channel(AVR32_PDCA_CHANNEL_SPI_RX, &pdcaRxBuf, bytes);
        pdca_load_channel(AVR32_PDCA_CHANNEL_SPI_TX, (void *)&pdcaTxBuf, bytes); //send dummy to activate the clock

        spi_write(SD_MMC_SPI, 0xFF);
            
        pdca_enable_interrupt_transfer_complete(AVR32_PDCA_CHANNEL_SPI_RX);
        
        pdca_enable(AVR32_PDCA_CHANNEL_SPI_RX);
        pdca_enable(AVR32_PDCA_CHANNEL_SPI_TX);
            
        //  wait for signal from ISR (irq_pdca)
        while (!(pdcaRxChan->isr & AVR32_PDCA_ISR_TRC_MASK));

        //  spi transfer
        for (i=0; i<bytes; i+=4)
        {            
            BFIN_SPI->tdr = MSG_SAMPLE_COM << AVR32_SPI_TDR_TD_OFFSET;
            while (!(BFIN_SPI->sr & AVR32_SPI_SR_TXEMPTY_MASK)) { ;; };

            BFIN_SPI->tdr = pdcaRxBuf[i] << AVR32_SPI_TDR_TD_OFFSET;
            while (!(BFIN_SPI->sr & AVR32_SPI_SR_TXEMPTY_MASK)) { ;; };
            
            BFIN_SPI->tdr = pdcaRxBuf[i + 1] << AVR32_SPI_TDR_TD_OFFSET;
            while (!(BFIN_SPI->sr & AVR32_SPI_SR_TXEMPTY_MASK)) { ;; };
            
            BFIN_SPI->tdr = pdcaRxBuf[i + 2] << AVR32_SPI_TDR_TD_OFFSET;
            while (!(BFIN_SPI->sr & AVR32_SPI_SR_TXEMPTY_MASK)) { ;; };

            BFIN_SPI->tdr = pdcaRxBuf[i + 3] << AVR32_SPI_TDR_TD_OFFSET;
            while (!(BFIN_SPI->sr & AVR32_SPI_SR_TXEMPTY_MASK)) { ;; };
        }
    }
    return 1;
}

