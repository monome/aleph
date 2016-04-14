#ifndef _SER_H_
#define _SER_H_
#include "types.h"

void recv_char (char c);

char serialInputLineBuffer[1024];
char serialOutputLineBuffer[1024];
int serialInputLineBufferPos;
void handle_Serial (s32 data);
#endif // header guard
