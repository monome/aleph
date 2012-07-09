#include "module.h"
#include "protocol.h"
#include "types.h"
#include "spi.h"

//------ global variables (initialized here)
// ringbuffer for spi rx data
u8 spiRxRing[MSG_BYTES];
// byte counter, intiailize to last value
u8 spiRxRingIdx = MSG_BYTES_1;

//--------- static vars
// generic message 
static msg_t msg;

//------- function definitions
// deal with new data in the spi rx ringbuffer
void handle_spi_rx(void) {
  // copy the raw byte value
  msg.raw[spiRxRingIdx] = spiRxRing[spiRxRingIdx];

  if (spiRxRingIdx == MSG_BYTES_1) {
    // on receiving the final byte, do something 
    if (msg.generic.command == COM_SET_PARAM) {
      // avr32 is big endian! so shitty
      byte_swap(&(msg.setParam.idx), 2);
      if (msg.setParam.idx < moduleData->numParams) {
	byte_swap(&(msg.setParam.value), 4);
	moduleData->paramData[msg.setParam.idx].value = msg.setParam.value;
	moduleData->paramData[msg.setParam.idx].changed = 1;
      }
    }
  }
  // TODO: check if we need to load something for MISO!
}
