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

//  command bytes
#define MSG_SET_PARAM_COM           0
#define MSG_GET_PARAM_COM           1
#define MSG_GET_NUM_PARAMS_COM      2
//#define MSG_GET_PARAM_DESC_COM      3 //not using, but leave reserved
#define MSG_GET_MODULE_NAME_COM     4

//  get 4-byte version packet
#define MSG_GET_MODULE_VERSION_COM  5
#define MSG_ENABLE_AUDIO           6
#define MSG_DISABLE_AUDIO           7
// fill buffer
#define MSG_FILL_BUFFER_COM	    8

//  set trig, calls module_set_event()
#define MSG_SET_EVENT_COM            8

//  send sample offset and size
#define MSG_NEW_SAMPLE_COM          9

//  transfer single s32 sample value from avr32
#define MSG_TRANSFER_SAMPLE_COM     10

//  set sequenced parameter value
#define MSG_SET_SQPARAM_COM         11

//  get recording head status, off | armed | on for led control
#define MSG_GET_HEADSTATUS_COM      12

//  get playhead position
#define MSG_GET_HEADPOSITION_COM    13


//  enumerate state-machine nodes for sending and receiving SPI.
//  WARNING!
//  the order of this enum is assumed by state machines
//  in avr32 and aleph.
//  do not change order without updating the logic as well!
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
<<<<<<< HEAD
    
#if 0
    //---- get param descriptor
  /// param descriptors in aleph-module.dsc
  /* eParamDescIdx, */
  /* // label */
  /* eParamDescLabel0, */
  /* eParamDescLabel1, */
  /* eParamDescLabel2, */
  /* eParamDescLabel3, */
  /* eParamDescLabel4, */
  /* eParamDescLabel5, */
  /* eParamDescLabel6, */
  /* eParamDescLabel7, */
  /* eParamDescLabel8, */
  /* eParamDescLabel9, */
  /* eParamDescLabel10, */
  /* eParamDescLabel11, */
  /* eParamDescLabel12, */
  /* eParamDescLabel13, */
  /* eParamDescLabel14, */
  /* eParamDescLabel15, */
  /* // type */
  /* eParamDescType, */
  /* // min */
  /* eParamDescMin0, */
  /* eParamDescMin1, */
  /* eParamDescMin2, */
  /* eParamDescMin3, */
  /* // max */
  /* eParamDescMax0, */
  /* eParamDescMax1, */
  /* eParamDescMax2, */
  /* eParamDescMax3, */
  /* // radix */
  /* eParamDescRadix, */
#endif
=======
  //---- get param descriptor
  /// param descriptors in aleph-module.dsc
>>>>>>> upstream/dev
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

  // module version
  eModuleVersionMaj,
  eModuleVersionMin,
  eModuleVersionRev0,
  eModuleVersionRev1,
    
    //  new sample
    eBufferOffset0,
    eBufferOffset1,
    eBufferOffset2,
    eBufferOffset3,
    eBufferSize0,
    eBufferSize1,
    eBufferSize2,
    eBufferSize3,
    
    //  transfer sample value
    eBufferSample0,
    eBufferSample1,
    eBufferSample2,
    eBufferSample3,

    //  set sq param value
    eSetParamStep0,
    eSetParamStep1,
    eSetParamStep2,
    eSetParamStep3,
    eSetParamStepIdx,
    eSetParamStepData0,
    eSetParamStepData1,
    eSetParamStepData2,
    eSetParamStepData3,
    
    eHeadState,

    eGetHeadPosition,

<<<<<<< HEAD
    eNumSpiBytes
=======
  // buffer 
  eBufferNumBytes0,
  eBufferNumBytes1,
  eBufferNumBytes2,
  eBufferNumBytes3,
  eBufferNumData,
  
  eNumSpiBytes
>>>>>>> upstream/dev
} eSpiByte;

#endif
