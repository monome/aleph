#include "BF533 Flags.h"

//--------------------------------------------------------------------------//
void Init_Flags(void)
{
  /*
	FIO_ANOM_0311_INEN_W(0x0100);// *pFIO_INEN		= 0x0100, workaround for anomaly 05000311.
	FIO_ANOM_0311_DIR_W(0x0000);// For more information please refer to the comment header in file sys/05000311.h
	FIO_ANOM_0311_EDGE_W(0x0100);
	FIO_ANOM_0311_MASKA_W(0x0100,pFIO_MASKA_D);
  */
  *pFIO_INEN = 0x0100;
  //  *pFIO_DIR = 0x0000;
  /// LEDS out on pf14, pf15
  *pFIO_DIR = 0xc000;

  *pFIO_EDGE = 0x0100;
  *pFIO_MASKA_D = 0x0100;
}



void Init_Timers(void)
{
	*pTIMER0_CONFIG		= 0x0019;
	*pTIMER0_PERIOD		= 0x00800000;
	*pTIMER0_WIDTH		= 0x00400000;
	*pTIMER_ENABLE		= 0x0001;
}

void Init_EBIU(void)
{
	*pEBIU_AMBCTL0	= 0x7bb07bb0;
	*pEBIU_AMBCTL1	= 0x7bb07bb0;
	*pEBIU_AMGCTL	= 0x000f;
}


void Init_Flash(void)
{
  //	*pFlashA_PortB_Dir = 0x3f;
}

void Init_Interrupts(void)
{
  int i;
  // assign core IDs to interrupts
  *pSIC_IAR0 = 0xffffffff;
  *pSIC_IAR1 = 0xffffffff;
  *pSIC_IAR2 = 0xffff5ff4;					// Timer0 -> ID4; FlagA -> ID5
  
  // assign ISRs to interrupt vectors
  //	register_handler(ik_ivg11, Timer0_ISR);		// Timer0 ISR -> IVG 11
  // register_handler(ik_ivg12, FlagA_ISR);		// FlagA ISR -> IVG 12
  
  *pEVT11 = Timer0_ISR;
  *pEVT12 = FlagA_ISR;
  // synchronize
  asm volatile ("cli %0; bitset (%0, 11); bitset (%0, 12); sti %0; csync;": "+d"(i));
  
  
  // enable Timer0 and FlagA interrupt
  *pSIC_IMASK = 0x00090000;
}

