#ifndef _ALEPH_COMMON_PROTOCOL_H_
#define _ALEPH_COMMON_PROTOCOL_H_

#include "module_common.h"
#include "param_common.h"
#include "types.h"

// super caveman version of SPI protocol

//---- com byte defines
#define MSG_SET_PARAM_COM           0 
#define MSG_GET_PARAM_COM           1
#define MSG_GET_NUM_PARAMS_COM      2
#define MSG_GET_PARAM_DESC_COM      3
#define MSG_GET_MODULE_NAME_COM     4
#define MSG_GET_MODULE_VERSION_COM  5
#define MSG_ENABLE_AUDIO            6
#define MSG_DISABLE_AUDIO           7

// nodes in a state machine
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
  // unit
  eParamDescUnit0,
  eParamDescUnit1,
  eParamDescUnit2,
  eParamDescUnit3,
  eParamDescUnit4,
  eParamDescUnit5,
  eParamDescUnit6,
  eParamDescUnit7,
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
  eNumSpiBytes
} eSpiByte;

#endif
