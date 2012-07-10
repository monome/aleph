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
u8 spiByte = 0;
u8 spiLastByte = 0;
static s8 idx;

//------- function definitions
// deal with new data in the spi rx ringbuffer
void handle_spi_rx(void) {
 
  // on the first byte, check the message type
  if (spiByte == 0) {
      switch(rxMsg.generic.command) {
    case MSG_SET_PARAM_COM:
      spiLastByte = sizeof(msgSetParam_t) - 1;
      // no tx data
      break;
    case MSG_GET_PARAM_COM:
      spiLastByte = sizeof(msgGetParam_t) - 1;
      // tx fill must wait for idx
      break;
    case MSG_GET_NUM_PARAMS_COM:
      spiLastByte = sizeof(msgGetNumParams_t) - 1;
      txMsg.getNumParams.value = moduleData->numParams;
      // byte swap for the fucking avr32
      byte_swap((U8*)&(txMsg.getNumParams.value), 2);
      break;
    case MSG_GET_PARAM_DESC_COM:
      spiLastByte = sizeof(msgGetParamDesc_t) - 1;
      // tx fill must wait for idx..
      break;
    }
  }
 
  // FIXME: hacky way to check for index byte
  if (spiByte == 1) {
    if (rxMsg.generic.command == MSG_GET_PARAM_DESC_COM) {
      idx = rxMsg.getParamDesc.idx;
      // byte swap the index
      byte_swap((U8*)&idx, 2);
      txMsg.getParamDesc.desc = moduleData->paramDesc[idx];
      /// byte-swap the float fields
      byte_swap((U8*)(&txMsg.getParamDesc.desc.min), 4);
      byte_swap((U8*)(&txMsg.getParamDesc.desc.max), 4);
    }
    if (rxMsg.generic.command == MSG_GET_PARAM_COM) {
      // byte swap the index
      idx = rxMsg.getParam.idx;
      byte_swap(&idx, 2);
      txMsg.getParam.value = moduleData->paramData[idx].value;
      // byte-swap the float fields
      byte_swap(&txMsg.getParam.value.asUint, 4);
    }
  }

  // final byte -> do something with MOSI data
  if (spiByte == spiLastByte) {
    // set parameter
    if (rxMsg.generic.command == MSG_SET_PARAM_COM) {
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
