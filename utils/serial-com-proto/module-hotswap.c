#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define START_FLAG 0x12
#define END_FLAG 0x13
#define DLE 0x7D

void proto_debug(char* c) {
  fprintf(stderr,c);
  fprintf(stderr,"\n");
}
FILE* ser;
void serial_putc(int c) {
  assert(putc(c, ser) != EOF);
}

int serial_read_pos = 0;

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


int main (int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "wrong args.  e.g:\n%s my-module.ldr my-module.dsc /dev/ttyACM0\n", argv[0]);
    return -1;
  }
  
  struct termios ser_termios;
  int ser_fd = open(argv[3], O_RDWR /* | O_NOCTTY */);
  if (ser_fd == -1) {
    fprintf(stderr, "failed to open %s\n", argv[3]);
    return -1;
  }
    
  ser = fdopen(ser_fd, "w");
  tcgetattr(ser_fd, &ser_termios);
  assert (cfsetspeed(&ser_termios, B115200) == 0);
  /* cfsetispeed(&ser_termios, B115200); */
  /* cfsetospeed(&ser_termios, B115200); */

  //Is this stuff needed?
  /* ser_termios.c_cflag &= PARENB; */
  /* ser_termios.c_cflag &= CSTOPB; */
  /* ser_termios.c_cflag &= CSIZE; */
  /* ser_termios.c_cflag |= CS8; */

  cfmakeraw(&ser_termios);
  tcsetattr(ser_fd, TCSANOW, &ser_termios);
  serial_putc(START_FLAG);
  
  serial_framedPutc(eSerialMsg_triggerParam);
  serial_framedPutc(0);
  serial_framedPutc(0);
  serial_framedPutc(0);
  serial_framedPutc(0);
  serial_putc(END_FLAG);
  fflush(ser);
  close(ser_fd);
}
