#include <string.h>
#include "print_funcs.h"

#include "usart.h"
#include "serial.h"

#include "bfin.h"
#include "events.h"
#include "event_types.h"
#include "net_protected.h"

#include "ser.h"

#define START_FLAG 0x12
#define END_FLAG 0x13
#define DLE 0x7D


/////TEST let's put op_serial to bed now...
extern op_serial_t* last_serial_op = NULL;

static u16 serial_read_pos = 0;

void serial_putc(char c) {
  usart_putchar(DEV_USART, c);
}
void proto_debug(const char* c) {
  /* print_dbg(c); */
}

void serial_startTx () {
  serial_putc(START_FLAG);
}

void serial_endTx () {
  serial_putc(END_FLAG);
}

void serial_framedPutc (char x) {
  if (x == DLE || x == START_FLAG || x == END_FLAG)
    serial_putc(DLE);
  serial_putc(x);
}

void serial_puts(const char *str) {
  int i;
  for(i=0;i<strlen(str);i++) {
    serial_framedPutc(str[i]);
  }
}


enum serialMsgTypes {
  eSerialMsg_debug,
  eSerialMsg_dumpIns,
  eSerialMsg_insDump,
  eSerialMsg_dumpParams,
  eSerialMsg_paramsDump,
  eSerialMsg_triggerParam,
  eSerialMsg_triggerIn,
  eSerialMsg_queryIn,
  eSerialMsg_inVal,
  eSerialMsg_queryParam,
  eSerialMsg_paramVal,
  eSerialMsg_outVal,
  eSerialMsg_numParams
};

void serial_debug(const char *str) {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_debug);
  serial_puts(str);
  serial_endTx();
}

void serial_insDump () {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_insDump);
  //Code goes here to dump all ins to serial port
  int i;
  for (i=0;i<net->numOps;i++) {
    int j;
    for (j=0; j<net->ops[i]->numInputs; j++) {
      serial_puts(net->ops[i]->opString);
      serial_puts("/");
      serial_puts(net->ops[i]->inString + (j * 8));
      serial_puts(",");
    }
  }
  serial_endTx();
}

void serial_paramsDump () {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_paramsDump);
  //Code goes here to dump all params to serial port
  /* serial_puts("ferret,marmoset,squirrel"); */
  int i;
  for (i=0;i<net->numParams;i++) {
    serial_puts(net->params[i].desc.label);
    serial_puts(",");
  }
  serial_endTx();
}

char hiByte (int x) {
  return x >> 8;
}

char loByte (int x) {
  return x & 0x00FF;
}

void serial_outVal (int addr, int data) {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_outVal);
  serial_framedPutc(hiByte(addr));
  serial_framedPutc(loByte(addr));
  serial_framedPutc(hiByte(data));
  serial_framedPutc(loByte(data));
  serial_endTx();
}

void serial_triggerParam (s16 idx, s16 data) {
  //param thwacking code goes here
}

void serial_triggerIn (s16 idx, s16 data) {
  //bees thwacking code goes here
}


void serial_inVal (s16 idx) {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_inVal);
  serial_framedPutc(hiByte(idx));
  serial_framedPutc(loByte(idx));
  //beesIn reading code goes here
  serial_framedPutc(hiByte(43));
  serial_framedPutc(loByte(43));
  serial_endTx();
}

void serial_paramVal (s16 idx) {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_paramVal);
  serial_framedPutc(hiByte(idx));
  serial_framedPutc(loByte(idx));
  //param reading code goes here
  serial_framedPutc(hiByte(43));
  serial_framedPutc(loByte(43));
  serial_endTx();
}


s16 charsToS16 (char hi, char lo) {
  s16 ret = hi;
  ret = ret << 8;
  ret += lo;
  return ret;
}

void processMessage (char* c, int len) {
  proto_debug("actually got some message");
  switch (c[0]) {
  case eSerialMsg_dumpIns :
    serial_debug("dumping inputs...");
    serial_insDump ();
    break;
  case eSerialMsg_dumpParams :
    serial_debug("dumping params...");
    serial_paramsDump ();
    break;
  case eSerialMsg_triggerParam :
    if(len < 5)
      serial_debug ("triggerParam requires 16 bit bfin address & 16 bit data");
    else  {
      serial_triggerParam(charsToS16(c[1],c[2]),
			  charsToS16(c[3],c[4]));
      serial_debug("triggered param");
    }
    break;
  case eSerialMsg_triggerIn :
    if(len < 5)
      serial_debug ("triggerIn requires 16 bit bees address & 16 bit data");
    else {
      serial_triggerIn(charsToS16(c[1],c[2]),
		       charsToS16(c[3],c[4]));
      serial_debug("triggered in");
    }
    break;
  case eSerialMsg_queryIn :
    if(len < 3)
      serial_debug ("queryIn requires 16 bit bees address");
    else {
      serial_inVal(charsToS16(c[1],c[2]));
      serial_debug("queried ins");
    }
    break;
  case eSerialMsg_queryParam :
    if(len < 3)
      serial_debug ("queryParam requires 16 bit bees address");
    else {
      serial_paramVal(charsToS16(c[1],c[2]));
      serial_debug("queried params");
    }
    break;
  default :
    serial_debug ("Unknown serial command issued to bees");
  }
}

enum serialRecvStates {
  eSerialState_waiting,
  eSerialState_escaping,
  eSerialState_started
};

#define MSG_MAX 4096
int serialState = eSerialState_waiting;
int msgPointer = 0;
char inBuf[MSG_MAX];


void recv_char (char c) {
  if (msgPointer > MSG_MAX) {
    serialState = eSerialState_waiting;
    msgPointer = 0;
    proto_debug("resetting overflowing msgPointer");

  }
  switch (serialState) {
  case eSerialState_waiting :
    if (c == START_FLAG) {
      serialState = eSerialState_started;
      proto_debug("resetting serialState");
    }
    break;
  case eSerialState_started :
    if (c == END_FLAG) {
      processMessage(inBuf, msgPointer);
      serialState = eSerialState_waiting;
      msgPointer = 0;
      proto_debug("actually received a message");
    }
    else if (c == DLE) {
      proto_debug("received an escape char");
      serialState = eSerialState_escaping;
    }
    else {
      proto_debug("writing to inBuf");
      inBuf[msgPointer] = c;
      msgPointer++;
    }
    break;
  case eSerialState_escaping :
    proto_debug("writing escaped char to inBuf");
    inBuf[msgPointer] = c;
    msgPointer++;
    serialState = eSerialState_started;
    break;
  default :
    proto_debug("undefined serial state -whaaaaat!?");
    serialState = eSerialState_waiting;
    break;
  }
}


void serial_process(s32 data) {
  // process_serial_t serial_decode = &serial_decode_dummy;
  u16 c = (u16)data;

  while(serial_read_pos != c) {
    //////////////////
    //// TEST: loopback
    /* print_dbg_char(serial_buffer[serial_read_pos]); */

    //// TEST: skanky hacked-to-bits op_serial 
    /* if (last_serial_op != NULL) { */
    /*   op_serial_out(last_serial_op, serial_buffer[serial_read_pos]); */
    /* } */

    recv_char (serial_buffer[serial_read_pos]);
    serial_read_pos++;
    if(serial_read_pos == SERIAL_BUFFER_SIZE) serial_read_pos = 0;
  }
}

