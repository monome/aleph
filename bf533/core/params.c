/* params.c
   aleph-blackfin core audio system

   routines for parameter handling from the audio core
*/


#include "bf533_audio_core.h"
#include "../../common/protocol.h"
#include "params.h"

//--------- static vars
static unsigned char paramMsgCom;
static unsigned short int paramMsgIdx;

// deal with new data in the spi rx ringbuffer
void handle_spi_rx(void) {
  spiRxFlag = 0;

  if (spiRxRingIdx == P_PARAM_MSG_WORD_COM) {
    paramMsgCom = P_GET_PARAM_COM(spiRxRing[P_PARAM_MSG_WORD_COM]);
    paramMsgIdx = P_GET_PARAM_IDX(spiRxRing[P_PARAM_MSG_WORD_COM]);
    if (paramMsgCom == P_PARAM_COM_GETI) {
      // TODO: load param integer MSW for next transmissiom
    }
    if (paramMsgCom == P_PARAM_COM_GETF) {
      // TODO: load param float MSw for next transmission
    }
  }

  if (spiRxRingIdx ==  P_PARAM_MSG_WORD_DATAH) {
    if (paramMsgCom == P_PARAM_COM_GETI) {
      // TODO: load param integer LSW for next transmission
    }
    if (paramMsgCom == P_PARAM_COM_GETF) {
      // TODO: load param float LSw for next transmission
    }
  }

  if (spiRxRingIdx ==  P_PARAM_MSG_WORD_DATAL) {
    // we just received the last word, do something    
    if(paramMsgCom == P_PARAM_COM_SETI) {
      unsigned int iParamData = P_GET_PARAM_DATA(spiRxRing[P_PARAM_MSG_WORD_DATAH], spiRxRing[P_PARAM_MSG_WORD_DATAL]);
      request_param_set_int(paramMsgIdx, iParamData);
    }
    if(paramMsgCom == P_PARAM_COM_SETF) {
      float fParamData = P_GET_PARAM_DATA(spiRxRing[P_PARAM_MSG_WORD_DATAH], spiRxRing[P_PARAM_MSG_WORD_DATAL]);
      // FIXME: for now, always ramp, always sync
      request_param_set_float(paramMsgIdx, fParamData, 1, 1);
    }
  } 
}
