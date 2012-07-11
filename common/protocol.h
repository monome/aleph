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
typedef struct __attribute__((__packed__)) _msgGeneric {
  u8 command;
  u8 * data;
} msgGeneric_t;

// 0: set parameter
#define MSG_SET_PARAM_COM 0
typedef struct __attribute__((__packed__)) _msgSetParam {
  u8 command;
  u8 idx;
  ParamValue value; // 4 bytes
} msgSetParam_t;

// 1: get parameter
#define MSG_GET_PARAM_COM 1
typedef struct __attribute__((__packed__)) _msgGetParam {
  u8 command;
  u8 idx;
  ParamValue value; // 4 bytes
} msgGetParam_t;

// 2: get number of parameters
#define MSG_GET_NUM_PARAMS_COM 2
typedef struct __attribute__((__packed__)) _msgGetNumParams {
  u8 command;
  u8 value;
} msgGetNumParams_t;

// 3: get parameter descriptor
#define MSG_GET_PARAM_DESC_COM 3
typedef struct __attribute__((__packed__)) _msgGetParamDesc {
  u8 command;
  u8 idx;
  ParamDesc desc;
} msgGetParamDesc_t;

// abstract message type is a large union
//#define MSG_MAX_BYTES MSG_GET_PARAM_DESC_BYTES
// whatever...
#define MSG_MAX_BYTES 64
typedef union __attribute__((__packed__)) {
  u8 raw[MSG_MAX_BYTES];
  msgGeneric_t generic;
  msgSetParam_t setParam;
  msgGetParam_t getParam;
  msgGetNumParams_t getNumParams;
  msgGetParamDesc_t getParamDesc;
} msg_t;

#endif
