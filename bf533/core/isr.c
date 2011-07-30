#include "bf533_audio_core.h"
#include "../../common/protocol.h"

// state machine and variables for receiving parameter changes/requests
static eParamMsgState paramMsgState = eCommand;
static char paramMsgCom;
static short int paramMsgIdx;
static short int paramMsgDataL;
static short int paramMsgDataH; 
static unsigned int paramMsgData;

// sport0 receive interrupt (audio input from codec)
void sport0_rx_isr() {
  // confirm interrupt handling
  *pDMA1_IRQ_STATUS = 0x0001;
  
  // copy input data from dma input buffer into variables
  // shift left from 24-bit
  // FIXME: data from codec is supposed to be left-justified in i2s mode...
  iIn00 = iRxBuf[INTERNAL_ADC_L0] << 8;
  iIn01 = iRxBuf[INTERNAL_ADC_R0] << 8;
  iIn10 = iRxBuf[INTERNAL_ADC_L1] << 8;
  iIn11 = iRxBuf[INTERNAL_ADC_R1] << 8;
  
  // call the module-defined process function
  process_frame();
  
  // copy processed data from variables into dma output buffer
  // shift right to 24-bit
  iTxBuf[INTERNAL_DAC_L0] = iOut00 >> 8;
  iTxBuf[INTERNAL_DAC_R0] = iOut01 >> 8;
  iTxBuf[INTERNAL_DAC_L1] = iOut10 >> 8;
  iTxBuf[INTERNAL_DAC_R1] = iOut11 >> 8;
}

// spi receive interrupt (control change from avr32)
void spi_rx_isr() {
  unsigned int spiData; 
  // reading the spi receive data register also clears the interrupt
  spiData = *pSPI_RDBR;

  switch(paramMsgState) {
  case eCommand:
// TODO: check for tx command, load up parameter MSW
    paramMsgCom = P_GET_PARAM_COM(spiData);
    paramMsgIdx = P_GET_PARAM_IDX(spiData);
    break;
  case eDataH:
    // TODO: check for tx command, load up parameter LSW
    paramMsgDataL = spiData;
    break;
  case eDataL:
    paramMsgDataL = spiData;
    paramMsgData = P_GET_PARAM_DATA(paramMsgDataH, paramMsgDataL);
    if (paramMsgCom == P_PARAM_COM_SETI) {
      // TODO: standardize the bit depth of params (or just always use 32.)
      // for, now, testing with 10 bits
      set_param_int(paramMsgIdx, paramMsgData, 10);
    }
    if (paramMsgCom == P_PARAM_COM_SETF) {
      set_param_float(paramMsgIdx, paramMsgData);
    }
    break;
  default:
    // fucked
    break;
   }
  // increment the state machine... woe to thee if thou misseth an spi transaction.
  paramMsgState = (paramMsgState + 1) % eNumParamMsgStates; 

  // TEST: set LEDs to last 6 bits of spi data
  *pFlashA_PortB_Data &= (~spiData & 0x3F) ;
  *pFlashA_PortB_Data |= (spiData & 0x3F) ;

}
