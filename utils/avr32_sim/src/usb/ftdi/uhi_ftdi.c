/*
  uhi_ftdi.c
  aleph-avr32
  
  usb host routines for ftdi driver
 */
// manufacturer string descriptor
//usb_str_desc_t manufacturer_desc;
char manufacturer_string[FTDI_STRING_MAX_LEN];
// product string descriptor
//usb_str_desc_t product_desc;
char product_string[FTDI_STRING_MAX_LEN];
// serial number string descriptor
//usb_str_desc_t serial_desc;
char serial_string[FTDI_STRING_MAX_LEN];

// control read-busy flag
static volatile u8 ctlReadBusy = 0;

//------- static funcs

// send control request
static u8 send_ctl_request(u8 reqtype, u8 reqnum, 
			   u8* data, u16 size,
			     u16 index, u16 val, 
			     uhd_callback_setup_end_t callbackEnd);

// control request end
static void ctl_req_end(
		usb_add_t add,
		uhd_trans_status_t status,
		uint16_t payload_trans);

//----- external (UHC) functions
uhc_enum_status_t uhi_ftdi_install(uhc_device_t* dev) {
    return UHC_ENUM_SUCCESS;
}

void uhi_ftdi_enable(uhc_device_t* dev) {
}

void uhi_ftdi_uninstall(uhc_device_t* dev) {
}

// run the input endpoint (bulk)
bool uhi_ftdi_in_run(uint8_t * buf, iram_size_t buf_size,
		     uhd_callback_trans_t callback) {
    return true;
}

// run the output endpoint (bulk)
bool uhi_ftdi_out_run(uint8_t * buf, iram_size_t buf_size,
		      uhd_callback_trans_t callback) {
    return true;
}

//----------------
//---- static functions definitions

// send control request
static u8 send_ctl_request(u8 reqtype, u8 reqnum, 
			   u8* data, u16 size,
			     u16 index, u16 val, 
			     uhd_callback_setup_end_t callbackEnd) {
  usb_setup_req_t req;
 
  /* if (uhi_ftdi_dev.dev != dev) { */
  /*   return;  // No interface to enable */
  /* } */

  req.bmRequestType = reqtype;
  req.bRequest = reqnum;
  req.wValue = (val);
  req.wIndex = (index);
  req.wLength = (size);
  return uhd_setup_request(uhi_ftdi_dev.dev->address,
		    &req,
		    data,
		    size,
		    NULL, //&ctl_req_run,
		    callbackEnd); //&ctl_req_end);		    
}

// to be called when control read is complete
static void ctl_req_end(
		usb_add_t add,
		uhd_trans_status_t status,
		uint16_t payload_trans) {
  // last transfer ok?
  //  print_dbg("\r\n ctl request end, status: ");
  //  print_dbg_hex((u32)status);
  ctlReadBusy = 0;
}

// read eeprom
void ftdi_get_strings(char** pManufacturer, char** pProduct, char** pSerial) {

  // get manufacturer string
  ctlReadBusy = 1;
  //  print_dbg("\r\n sending ctl request for manufacturer string, index : ");
  //  print_dbg_hex(uhi_ftdi_dev.dev->dev_desc.iManufacturer);
  if(!(send_ctl_request(
			/* req type*/
			FTDI_STRING_DESC_REQ_TYPE,
			/* req num */
			USB_REQ_GET_DESCRIPTOR, 
			/* data */
			(u8*)manufacturer_string,
			/* size */
			FTDI_STRING_MAX_LEN,
			//			0x2000,
			/*idx*/
			FTDI_STRING_DESC_LANGID,
			/*val*/
			// high byte is 3 for string descriptor (yay, magic!)
			(USB_DT_STRING << 8) | uhi_ftdi_dev.dev->dev_desc.iManufacturer,
			// end-transfer callback
			&ctl_req_end )
       
       )) {
    print_dbg("\r\n control request for string descriptor failed");
    return;
  }
  // wait for transfer end
  while(ctlReadBusy) { ;; } 

  // get product string
  ctlReadBusy = 1;
  //  print_dbg("\r\n sending ctl request for product string, index : ");
  //  print_dbg_ulong( uhi_ftdi_dev.dev->dev_desc.iProduct);
  if(!(send_ctl_request(
			/* req type*/
			FTDI_STRING_DESC_REQ_TYPE,
			/* req num */
			USB_REQ_GET_DESCRIPTOR, 
			/* data */
			(u8*)product_string,
			/* size */
			FTDI_STRING_MAX_LEN,
			/*idx*/
			FTDI_STRING_DESC_LANGID,
			/*val*/
			(USB_DT_STRING << 8) | uhi_ftdi_dev.dev->dev_desc.iProduct,
			// end-transfer callback
			&ctl_req_end )
       
       )) {
    print_dbg("\r\n control request for string descriptor failed");
    return;
  }
  // wait for transfer end
  while(ctlReadBusy) { ;; } 

  // get serial string
  ctlReadBusy = 1;
  // print_dbg("\r\n sending ctl request for serial string : ");
  if(!(send_ctl_request(
			/* req type*/
			FTDI_STRING_DESC_REQ_TYPE,
			/* req num */
			USB_REQ_GET_DESCRIPTOR, 
			/* data */
			(u8*)serial_string,
			/* size */
			FTDI_STRING_MAX_LEN,
			/*idx*/
			FTDI_STRING_DESC_LANGID,
			/*val*/
			(USB_DT_STRING << 8) | uhi_ftdi_dev.dev->dev_desc.iSerialNumber,
			// end-transfer callback
			&ctl_req_end )
       
       )) {
    print_dbg("\r\n control request for string descriptor failed");
    return;
  }
  // wait for transfer end
  while(ctlReadBusy) { ;; }
  //  print_dbg("\r\n requested all string descriptors.");
  *pManufacturer = manufacturer_string + FTDI_STRING_DESC_OFFSET;
  *pProduct = product_string + FTDI_STRING_DESC_OFFSET;
  *pSerial = serial_string + FTDI_STRING_DESC_OFFSET;
  
}
  
    /* for (i = 0; i < FTDI_EEPROM_SIZE__2; i++) { */
    /*     if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE, SIO_READ_EEPROM_REQUEST, 0, i, eeprom+(i*2), 2, ftdi->usb_read_timeout) != 2) */
    /*         ftdi_error_return(-1, "reading eeprom failed"); */
    /* } */

/*
// libusb syntax (from libftdi) :
        usb_control_msg(
	ftdi->usb_dev,           // device handle
	FTDI_DEVICE_IN_REQTYPE,  // request type
	SIO_READ_EEPROM_REQUEST, // request ID
	0,                       // value
	i,                       // index (in halfwords)
	eeprom+(i*2),            // data pointer
	2,                       // number of bytes
	ftdi->usb_read_timeout   // timeout


 */
