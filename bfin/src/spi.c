#include "types.h"
#include "../../common/protocol.h"
#include "spi.h"

//------ global variables (initialized here)
// ringbuffer for spi rx data
u16 spiRxRing[P_PARAM_MSG_WORD_COUNT];
// counter 
u16 spiRxRingIdx = P_PARAM_MSG_WORD_COUNT_1;

//--------- static vars
// command segment of current SPI message
static u8 paramMsgCom;
// data segment of current SPI message
static u16 paramMsgIdx;

//------- function definitions
// deal with new data in the spi rx ringbuffer
void handle_spi_rx(void) {

  if (spiRxRingIdx == P_PARAM_MSG_WORD_COM) {
    paramMsgCom = P_GET_PARAM_COM(spiRxRing[P_PARAM_MSG_WORD_COM]);
    paramMsgIdx = P_GET_PARAM_IDX(spiRxRing[P_PARAM_MSG_WORD_COM]);
    if (paramMsgCom == P_PARAM_COM_GET) {
      // TODO: load param integer MSW for next transmission?
    }
  }

  if (spiRxRingIdx ==  P_PARAM_MSG_WORD_DATAH) {
    if (paramMsgCom == P_PARAM_COM_GET) {
      // TODO: load param integer LSW for next transmission?
    }
  }

  if (spiRxRingIdx ==  P_PARAM_MSG_WORD_DATAL) {
    // we just received the last word, do something    
    if(paramMsgCom == P_PARAM_COM_SET) {
      /*
      u32 iParamData = P_GET_PARAM_DATA(spiRxRing[P_PARAM_MSG_WORD_DATAH], spiRxRing[P_PARAM_MSG_WORD_DATAL]);
      // request_param_set_int(paramMsgIdx, iParamData);
      */
    }
   } 
}
