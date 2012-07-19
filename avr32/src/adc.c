//ASF
#include "compiler.h"
#include "delay.h"
#include "print_funcs.h"
#include "spi.h"
// aleph
#include "conf_aleph.h"
#include "events.h"
#include "event_types.h"
#include "types.h"
#include "adc.h"

// ad7923 control register
#define AD7923_CTL_WRITE  0x800
#define AD7923_CTL_SEQ1   0x400
#define AD7923_CTL_ADD1   0x80
#define AD7923_CTL_ADD0   0x40
#define AD7923_CTL_PM1    0x20 
#define AD7923_CTL_PM0    0x10
#define AD7923_CTL_SEQ0   0x8
#define AD7923_CTL_RANGE  0x2
#define AD7923_CTL_CODING 0x1

// command word to which we add the desired channel address for conversion
#define AD7927_CMD_BASE ( AD7923_CTL_WRITE | AD7923_CTL_PM0 | AD7923_CTL_PM1 | AD7923_CTL_CODING )

// adc events
static const eEventType getAdcEvent(const u8 i) {
  switch(i) {
  case 0:
    return kEventAdc0;
    break;
  case 1:
    return kEventAdc1;
    break;
  case 2:
    return kEventAdc2;
    break;
  case 3:
    return kEventAdc3;
    break;
  default:
    return 0;
    break;
  }
}

// perform a conversion on all 4 channels
static void adc_convert(U16 (*dst)[4]) {
  U16 cmd, val;
  //U8 i;

  print_dbg("\r\n perofrming adc conversion on 4 channels...");

  // data into AD7927 is a left-justified 12-bit value in a 16-bit word
  // so, always lshift the command before sending
  cmd = ( AD7927_CMD_BASE ) << 4;

  // write to set up channel 0
  spi_selectChip(ADC_SPI, ADC_SPI_NPCS);
  spi_write(ADC_SPI, cmd);
  spi_unselectChip(ADC_SPI, ADC_SPI_NPCS);

  // get channel 0, setup channel 1
  cmd = ( AD7927_CMD_BASE | AD7923_CTL_ADD0 ) << 4;
  spi_selectChip(ADC_SPI, ADC_SPI_NPCS);
  spi_write(ADC_SPI, cmd);
  spi_read(ADC_SPI, &val);
  spi_unselectChip(ADC_SPI, ADC_SPI_NPCS);
  (*dst)[0] = val & 0xfff;

  print_dbg("\r\n got adc0 conversion: ");
  print_dbg_ulong(val);

  // get channel 1, setup channel 2
  cmd = ( AD7927_CMD_BASE | AD7923_CTL_ADD1 ) << 4;
  spi_selectChip(ADC_SPI, ADC_SPI_NPCS);
  spi_write(ADC_SPI, cmd);
  spi_read(ADC_SPI, &val);
  spi_unselectChip(ADC_SPI, ADC_SPI_NPCS);
  (*dst)[1] = val & 0xfff;

  // get channel 2, setup channel 3
  cmd = ( AD7927_CMD_BASE | AD7923_CTL_ADD1 | AD7923_CTL_ADD0 ) << 4;
  spi_selectChip(ADC_SPI, ADC_SPI_NPCS);
  spi_write(ADC_SPI, cmd);
  spi_read(ADC_SPI, &val);
  spi_unselectChip(ADC_SPI, ADC_SPI_NPCS);
  (*dst)[2] = val & 0xfff;
  
  // get channel 3, dummy write
  cmd = ( AD7927_CMD_BASE ) << 4;
  spi_selectChip(ADC_SPI, ADC_SPI_NPCS);
  spi_write(ADC_SPI, cmd);
  spi_read(ADC_SPI, &val);
  spi_unselectChip(ADC_SPI, ADC_SPI_NPCS);
  (*dst)[3] = val & 0xfff;
}

// setup ad7923
void init_adc(void) {
  int i;
  u16 cmd;

  // at powerup, the part wants a dummy conversion with DIN high
  spi_selectChip(ADC_SPI, ADC_SPI_NPCS);
  spi_write(ADC_SPI, 0xffff);
  spi_unselectChip(ADC_SPI, ADC_SPI_NPCS);

  // wait for powerup time (1us in datasheet)
   for(i=0; i<100; i++) { ;; }
  delay_ms(1);

  /// ok... do it twice
  spi_selectChip(ADC_SPI, ADC_SPI_NPCS);
  spi_write(ADC_SPI, 0xffff);
  spi_unselectChip(ADC_SPI, ADC_SPI_NPCS);

  // wait for powerup time (1us in datasheet)
   for(i=0; i<100; i++) { ;; }
  //  delay_ms(1);
   
   // write base configuration
   cmd = AD7927_CMD_BASE << 4;
   spi_selectChip( ADC_SPI, ADC_SPI_NPCS );
   spi_write( ADC_SPI, cmd );
   spi_unselectChip( ADC_SPI, ADC_SPI_NPCS );

}

// perform conversion, check for changes, and post events
void adc_poll(void) {
  static u16 adcVal[4];
  static u16 adcOldVal[4];
  static event_t e;
  u8 i;
  adc_convert(&adcVal);
  for(i=0; i<4; i++) {
    // TODO:
    /// probably want more filtering before posting events
    if(adcVal[i] != adcOldVal[i]) {
      adcOldVal[i] = adcVal[i];
      e.eventType = getAdcEvent(i); //kAdcEvents[i];
      e.eventData = (S16)(adcVal[i]);
    }
  }
}
