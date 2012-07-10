#ifndef _ALEPH_COMMON_PROTOCOL_H_
#define _ALEPH_COMMON_PROTOCOL_H_

#include "types.h"
#include "param.h"


// AVR32 <-> BLACKFIN
// SPI communication protocol

// all transactions will be 8 bits;
// this is necessary for bf533 spi-boot mode.

// since there is a transaction on each byte, 
// it is relatively easy to implemement variable-length messaging.

// generic message structure
typedef struct _msgGeneric {
  u8 command;
  u8 * data;
} msgGeneric_t;

// 0: set parameter
#define MSG_SET_PARAM_COM 0
typedef struct _msgSetParam {
  u8 command;
  u16 idx;
  ParamValue value; // 4 bytes
} msgSetParam_t;
//#define MSG_SET_PARAM_BYTES 7

// 1: get parameter
#define MSG_GET_PARAM_COM 1
typedef struct _msgGetParam {
  u8 command;
  u16 idx;
  ParamValue value; // 4 bytes
} msgGetParam_t;
//#define MSG_GET_PARAM_BYTES 7

// 2: get number of parameters
#define MSG_GET_NUM_PARAMS_COM 2
typedef struct _msgGetNumParams {
  u8 command;
  u16 value;
} msgGetNumParams_t;
//#define MSG_GET_NUM_PARAMS_BYTES 3

// 3: get parameter descriptor
#define MSG_GET_PARAM_DESC_COM 3
typedef struct _msgGetParamDesc {
  u8 command;
  u16 idx;
  ParamDesc desc;
} msgGetParamDesc_t;
/// WARNING: hack. must agree with param.h 
//#define MSG_GET_PARAM_DESC_BYTES (PARAM_DESC_BYTES + 1)

// abstract message type is a large union
//#define MSG_MAX_BYTES MSG_GET_PARAM_DESC_BYTES
// whatever...
#define MSG_MAX_BYTES 64
typedef union {
  u8 raw[MSG_MAX_BYTES];
  msgGeneric_t generic;
  msgSetParam_t setParam;
  msgGetParam_t getParam;
  msgGetNumParams_t getNumParams;
  msgGetParamDesc_t getParamDesc;
} msg_t;

#endif
