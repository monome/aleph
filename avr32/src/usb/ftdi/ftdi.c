/*
  ftdi.c
  aleph-avr32

  FTDI usb functions.
*/

// asf
#include "delay.h"
#include "print_funcs.h"
// aleph
#include "conf_usb_host.h"
#include "events.h"
#include "event_types.h"
#include "ftdi.h"
#include "global.h"
#include "monome.h"
#include "uhi_ftdi.h"

//---- defines


//---- global vars (initialized here) 
u8 ftdiPlug = 0;

//---- extern vars
u8 ftdiRxBuf[FTDI_IN_BUF_SIZE];

//----- static vars
//static u8 ftdi_rx_buf[FTDI_IN_BUF_SIZE];
//static u8 ftdi_tx_buf[FTDI_OUT_BUF_SIZE];
static volatile  uhd_trans_status_t status = 0;
static volatile u32 rxBytes = 0;
static volatile u8 rxBusy = 0;
static volatile u8 txBusy = 0;

//------- static functions

// debug print
//static void print_arr(u32 addr, u32 size);
//static void print_unicode_string(char* str, u32 len);

// usb bulk transfer callback
static void ftdi_transfer_done(
			       usb_add_t add,
			       usb_ep_t ep,
			       uhd_trans_status_t stat,
			       iram_size_t nb_transfered) {
  //  print_dbg("\r\n ftdi transfer done; status: ");
  //  print_dbg_hex((u32)status);
  //  print_dbg(" ; bytes transferred : ");
  //  print_dbg_ulong((u32)nb_transfered);

  status = stat;
  if (ep & USB_EP_DIR_IN) { 
    rxBusy = 0;
    rxBytes = nb_transfered;
  } else {
    txBusy = 0;
  }
}

//-------- extern functions
void ftdi_write(u8* data, u32 bytes) {
  //  int i;
  //  u8 val;
  txBusy = 1;

  /* //// testing */
  /* if(data > 0) { */
  /*   val = 145; */
  /* } else { */
  /*   val = 144; */
  /* } */


  //  for(i=0; i<FTDI_OUT_BUF_SIZE; i++) {
  //    ftdi_out_buf[i] = val;
  //  }
  ///////
  /*
    if(!uhi_ftdi_out_run(ftdi_out_buf,
    FTDI_OUT_BUF_SIZE,
    &ftdi_transfer_done)) {
  */
  if(!uhi_ftdi_out_run(data,
		       bytes,
		       &ftdi_transfer_done)) {

    print_dbg("\r\n error requesting ftdi output pipe");
    return;
  }
  while (txBusy) { ;; }
  print_dbg("\r\n finished ftdi bulk output transfer");
  
  if (status != UHD_TRANS_NOERROR) {
    print_dbg("\r\n ftdi tx error");
    return;
  }
}
    
u32 ftdi_read(void) {
  rxBytes = 0;
  rxBusy = true;
  if (!uhi_ftdi_in_run(ftdiRxBuf,
		       FTDI_IN_BUF_SIZE, &ftdi_transfer_done)) {
    print_dbg("\r\n uhi_ftdi_in_run returned false; aborting input transfer");
    return 0;
  }
  while (rxBusy) { ;; }
  if (status != UHD_TRANS_NOERROR) {
    print_dbg("\r\n ftdi input transfer error, exiting poll function");
    return 0;
  }
  //    print_arr((u32)ftdi_in_buf, FTDI_IN_BUF_SIZE);
  return rxBytes;
}


// respond to connection or disconnection of ftdi device.
// may be called from an interrupt
void ftdi_change(uhc_device_t* dev, u8 plug) {
  event_t e;
  print_dbg("\r\n changed FTDI connection status");
  if(plug) { 
    e.eventType = kEventFtdiConnect; 
  } else {
    e.eventType = kEventFtdiDisconnect;
  }
  // posting an event so the main loop can respond
  post_event(&e); 
}

// setup new device connection
void ftdi_setup(void) {
  char * manstr;
  char * prodstr;
  char * serstr;
  print_dbg("\r\n FTDI setup routine");

  // get string data...
  ftdi_get_strings(&manstr, &prodstr, &serstr);
  
  // print the strings
  // print_unicode_string(manstr, FTDI_STRING_MAX_LEN);
  //  print_unicode_string(prodstr, FTDI_STRING_MAX_LEN);
  //  print_unicode_string(serstr, FTDI_STRING_MAX_LEN);

  check_monome_device(manstr, prodstr, serstr);

  // set connection flag
  ftdiPlug = 1;
}
