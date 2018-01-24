// aleph/common
#include "param_common.h"

// bfin_lib
#include "bfin_core.h"
#include "control.h"
#include "gpio.h"
#include "module.h"
#include "protocol.h"
#include "types.h"
#include "util.h"
#include "spi.h"

//----- static variables

// byte to process
static eSpiByte byte = eCom;
// current command
static u8 com;
// current param index
static u8 idx;

//------ static functions
static void spi_set_param(u32 idx, ParamValue pv) {
  gModuleData->paramData[idx].value = pv;
  module_set_param(idx, pv);
}

//------- function definitions
// deal with new data in the spi rx ringbuffer
// return byte to load for next MISO
u8 spi_process(u8 rx) {
  static ParamValueSwap pval;
  switch(byte) {
    /// caveman style case statement
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
      return gModuleData->numParams; // load num params
      break;
    case MSG_GET_MODULE_NAME_COM:
      byte = eModuleName0;
      return gModuleData->name[0];
      break;
    case MSG_GET_MODULE_VERSION_COM:
      byte = eModuleVersionMaj;
      return MAJ;
      break;
    case MSG_ENABLE_AUDIO:
      processAudio = 1;
      return processAudio;
      break;
    case MSG_DISABLE_AUDIO:
      processAudio = 0;
      return processAudio;
      break;
        
        case MSG_OFFSET_COM :
            byte = eOffset0;
            break;

        case MSG_SAMPLE_COM :
            byte = eSample0;
            break;
            
    default:
      break;
    }
    return 0;
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
    pval.asByte[3] = rx; // set paramval
    return 0; // don't care
    break;
  case eSetParamData1 :
    byte = eSetParamData2;
    // byte-swap from BE on avr32
    pval.asByte[2] = rx; // set paramval
    return 0; // don't care
    break;
  case eSetParamData2 :
    byte = eSetParamData3;
    // byte-swap from BE on avr32
    pval.asByte[1] = rx; // set paramval
    return 0; // don't care
    break;
  case eSetParamData3 :
    // byte-swap from BE on avr32
    pval.asByte[0] = rx; // set paramval
    spi_set_param(idx, pval.asInt);
    byte = eCom; //reset
    return 0; // don't care
    break;


    //---- get param
  case eGetParamIdx :
    idx = rx; // set index
    byte = eGetParamData0;
    pval.asInt = gModuleData->paramData[idx].value;
    // byte-swap from BE on avr32
    return pval.asByte[3];
    break;

  case eGetParamData0 :
    byte = eGetParamData1;
    // byte-swap from BE on avr32
    return pval.asByte[2];
    break;
  case eGetParamData1 :
    byte = eGetParamData2;
    // byte-swap from BE on avr32
    return pval.asByte[1];
    break;
  case eGetParamData2 :
    byte = eGetParamData3;
    // byte-swap from BE on avr32
    return pval.asByte[0];
    break;
  case eGetParamData3 :
    byte = eCom; //reset
    return 0; // don't care
    break;

    //---- get num params
  case eNumParamsVal :
    byte = eCom; //reset
    return 0; // don't care 
    break;



    //----- get module name
  case eModuleName0 :
    byte = eModuleName1;
    return gModuleData->name[1];
    break;
  case eModuleName1 :
    byte = eModuleName2;
    return gModuleData->name[2];
    break;
  case eModuleName2 :
    byte = eModuleName3;
    return gModuleData->name[3];
    break;
  case eModuleName3 :
    byte = eModuleName4;
    return gModuleData->name[4];
    break;
  case eModuleName4 :
    byte = eModuleName5;
    return gModuleData->name[5];
    break;
  case eModuleName5 :
    byte = eModuleName6;
    return gModuleData->name[6];
    break;
  case eModuleName6 :
    byte = eModuleName7;
    return gModuleData->name[7];
    break;
  case eModuleName7 :
    byte = eModuleName8;
    return gModuleData->name[8];
    break;
  case eModuleName8 :
    byte = eModuleName9;
    return gModuleData->name[9];
    break;
  case eModuleName9 :
    byte = eModuleName10;
    return gModuleData->name[10];
    break;
  case eModuleName10 :
    byte = eModuleName11;
    return gModuleData->name[11];
    break;
  case eModuleName11 :
    byte = eModuleName12;
    return gModuleData->name[12];
    break;
  case eModuleName12 :
    byte = eModuleName13;
    return gModuleData->name[13];
    break;
  case eModuleName13 :
    byte = eModuleName14;
    return gModuleData->name[14];
    break;
  case eModuleName14 :
    byte = eModuleName15;
    return gModuleData->name[15];
    break;
  case eModuleName15 :
    byte = eModuleName16;
    return gModuleData->name[16];
    break;
  case eModuleName16 :
    byte = eModuleName17;
    return gModuleData->name[17];
    break;
  case eModuleName17 :
    byte = eModuleName18;
    return gModuleData->name[18];
    break;
  case eModuleName18 :
    byte = eModuleName19;
    return gModuleData->name[19];
    break;
  case eModuleName19 :
    byte = eModuleName20;
    return gModuleData->name[20];
    break;
  case eModuleName20 :
    byte = eModuleName21;
    return gModuleData->name[21];
    break;
  case eModuleName21 :
    byte = eModuleName22;
    return gModuleData->name[22];
    break;
  case eModuleName22 :
    byte = eModuleName23;
    return gModuleData->name[23];
    break;
  case eModuleName23 :
    byte = eCom; // reset
    return 0;    // don't care
    break;

    /// version
  case eModuleVersionMaj :
    byte = eModuleVersionMin;
    return MIN; 
    break;

  case eModuleVersionMin :
    byte = eModuleVersionRev0;
    // patch (u16)
    // === byteswap for BE on avr32
    return REV >> 8;  
    break;

  case eModuleVersionRev0 :
    byte = eModuleVersionRev1;
    return REV & 0x00ff;
    break;

  case eModuleVersionRev1 :
    byte = eCom; // reset
    return 0;    // don't care
    break;

      
      //  set offset
        case eOffset0 :
            byte = eOffset1;
            o.asByte[3] = rx;
            return 0;
            break;
        case eOffset1 :
            byte = eOffset2;
            o.asByte[2] = rx;
            return 0;
            break;
        case eOffset2 :
            byte = eOffset3;
            o.asByte[1] = rx;
            return 0;
            break;
        case eOffset3 :
            o.asByte[0] = rx;
            module_set_offset(o.asInt);
            byte = eCom;
            return 0;
            break;

        //  set sample
        case eSample0 :
            byte = eSample1;
            s.asByte[3] = rx;
            return 0;
            break;
        case eSample1 :
            byte = eSample2;
            s.asByte[2] = rx;
            return 0;
            break;
        case eSample2 :
            byte = eSample3;
            s.asByte[1] = rx;
            return 0;
            break;
        case eSample3 :
            s.asByte[0] = rx;
            module_set_sample(s.asInt);
            byte = eCom;
            return 0;
            break;
      
      
  default:
    byte = eCom; // reset
    return 0;
    break;
  }
}
