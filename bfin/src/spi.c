#include "module.h"
#include "protocol.h"
#include "types.h"
#include "util.h"
#include "spi.h"

//------ global variables (initialized here)
// receive message data 
msg_t rxMsg;
// transmit message data
msg_t txMsg;
// spi byte counter
u8 spiByte = MSG_BYTES_1;

//------- function definitions
// deal with new data in the spi rx ringbuffer
void handle_spi_rx(void) {
 
  // on the first byte, check if this transaction requires MISO
  if (spiByte == eComByte) {

    if (rxMsg.generic.command == COM_GET_NUM_PARAMS) {
      // fill tx message with number-of-params
      txMsg.numParams.value = moduleData->numParams;
      // byte swap for the fucking avr32
      //?????
            byte_swap((U8*)&(txMsg.numParams.value), 2);
    }

    // TODO: param name
    // TODO: param descriptor

  }
 
  // final byte -> do something with MOSI data
  if (spiByte == MSG_BYTES_1) {
    // set parameter
    if (rxMsg.generic.command == COM_SET_PARAM) {
      // avr32 is big endian! so shitty
      byte_swap(&(rxMsg.setParam.idx), 2);
      if (rxMsg.setParam.idx < moduleData->numParams) {
	byte_swap(&(rxMsg.setParam.value), 4);
	moduleData->paramData[rxMsg.setParam.idx].value = rxMsg.setParam.value;
	moduleData->paramData[rxMsg.setParam.idx].changed = 1;
      }
    }
  }

  
}
