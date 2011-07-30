#include "bf533_audio_core.h"

void init_EBIU(void) {
  *pEBIU_AMBCTL0	= 0x7bb07bb0;
  *pEBIU_AMBCTL1	= 0x7bb07bb0;
  *pEBIU_AMGCTL	= 0x000f;
}

void init_flash(void) {
  // flash A, port A0 -> AD1836_reset
  *pFlashA_PortA_Dir = 0x1;
  // flash A, ports [B0, B5] -> [led1, led6]
  *pFlashA_PortB_Dir = 0x3f;
}

// setup SPI0 -> AD1836 config
void init_1836(void) {
  int i;
  int j;
   
  // write to Port A to reset AD1836
  *pFlashA_PortA_Data = 0x00;
  
  // write to Port A to enable AD1836
  *pFlashA_PortA_Data = 0x01;
  
  // wait to recover from reset
  for (i=0; i<0xf0000; i++) asm("nop;");
  
  // Enable PF4 peripheral function
  *pSPI_FLG = FLS4;
  // Set baud rate SCK = HCLK/(2*SPIBAUD) SCK = 2MHz	
  *pSPI_BAUD = 16;
  // configure spi port
  // SPI DMA write, 16-bit data, MSB first, SPI Master
  *pSPI_CTL = TIMOD_DMA_TX | SIZE | MSTR;
  
  // Set up DMA5 to transmit
  // Map DMA5 to SPI
  *pDMA5_PERIPHERAL_MAP	= 0x5000;
  
  // Configure DMA5
  // 16-bit transfers
  *pDMA5_CONFIG = WDSIZE_16;
  // Start address of data buffer
  *pDMA5_START_ADDR = (void *)sCodec1836TxRegs;
  // DMA inner loop count
  *pDMA5_X_COUNT = CODEC_1836_REGS_LENGTH;
  // Inner loop address increment
  *pDMA5_X_MODIFY = 2;
  // enable DMAs
  *pDMA5_CONFIG = (*pDMA5_CONFIG | DMAEN);
  // enable spi
  *pSPI_CTL = (*pSPI_CTL | SPE);

  // wait until DMA transfers for spi are finished 
  for (j=0; j<0xaff0; j++) asm("nop;");
  // disable spi
  *pSPI_CTL = 0x0000;

  /*
  /// uhh, wait some more?
  for (j=0; j<0xaff0; j++) asm("nop;");
  /// uhh, wait some more?
  for (j=0; j<0xaff0; j++) asm("nop;");
  /// uhh, wait some more?
  for (j=0; j<0xaff0; j++) asm("nop;");
  /// uhh, wait some more?
  for (j=0; j<0xaff0; j++) asm("nop;");
  /// uhh, wait some more?
  for (j=0; j<0xaff0; j++) asm("nop;");
  /// uhh, wait some more?
  for (j=0; j<0xaff0; j++) asm("nop;");
  /// uhh, wait some more?
  for (j=0; j<0xaff0; j++) asm("nop;");
  */
}

//--------------------------------------------------------------------//
// init_spi_slave()
// (re-)onfigure spi in slave mode to receive control data from avr32 
void init_spi_slave(void) {
  int j;
  int spi_stat_debug;
 

  /// gotta reset these fucks as inputs???
  // *pFIO_INEN = 0x0100;
  // *pFIO_DIR = 0x0000;
  
  //don't try to prouce a clock, you dumbass 
  *pSPI_BAUD = 0;
  // reset the flags register? to defaults?
  *pSPI_FLG = 0xff00;

  /// uhh, wait some more?
  //for (j=0; j<0xaff0; j++) asm("nop;");
    /// uhh, wait some more?
  //for (j=0; j<0xaff0; j++) asm("nop;");
  
  spi_stat_debug = *pSPI_STAT;

  // try clearing the rx error bit? (sticky - W1C)
  *pSPI_STAT |= 0x10;  
  *pSPI_STAT |= 0x10;  

  spi_stat_debug = *pSPI_STAT;

  // slave mode, 8 bit transfers, non-dma receive mode (interrupt when SPI_RDBR is full) 
  *pSPI_CTL = 0x0400;
  // uh, gotta write this guy again? (SSEL)
  // no, PSSE is only for master
  //  *pSPI_CTL |= PSSE;
  // enable transmit on MISO
  // *pSPI_CTL |= EMISO;
  
  spi_stat_debug = *pSPI_STAT;

  // enable spi (now in slave mode)
  *pSPI_CTL = (*pSPI_CTL | SPE);

  spi_stat_debug = *pSPI_STAT;

  // try clearing the rx error bit? (sticky - W1C)
  *pSPI_STAT |= 0x10;
  *pSPI_STAT |= 0x10;  

  spi_stat_debug = *pSPI_STAT;
}


//--------------------------------------------------------------------------//
// Function:	Init_sport0						
//									
// Description:	Configure Sport0 for I2S mode, to transmit/receive data 
//		to/from the AD1836. Configure Sport for external clocks and
//	        frame syncs.						
//--------------------------------------------------------------------------//
void init_sport0(void)
{
	// Sport0 receive configuration
	// External CLK, External Frame sync, MSB first, Active Low
	// 24-bit data, Stereo frame sync enable
	*pSPORT0_RCR1 = RFSR | RCKFE;
	*pSPORT0_RCR2 = SLEN_24 | RXSE | RSFSE;
	
	// Sport0 transmit configuration
	// External CLK, External Frame sync, MSB first, Active Low
	// 24-bit data, Secondary side enable, Stereo frame sync enable
	*pSPORT0_TCR1 = TFSR | TCKFE;
	*pSPORT0_TCR2 = SLEN_24 | TXSE | TSFSE;
}

//--------------------------------------------------------------------------//
// Function:	init_DMA						
//							
// Description:	Initialize DMA1 in autobuffer mode to receive and DMA2 in
//	        autobuffer mode to transmit		
//--------------------------------------------------------------------------//
void init_DMA(void)
{
	// Set up DMA1 to receive
	// Map DMA1 to Sport0 RX
	*pDMA1_PERIPHERAL_MAP = 0x1000;
	
	// Configure DMA1
	// 32-bit transfers, Interrupt on completion, Autobuffer mode
	*pDMA1_CONFIG = WNR | WDSIZE_32 | DI_EN | FLOW_1;
	// Start address of data buffer
	*pDMA1_START_ADDR = (void *)iRxBuf;
	// DMA inner loop count
	*pDMA1_X_COUNT = 4;
	// Inner loop address increment
	*pDMA1_X_MODIFY = 4;
	
	// Set up DMA2 to transmit
	// Map DMA2 to Sport0 TX
	*pDMA2_PERIPHERAL_MAP = 0x2000;
	
	// Configure DMA2
	// 32-bit transfers, Autobuffer mode
	*pDMA2_CONFIG = WDSIZE_32 | FLOW_1;
	// Start address of data buffer
	*pDMA2_START_ADDR = (void *)iTxBuf;
	// DMA inner loop count
	*pDMA2_X_COUNT = 4;
	// Inner loop address increment
	*pDMA2_X_MODIFY = 4;
}

void init_interrupts(void)
{
  int i;
  // set sport0 rx (dma1) interrupt priority to 2 = IVG9 
  // set spi rx interrupt priority to 3 = IVG10
  *pSIC_IAR0 = 0xffffffff;
  *pSIC_IAR1 = 0xff3fff2f;
  *pSIC_IAR2 = 0xffffffff;
  
  // assign ISRs to interrupt vectors
  // sport0 rx isr -> ivg 9
  // spi rx isr -> ivg 10 
  *pEVT9 = sport0_rx_isr;
  *pEVT10 = spi_rx_isr;
  asm volatile ("cli %0; bitset (%0, 9); bitset(%0, 10); sti %0; csync;": "+d"(i));
  
  // enable Sport0 RX interrupt, spi/dma5 interrupt
  *pSIC_IMASK = 0x2200;
}


void enable_DMA_sport0(void)
{
  // enable DMAs
  *pDMA2_CONFIG	= (*pDMA2_CONFIG | DMAEN);
  *pDMA1_CONFIG	= (*pDMA1_CONFIG | DMAEN);
  
  // enable Sport0 TX and RX
  *pSPORT0_TCR1 	= (*pSPORT0_TCR1 | TSPEN);
  *pSPORT0_RCR1 	= (*pSPORT0_RCR1 | RSPEN);
}
