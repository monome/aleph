#include <stdio.h>
#include <string.h>

#define START_FLAG 0x12
#define END_FLAG 0x13
#define DLE 0x7D

#ifdef BEES

void serial_putc(char c) {
  usart_putchar(DEV_USART, c);
}
#else

void serial_putc(int c) {
  putc(c, stdout);
}

typedef short s16;
#endif

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
  serial_puts("monkey,badger,weasel");
  serial_endTx();
}

void serial_paramsDump () {
  serial_startTx ();
  serial_framedPutc(eSerialMsg_paramsDump);
  //Code goes here to dump all params to serial port
  serial_puts("ferret,marmoset,squirrel");
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
  switch (c[0]) {
  case eSerialMsg_dumpIns :
    serial_insDump ();
    break;
  case eSerialMsg_dumpParams :
    serial_paramsDump ();
    break;
  case eSerialMsg_triggerParam :
    if(len < 5)
      serial_debug ("triggerParam requires 16 bit bfin address & 16 bit data");
    else 
      serial_triggerParam(charsToS16(c[1],c[2]),
			  charsToS16(c[3],c[4]));
    break;
  case eSerialMsg_triggerIn :
    if(len < 5)
      serial_debug ("triggerIn requires 16 bit bees address & 16 bit data");
    else 
      serial_triggerIn(charsToS16(c[1],c[2]),
		 charsToS16(c[3],c[4]));
    break;
  case eSerialMsg_queryIn :
    if(len < 3)
      serial_debug ("queryIn requires 16 bit bees address");
    else 
      serial_inVal(charsToS16(c[1],c[2]));
    break;
  case eSerialMsg_queryParam :
    if(len < 3)
      serial_debug ("queryParam requires 16 bit bees address");
    else 
      serial_paramVal(charsToS16(c[1],c[2]));
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
  switch (serialState) {
  case eSerialState_waiting :
    if (c == START_FLAG)
      serialState = eSerialState_started;
    break;
  case eSerialState_started :
    if (c == END_FLAG) {
      processMessage(inBuf, msgPointer);
      serialState = eSerialState_waiting;
      msgPointer = 0;
    }
    else if (c == DLE) {
      serialState = eSerialState_escaping;
    }
    else {
      inBuf[msgPointer] = c;
      msgPointer++;
    }
    break;
  case eSerialState_escaping :
    inBuf[msgPointer] = c;
    break;
  }
}

#ifndef BEES
int main () {
  while(1) {
    recv_char(getchar());
  }
}
#endif
