#include "bfin_core.h"
#include "init.h"
#include "module.h"
#include "spi.h"
#include "types.h"
#include "isr.h"

//--------- global variables (initialized here)
// 4 channels of input from ad1836
fract32 in0, in1, in2, in3;
// 4 channels of output to ad1836
fract32 out0, out1, out2, out3;

// sport0 receive interrupt (audio input from codec)
void sport0_rx_isr() {
  fract32 outLevel;
  u8 leds = 0;
  // confirm interrupt handling
  *pDMA1_IRQ_STATUS = 0x0001;
  
  // copy input data from dma input buffer 
  // shift left from 24-bit
  // FIXME: data from codec is supposed to be left-justified in i2s mode...?
  in0 = iRxBuf[INTERNAL_ADC_L0] << 8;
  in1 = iRxBuf[INTERNAL_ADC_R0] << 8;
  in2 = iRxBuf[INTERNAL_ADC_L1] << 8;
  in3 = iRxBuf[INTERNAL_ADC_R1] << 8;
  
  //// output level meter
  outLevel = max_fr1x32(
			max_fr1x32(
				   max_fr1x32(
					      abs_fr1x32(out0),
					      abs_fr1x32(out1)
					      ),
				   abs_fr1x32(out2)
				   ),
			abs_fr1x32(out3)
			);
  
  if (outLevel > 1076291388)      { leds = 0x3f; }
  else if (outLevel > 539423503)  { leds = 0x3e; }
  else if (outLevel > 135497058)  { leds = 0x3c; }
  else if (outLevel > 53942350)   { leds = 0x38; }
  else if (outLevel > 8549286)    { leds = 0x30; }
  else if (outLevel > 2147484)    { leds = 0x20; }

  *pFlashA_PortB_Data = leds;
  /////

  // copy last frame's processing result to DMA output buffer
  // shift right to 24-bit
  iTxBuf[INTERNAL_DAC_L0] = out0 >> 8;

  iTxBuf[INTERNAL_DAC_R0] = out1 >> 8;
  iTxBuf[INTERNAL_DAC_L1] = out2 >> 8;
  iTxBuf[INTERNAL_DAC_R1] = out3 >> 8;

   // call the module-defined process function on this frame
   process_frame();
}

// spi receive interrupt (control change from avr32)
void spi_rx_isr() {
  // increment first so the message handler stays in sync
  spiByte = (spiByte + 1) % MSG_BYTES;
  // copy rx data to ringbuffer
  // reading from the rx data register also clears the rx interrupt
  *pSPI_TDBR = txMsg.raw[spiByte];
  rxMsg.raw[spiByte] = *pSPI_RDBR;
  handle_spi_rx();
}
