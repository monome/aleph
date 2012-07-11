#include "module.h"
//#include "protocol.h"
#include "param.h"
#include "protocol_dumb.h"
#include "types.h"
#include "util.h"
#include "spi.h"

//------ global variables (initialized here)
// receive message data 
//msg_t rxMsg;
// transmit message data
//msg_t txMsg;
// spi byte counter
//u8 spiByte = 0;

//u8 spiLastByte = 0;

// last byte of index field
//static const u8 kIdxByte = 1;

// byte to process
static eSpiByte byte = eCom;
// current command
static u8 com;
// current param index
static u8 idx;

/// DEBUG
// static u16 dbgDscIdx[32][2];
// static u8 n = 0;


//------- function definitions
// deal with new data in the spi rx ringbuffer
u8 handle_spi_rx(u8 rx) {
  static ParamValue pval;
  switch(byte) {
  /// caveman style
  case eCom :
    com = rx;
    switch(com) {
    case MSG_SET_PARAM_COM:
      byte = eSetParamIdx;
      break;
    case MSG_GET_PARAM_COM:
      byte = eGetParamIdx;
      break;
    case MSG_GET_NUM_PARAMS_COM:
      byte = eNumParamsVal;
      return moduleData->numParams; // load num params
      break;
    case MSG_GET_PARAM_DESC_COM:
      byte = eParamDescIdx;
      break;
    default:
      break;
    }
    break;
  //---- set param
  case eSetParamIdx :
    idx = rx; // set index
    byte = eSetParamData0;
    return 0; // dont care
    break;
  case eSetParamData0 :
    byte = eSetParamData1;
    // byte-swap from BE on avr32
    moduleData->paramData[idx].value.asByte[3] = rx; // set paramval
    return 0; // don't care
    break;
  case eSetParamData1 :
    byte = eSetParamData2;
    // byte-swap from BE on avr32
    moduleData->paramData[idx].value.asByte[2] = rx; // set paramval
    return 0; // don't care
    break;
  case eSetParamData2 :
    byte = eSetParamData3;
    // byte-swap from BE on avr32
    moduleData->paramData[idx].value.asByte[1] = rx; // set paramval
    return 0; // don't care
    break;
  case eSetParamData3 :
    // byte-swap from BE on avr32
    moduleData->paramData[idx].value.asByte[0] = rx;
    moduleData->paramData[idx].changed = 1; // done -> mark changed
    byte = 0; //reset
    return 0; // don't care
    break;
  //---- get num params
  case eNumParamsVal :
    byte = 0; //reset
    return 0; // don't care 
    break;
  //---- get param descriptor
  case eParamDescIdx :
    byte = eParamDescLabel0;
    idx = rx;
    return moduleData->paramDesc[idx].label[0]; // load label 
    break;
  // label
  case eParamDescLabel0 :
    byte = eParamDescLabel1;
    return moduleData->paramDesc[idx].label[1]; // load label 
    break;
  case eParamDescLabel1 :
    byte = eParamDescLabel2;
    return moduleData->paramDesc[idx].label[2]; // load label 
    break;
  case eParamDescLabel2 :
    byte = eParamDescLabel3;
    return moduleData->paramDesc[idx].label[3]; // load label 
    break;
  case eParamDescLabel3 :
    byte = eParamDescLabel4;
    return moduleData->paramDesc[idx].label[4]; // load label 
    break;
  case eParamDescLabel4 :
    byte = eParamDescLabel5;
    return moduleData->paramDesc[idx].label[5]; // load label 
    break;
  case eParamDescLabel5 :
    byte = eParamDescLabel6;
    return moduleData->paramDesc[idx].label[6]; // load label 
    break;
  case eParamDescLabel6 :
    byte = eParamDescLabel7;
    return moduleData->paramDesc[idx].label[7]; // load label 
    break;
  case eParamDescLabel7 :
    byte = eParamDescLabel8;
    return moduleData->paramDesc[idx].label[8]; // load label 
    break;
  case eParamDescLabel8 :
    byte = eParamDescLabel9;
    return moduleData->paramDesc[idx].label[9]; // load label 
    break;
  case eParamDescLabel9 :
    byte = eParamDescLabel10;
    return moduleData->paramDesc[idx].label[10]; // load label 
    break;
  case eParamDescLabel10 :
    byte = eParamDescLabel11;
    return moduleData->paramDesc[idx].label[11]; // load label 
    break;
  case eParamDescLabel11 :
    byte = eParamDescLabel12;
    return moduleData->paramDesc[idx].label[12]; // load label 
    break;
  case eParamDescLabel12 :
    byte = eParamDescLabel13;
    return moduleData->paramDesc[idx].label[13]; // load label 
    break;
  case eParamDescLabel13 :
    byte = eParamDescLabel14;
    return moduleData->paramDesc[idx].label[14]; // load label 
    break;
  case eParamDescLabel14 :
    byte = eParamDescLabel15;
    return moduleData->paramDesc[idx].label[15]; // load label 
    break;
  case eParamDescLabel15 :
    byte = eParamDescUnit0;
    return moduleData->paramDesc[idx].unit[0]; // load label 
    break;
  // unit
  case eParamDescUnit0 :
    byte = eParamDescUnit1;
    return moduleData->paramDesc[idx].unit[1]; // load unit
    break;
  case eParamDescUnit1 :
    byte = eParamDescUnit2;
    return moduleData->paramDesc[idx].unit[2]; // load unit
    break;
  case eParamDescUnit2 :
    byte = eParamDescUnit3;
    return moduleData->paramDesc[idx].unit[3]; // load unit
    break;
  case eParamDescUnit3 :
    byte = eParamDescUnit4;
    return moduleData->paramDesc[idx].unit[4]; // load unit
    break;
  case eParamDescUnit4 :
    byte = eParamDescUnit5;
    return moduleData->paramDesc[idx].unit[5]; // load unit
    break;
  case eParamDescUnit5 :
    byte = eParamDescUnit6;
    return moduleData->paramDesc[idx].unit[6]; // load unit
    break;
  case eParamDescUnit6 :
    byte = eParamDescUnit7;
    return moduleData->paramDesc[idx].unit[7]; // load unit
    break;
  case eParamDescUnit7 :
    byte = eParamDescType;
    return moduleData->paramDesc[idx].type; // load type
    break;
  // type
  case eParamDescType :
    byte = eParamDescMin0;
    pval.asFloat = moduleData->paramDesc[idx].min;
    return pval.asByte[3]; // min, high byte
      break;
  // min  // === byteswap for BE on avr32
  case eParamDescMin0 :
    byte = eParamDescMin1;
    return pval.asByte[2];
      break;
  case eParamDescMin1 :
    byte = eParamDescMin1;
    return pval.asByte[1];

    break;
  case eParamDescMin2 :
    byte = eParamDescMin1;
    return pval.asByte[0]; // min, low byte
  
    break;
  case eParamDescMin3 :
    byte = eParamDescMax0;
    pval.asFloat = moduleData->paramDesc[idx].max;
    return pval.asByte[3]; // max, high byte
      break;
  // max  // === byteswap for BE on avr32
  case eParamDescMax0 :
    byte = eParamDescMax1;
    return pval.asByte[2];
     break;
  case eParamDescMax1 :
    byte = eParamDescMax2;
    return pval.asByte[1];
     break;
  case eParamDescMax2 :
    byte = eParamDescMax3;
    return pval.asByte[0]; // max, low byte
  
    break;
  case eParamDescMax3 :
    byte = eCom; // reset
    return 0; // dont care
    break;
  default:
    return 0;
    break;
  }
}




  /*
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
      // byte_swap((U8*)(&(txMsg.getNumParams.value)), 2);
      break;
    case MSG_GET_PARAM_DESC_COM:
      spiLastByte = sizeof(msgGetParamDesc_t) - 1;
      // tx fill must wait for idx..
      break;
    default:
      break;
    }
  }
 
  // spi counter at last byte of idx field;
  // so parse the idx and fill remainder of tx message data
  if (spiByte == kIdxByte) {
    if (rxMsg.generic.command == MSG_GET_PARAM_DESC_COM) {
      //   U8 dum;
      idx = rxMsg.getParamDesc.idx;

      //      dbgDscIdx[n][0] = idx;

      // byte swap the index
      //byte_swap((u8*)(&idx), 2);

   
      txMsg.getParamDesc.desc = moduleData->paramDesc[idx];
      /// byte-swap the float fields
      byte_swap((u8*)(&txMsg.getParamDesc.desc.min), 4);
      byte_swap((u8*)(&txMsg.getParamDesc.desc.max), 4);
    }
    if (rxMsg.generic.command == MSG_GET_PARAM_COM) {
     
      idx = rxMsg.getParam.idx;
      // byte swap the index     
      //byte_swap((u8*)(&idx), 2);
      txMsg.getParam.value = moduleData->paramData[idx].value;
      // byte-swap the float fields
      byte_swap((u8*)(&txMsg.getParam.value), 4);
    }
  }

  // final byte -> do something with MOSI data
  if (spiByte == spiLastByte) {
    // set parameter
    if (rxMsg.generic.command == MSG_SET_PARAM_COM) {
      // avr32 is big endian! so shitty
      //byte_swap((u8*)(&(rxMsg.setParam.idx)), 2);
      if (rxMsg.setParam.idx < moduleData->numParams) {
	byte_swap((u8*)(&(rxMsg.setParam.value)), 4);
	moduleData->paramData[rxMsg.setParam.idx].value = rxMsg.setParam.value;
	moduleData->paramData[rxMsg.setParam.idx].changed = 1;
      }
    }
  }
}
*/
