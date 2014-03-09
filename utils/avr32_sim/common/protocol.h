/* 
   protocol.h
   
   common
   aleph

   SPI procotol definitions.

 */

#ifndef _ALEPH_COMMON_PROTOCOL_H_
#define _ALEPH_COMMON_PROTOCOL_H_

#include "module_common.h"
#include "param_common.h"
#include "types.h"

//// command bytes
#define MSG_SET_PARAM_COM           0 
#define MSG_GET_PARAM_COM           1
#define MSG_GET_NUM_PARAMS_COM      2
#define MSG_GET_PARAM_DESC_COM      3 // not using, but leave reserved
#define MSG_GET_MODULE_NAME_COM     4
// get 4-byte version packet
#define MSG_GET_MODULE_VERSION_COM  5
#define MSG_ENABLE_AUDIO            6
#define MSG_DISABLE_AUDIO           7

// enumerate state-machine nodes for sending and receiving SPI.

/// WARNING!
/// the order of this enum is assumed by state machines
// in avr32 and aleph.
/// do not change order without updating the logic as well!
typedef enum {
  //---- command
  eCom,
  //---- set param
  eSetParamIdx,
  eSetParamData0,
  eSetParamData1,
  eSetParamData2,
  eSetParamData3,
  //---- get param
  eGetParamIdx,
  eGetParamData0,
  eGetParamData1,
  eGetParamData2,
  eGetParamData3,
  //---- get num params
  eNumParamsVal,
  //---- get param descriptor
  /// param descriptors in aleph-module.dsc
#if 0
  eParamDescIdx,
  // label
  eParamDescLabel0,
  eParamDescLabel1,
  eParamDescLabel2,
  eParamDescLabel3,
  eParamDescLabel4,
  eParamDescLabel5,
  eParamDescLabel6,
  eParamDescLabel7,
  eParamDescLabel8,
  eParamDescLabel9,
  eParamDescLabel10,
  eParamDescLabel11,
  eParamDescLabel12,
  eParamDescLabel13,
  eParamDescLabel14,
  eParamDescLabel15,
  // type
  eParamDescType,
  // min
  eParamDescMin0,
  eParamDescMin1,
  eParamDescMin2,
  eParamDescMin3,
  // max
  eParamDescMax0,
  eParamDescMax1,
  eParamDescMax2,
  eParamDescMax3,
  // radix
  eParamDescRadix,
#endif
  // get module name
  eGetModuleName,
  // module name bytes
  eModuleName0,
  eModuleName1,
  eModuleName2,
  eModuleName3,
  eModuleName4,
  eModuleName5,
  eModuleName6,
  eModuleName7,
  eModuleName8,
  eModuleName9,
  eModuleName10,
  eModuleName11,
  eModuleName12,
  eModuleName13,
  eModuleName14,
  eModuleName15,
  eModuleName16,
  eModuleName17,
  eModuleName18,
  eModuleName19,
  eModuleName20,
  eModuleName21,
  eModuleName22,
  eModuleName23,
#if 0 // include these in offline desc
  //---- added:
  //---- buffer stuff
  /// get number of buffers
  eNumBuffersVal,
  //-- get buffer descriptor
  eBufferDescIdx,
  eBufferDescChannels,
  eBufferDescFrames,
  //-- send buffer data
  eSendBufferData,
#endif

  // module version
  eModuleVersionMaj,
  eModuleVersionMin,
  eModuleVersionRev0,
  eModuleVersionRev1,

  
  eNumSpiBytes
} eSpiByte;

#endif
