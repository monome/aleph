#include "bfin_core.h"
#include "dac.h"

// AD5686R 16-bit quad DAC

#define DAC_COM_WRITE 0x3
#define DAC_COM_LSHIFT 20
#define DAC_ADDR_LSHIFT 16

#define DAC_COM_WRITE 0x3
#define DAC_COM_LSHIFT 20
#define DAC_ADDR_LSHIFT 16

//====================
//=== global variables , initialized here
volatile u32 cvTxBuf = 0;

//=============================
// extern functions
void dac_update(u8 ch, u16 val) {
  static u32 buf;
  buf = 0;
  buf |= (DAC_COM_WRITE << DAC_COM_LSHIFT);
  buf |= ((1 << ch) << DAC_ADDR_LSHIFT);
  buf |= val;
  cvTxBuf = buf << 1;
}
