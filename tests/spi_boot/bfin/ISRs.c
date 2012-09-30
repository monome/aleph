#include "BF533 Flags.h"

void Timer0_ISR() {
// confirm interrupt handling
 *pTIMER_STATUS = 0x0001;
 
 if (++gLedState > 0x3f) { gLedState = 0; }

}

//--------------------------------------------------------------------------//
void FlagA_ISR() {
  // confirm interrupt handling
  //	FIO_ANOM_0311_FLAG_W(0x0100,pFIO_FLAG_C);//*pFIO_FLAG_C = 0x0100, workaround for anomaly 05000311.
  *pFIO_FLAG_C = 0x0100;
  
  //  gButState = ~gButState;
  if ( gButState ) { gButState = 0; }
  else { gButState = 1; }

}
