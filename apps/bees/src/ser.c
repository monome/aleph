#include <string.h>
#include "print_funcs.h"

#include "usart.h"
#include "serial.h"
#include "memory.h"
#include "delay.h"

#include "bfin.h"
#include "events.h"
#include "event_types.h"
#include "net_protected.h"

#include "ser.h"
#include "op.h"
#include "preset.h"
#include "app.h"
#include "param.h"
#include "scene.h"

// Using serial framing as described in the following link:
// http://eli.thegreenplace.net/2009/08/12/framing-in-serial-communications/
#define START_FLAG 0x12
#define END_FLAG 0x13
#define DLE 0x7D


/////TEST let's put op_serial to bed now...

static u16 serial_read_pos = 0;
void serial_init(void) {
  serial_rx_flow_control = true;
  serial_delimiter = END_FLAG;
}

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

  //messages for patching bees
  eSerialMsg_dumpOutputs,
  eSerialMsg_outputsDump,
  eSerialMsg_dumpConnections,
  eSerialMsg_connectionsDump,
  eSerialMsg_connect,
  eSerialMsg_disconnect,
  eSerialMsg_dumpOps,
  eSerialMsg_opsDump,
  eSerialMsg_dumpOpDescriptions,
  eSerialMsg_opDescriptionsDump,
  eSerialMsg_newOp,
  eSerialMsg_deleteOp,

  //Messages for preset store/recall
  eSerialMsg_storePreset,
  eSerialMsg_recallPreset,

  //Messages for serial bfin prog
  eSerialMsg_bfinProgStart,
  eSerialMsg_bfinHexChunk,
  eSerialMsg_bfinDscChunk,
  eSerialMsg_bfinProgEnd,
  eSerialMsg_bfinProgEcho,
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
      serial_framedPutc(0);
    }
  }
  serial_endTx();
}

void serial_paramsDump () {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_paramsDump);
  //Code goes here to dump all params to serial port
  int i;
  for (i=0;i<net->numParams;i++) {
    serial_puts(net->params[i].desc.label);
    serial_framedPutc(0);
  }
  serial_endTx();
}

void serial_opDescriptionsDump () {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_opDescriptionsDump);
  int i;
  for (i=0; i < NUM_USER_OP_TYPES; i++) {
    serial_puts(op_registry[userOpTypes[i]].name);
    serial_framedPutc(0);
  }
  serial_endTx();
}
void serial_connectionsDump () {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_connectionsDump);
  int i;
  for(i=0; i < net->numOuts; i++) {
    int opIdx = net->outs[i].opIdx;
    int opOutIdx = net->outs[i].opOutIdx;
    int outDest = net->ops[opIdx]->out[opOutIdx];
    serial_framedPutc(hiByte(i));
    serial_framedPutc(loByte(i));
    serial_framedPutc(hiByte(outDest));
    serial_framedPutc(loByte(outDest));
  }
  serial_endTx();
}
void serial_outputsDump () {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_outputsDump);

  int i;
  for (i=0;i<net->numOps;i++) {
    int j;
    for (j=0; j<net->ops[i]->numOutputs; j++) {
      serial_puts(net->ops[i]->opString);
      serial_puts("/");
      serial_puts(net->ops[i]->outString + (j * 8));
      serial_framedPutc(0);
    }
  }
  serial_endTx();
}

void serial_opsDump () {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_opsDump);
  int i;
  for (i=0;i<net->numOps;i++) {
    int nIns = net->ops[i]->numInputs;
    int nOuts = net->ops[i]->numOutputs;
    serial_puts(net->ops[i]->opString);
    serial_framedPutc(0);
    serial_framedPutc(hiByte(nIns));
    serial_framedPutc(loByte(nIns));
    serial_framedPutc(hiByte(nOuts));
    serial_framedPutc(loByte(nOuts));
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

void serial_bfinProgEcho (volatile u8* buf, int len) {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_bfinProgEcho);
  int i;
  for (i=0; i < len; i++)
    serial_framedPutc(buf[i]);
  serial_endTx();
}

void serial_triggerParam (s16 idx, io_t data) {
  //param thwacking code goes here
  net_activate(idx+net->numIns, data, NULL);
}

void serial_triggerIn (s16 idx, io_t data) {
  //bees thwacking code goes here
  net_activate(idx, data, NULL);
}


void serial_inVal (s16 idx) {
  if (idx >= net->numIns || idx < 0) {
    /* serial_debug("Index out of range"); */
    return;
  }
  serial_startTx ();
  serial_framedPutc(eSerialMsg_inVal);
  serial_framedPutc(hiByte(idx));
  serial_framedPutc(loByte(idx));
  //beesIn reading code goes here
  inode_t* pIn = &(net->ins[idx]);
  io_t inVal = op_get_in_val(net->ops[pIn->opIdx],
			     pIn->opInIdx);
  serial_framedPutc(hiByte(inVal));
  serial_framedPutc(loByte(inVal));
  serial_endTx();
}

void serial_paramVal (s16 idx) {
  if (idx >= net->numParams || idx < 0) {
    /* serial_debug("Index out of range"); */
    return;
  }
  serial_startTx ();
  serial_framedPutc(eSerialMsg_paramVal);
  serial_framedPutc(hiByte(idx));
  serial_framedPutc(loByte(idx));
  //param reading code goes here
  s32 bfinPval = bfin_get_param(idx);
  io_t beesPval = scaler_get_in( &(net->params[idx].scaler), bfinPval);
  serial_framedPutc(hiByte(beesPval));
  serial_framedPutc(loByte(beesPval));
  serial_endTx();
}

void serial_disconnect (s16 idx) {
  net_disconnect(idx);
}

void serial_deleteOp (s16 idx) {
  net_pop_op();
  serial_debug("deleting last op - arbitrary op deletion not yet supported...");
}

void serial_newOp (s16 idx) {
  net_add_op(userOpTypes[idx]);
}

void serial_connect (s16 outIdx, s16 inIdx) {
  net_connect(outIdx, inIdx);
}

void serial_storePreset (s16 idx) {
  preset_store(idx);
}
void serial_recallPreset (s16 idx) {
  preset_recall(idx);
}

s16 charsToS16 (char hi, char lo) {
  s16 ret = hi;
  ret = ret << 8;
  ret += lo;
  return ret;
}

volatile u8* serial_bfinHexBuf = NULL;
volatile u8* serial_bfinDscBuf = NULL;
int serial_bfinHexBuf_idx;
int serial_bfinDscBuf_idx;
// 256kb max module size
#define MAX_SERIAL_HEX_SIZE (256 * 1024)
// 256kb max module description size
#define MAX_SERIAL_DSC_SIZE (256 * 1024)

void serial_bfinProgStart() {
  pause_timers();
  if(serial_bfinHexBuf == NULL)
    serial_bfinHexBuf = alloc_mem(MAX_SERIAL_HEX_SIZE);
  serial_bfinHexBuf_idx = 0;

  if(serial_bfinDscBuf == NULL)
    serial_bfinDscBuf = alloc_mem(MAX_SERIAL_DSC_SIZE);
  serial_bfinDscBuf_idx = 0;
}
void serial_bfinHexChunk(char* c, int len) {
  if (serial_bfinHexBuf_idx + len - 1 >= MAX_SERIAL_HEX_SIZE) {
    serial_debug("bfin hex buffer full - 256kb is max size for bfin prog!");
    return;
  }
  int i;
  for (i = 1; i < len; i++) {
    serial_bfinHexBuf[serial_bfinHexBuf_idx + i - 1] = c[i];
  }
  serial_bfinHexBuf_idx += len - 1;
}
void serial_bfinDscChunk(char* c, int len) {
  if (serial_bfinDscBuf_idx + len - 1 >= MAX_SERIAL_DSC_SIZE) {
    serial_debug("bfin dsc buffer full - 256kb is max size for Dsc file!");
    return;
  }
  int i;
  for (i = 1; i < len; i++) {
    serial_bfinDscBuf[serial_bfinDscBuf_idx + i - 1] = c[i];
  }
  serial_bfinDscBuf_idx += len - 1;
}
void serial_bfinProgEnd() {
  app_pause();
  net_disconnect_params();
  delay_ms(2);

  bfin_load_buf((const u8*) serial_bfinHexBuf, serial_bfinHexBuf_idx);
  scene_set_module_name("serialdump");

  buf_load_desc((u8*) serial_bfinDscBuf);

  // DEBUG uncomment this to send the loaded module back
  // over serial connection
  /* serial_bfinProgEcho(serial_bfinHexBuf, serial_bfinHexBuf_idx); */
  /* serial_bfinProgEcho(serial_bfinDscBuf, serial_bfinDscBuf_idx); */
  free_mem(serial_bfinDscBuf);
  free_mem(serial_bfinHexBuf);
  serial_bfinDscBuf = NULL;
  serial_bfinHexBuf = NULL;
  bfin_wait_ready();
  bfin_enable();
  app_resume();

  start_timers();
}


void processMessage (char* c, int len) {
  /* proto_debug("actually got some message"); */
  switch (c[0]) {
  case eSerialMsg_dumpIns :
    /* serial_debug("dumping inputs..."); */
    serial_insDump ();
    break;
  case eSerialMsg_dumpParams :
    /* serial_debug("dumping params..."); */
    serial_paramsDump ();
    break;
  case eSerialMsg_dumpOps :
    /* serial_debug("dumping ops..."); */
    serial_opsDump ();
    break;
  case eSerialMsg_dumpOutputs :
    /* serial_debug("dumping outputs..."); */
    serial_outputsDump ();
    break;
  case eSerialMsg_dumpConnections :
    /* serial_debug("dumping connections..."); */
    serial_connectionsDump ();
    break;
  case eSerialMsg_dumpOpDescriptions :
    /* serial_debug("dumping opDescriptions..."); */
    serial_opDescriptionsDump ();
    break;
  case eSerialMsg_triggerParam :
    if(len < 5)
      serial_debug ("triggerParam requires 16 bit bfin address & 16 bit data");
    else  {
      serial_triggerParam(charsToS16(c[1],c[2]),
			  charsToS16(c[3],c[4]));
      /* serial_debug("triggered param"); */
    }
    break;
  case eSerialMsg_triggerIn :
    if(len < 5)
      serial_debug ("triggerIn requires 16 bit bees address & 16 bit data");
    else {
      serial_triggerIn(charsToS16(c[1],c[2]),
		       charsToS16(c[3],c[4]));
      /* serial_debug("triggered in"); */
    }
    break;
  case eSerialMsg_queryIn :
    if(len < 3)
      serial_debug ("queryIn requires 16 bit bees address");
    else {
      serial_inVal(charsToS16(c[1],c[2]));
      /* serial_debug("queried ins"); */
    }
    break;
  case eSerialMsg_queryParam :
    if(len < 3)
      serial_debug ("queryParam requires 16 bit bees address");
    else {
      serial_paramVal(charsToS16(c[1],c[2]));
      /* serial_debug("queried params"); */
    }
    break;
  case eSerialMsg_disconnect :
    if(len < 3)
      serial_debug ("disconnect requires 16 bit bees address");
    else {
      serial_disconnect(charsToS16(c[1],c[2]));
      /* serial_debug("broke a connection"); */
    }
    break;
  case eSerialMsg_newOp :
    if(len < 3)
      serial_debug ("newOp requires 16 bit bees address");
    else {
      serial_newOp(charsToS16(c[1],c[2]));
      /* serial_debug("added an op"); */
    }
    break;
  case eSerialMsg_deleteOp :
    if(len < 3)
      serial_debug ("deleteOp requires 16 bit bees address");
    else {
      serial_deleteOp(charsToS16(c[1],c[2]));
      /* serial_debug("deleted an op"); */
    }
    break;
  case eSerialMsg_connect :
    if(len < 5)
      serial_debug ("connect requires 2 x 16 bit bees address");
    serial_connect(charsToS16(c[1],c[2]), charsToS16(c[3],c[4]));
    /* serial_debug("made a connection"); */
    break;
  case eSerialMsg_storePreset :
    if(len < 3)
      serial_debug ("storePreset requires 16 bit bees address");
    else {
      serial_storePreset(charsToS16(c[1],c[2]));
      /* serial_debug("stored a preset"); */
    }
    break;
  case eSerialMsg_recallPreset :
    if(len < 3)
      serial_debug ("recallPreset requires 16 bit bees address");
    else {
      serial_recallPreset(charsToS16(c[1],c[2]));
      /* serial_debug("recalled a preset"); */
    }
    break;
  case eSerialMsg_bfinProgStart :
    serial_bfinProgStart();
    break;
  case eSerialMsg_bfinHexChunk :
    serial_bfinHexChunk(c, len);
    break;
  case eSerialMsg_bfinDscChunk :
    serial_bfinDscChunk(c, len);
    break;
  case eSerialMsg_bfinProgEnd :
    serial_bfinProgEnd();
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


char chunks_per_message = 0;

void recv_char (char c) {
  if (msgPointer >= MSG_MAX) {
    serialState = eSerialState_waiting;
    msgPointer = 0;
    /* proto_debug("resetting overflowing msgPointer"); */

  }
  switch (serialState) {
  case eSerialState_waiting :
    if (c == START_FLAG) {
      serialState = eSerialState_started;
      /* proto_debug("resetting serialState"); */
    }
    break;
  case eSerialState_started :
    if (c == END_FLAG) {
      print_dbg("\r\n chunks per message: ");
      print_dbg_ulong(chunks_per_message);
      
      chunks_per_message = 0;
      processMessage(inBuf, msgPointer);
      serialState = eSerialState_waiting;
      msgPointer = 0;
      /* proto_debug("actually received a message"); */
    }
    else if (c == DLE) {
      /* proto_debug("received an escape char"); */
      serialState = eSerialState_escaping;
    }
    else {
      /* proto_debug("writing to inBuf"); */
      inBuf[msgPointer] = c;
      msgPointer++;
    }
    break;
  case eSerialState_escaping :
    /* proto_debug("writing escaped char to inBuf"); */
    inBuf[msgPointer] = c;
    msgPointer++;
    serialState = eSerialState_started;
    break;
  default :
    /* proto_debug("undefined serial state -whaaaaat!?"); */
    serialState = eSerialState_waiting;
    break;
  }
}
void serial_process(s32 data) {
  // process_serial_t serial_decode = &serial_decode_dummy;
  u16 c = (u16)data;
  chunks_per_message++;

  while(serial_read_pos != c) {
    //////////////////
    //// TEST: loopback
    /* print_dbg_char(serial_buffer[serial_read_pos]); */

    recv_char (serial_buffer[serial_read_pos]);
    serial_read_pos++;
    if(serial_read_pos == SERIAL_BUFFER_SIZE) serial_read_pos = 0;
  }
}

