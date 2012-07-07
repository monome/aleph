
#include "bfin_core.h"
#include "isr.h"
#include "init.h"

//------ global variables initialized here

// AD1836 config registers
static volatile short codec1836TxRegs[CODEC_1836_REGS_LENGTH] = {
  DAC_CONTROL_1	| 0x000,
  DAC_CONTROL_2	| 0x000,
  DAC_VOLUME_0	| 0x3ff,
  DAC_VOLUME_1	| 0x3ff,
  DAC_VOLUME_2	| 0x3ff,
  DAC_VOLUME_3	| 0x3ff,
  DAC_VOLUME_4	| 0x000,
  DAC_VOLUME_5	| 0x000,
  ADC_CONTROL_1	| 0x000,
  ADC_CONTROL_2	| 0x000,
  ADC_CONTROL_3	| 0x000
};

// SPORT0 DMA transmit buffer
volatile s32 iTxBuf[4];
// SPORT0 DMA receive buffer
volatile s32 iRxBuf[4];

//----- function definitions
void init_EBIU(void) {
  //  straight from the self-test example project
// Initalize EBIU control registers to enable all banks	
  *pEBIU_AMBCTL1 = 0xFFFFFF02;
  ssync();
  
  // -- not sure why there is a read here, possibly anomaly 05000157?
  //temp = *pEBIU_AMBCTL1;
  //temp++;
  
  *pEBIU_AMGCTL = 0x00FF;
  ssync();
    
  // Check if already enabled
  if( SDRS != ((*pEBIU_SDSTAT) & SDRS) ) {
      return;
  }
    
  //SDRAM Refresh Rate Control Register
  *pEBIU_SDRRC = 0x01A0; 
  
  //SDRAM Memory Bank Control Register
  *pEBIU_SDBCTL = 0x0025; //1.7	64 MB
  //	*pEBIU_SDBCTL = 0x0013;	//1.6 and below 32 MB
  
  //SDRAM Memory Global Control Register	
  *pEBIU_SDGCTL = 0x0091998d;//0x998D0491;
  ssync();
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
  // point DMA at the configuration data
  *pDMA5_START_ADDR = (void *)codec1836TxRegs;
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
  // disable dma5
  *pDMA5_CONFIG = (*pDMA5_CONFIG & ~DMAEN);

}

//--------------------------------------------------------------------//
// init_spi_slave()
// (re-)onfigure spi in slave mode to receive control data from avr32 
void init_spi_slave(void) {
  int j;
  int spi_stat_debug;
 
  //don't try to produce a clock, you dumbass 
  *pSPI_BAUD = 0;
  // reset the flags register? to defaults?
  *pSPI_FLG = 0xff00;

  // try clearing the rx error bit? (sticky - W1C)
  *pSPI_STAT |= 0x10;  
  *pSPI_STAT |= 0x10;  

  // slave mode, 16 bit transfers, MSB first, non-dma rx mode, overwrite (interrupt when SPI_RDBR is full) 
  *pSPI_CTL = CPHA | SIZE | GM;

  // enable transmit on MISO
  *pSPI_CTL |= EMISO;
  
  spi_stat_debug = *pSPI_STAT;

  // enable spi (now in slave mode)
  *pSPI_CTL = (*pSPI_CTL | SPE);

  spi_stat_debug = *pSPI_STAT;

  // clear the spi rx register by reading from it
  j = *pSPI_RDBR;
  // clear the rx error bit (sticky - W1C)
  *pSPI_STAT |= 0x10;
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
  int i=0;
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
