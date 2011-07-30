// protocol

// P_PARAM from AVR to bf, param channel 
//
// A--- B------------- C--------------------------------------
// 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
//
// (A) COMMAND (4 bit) left aligned for extensibility
// (B) PARAM (12 bit)
// (C) VALUE (32 bit) int or float depending on COMMAND
//
// P_PARAM COMMANDS
#define P_PARAM_COM_SETI	0x80
#define P_PARAM_COM_SETF	0xC0
#define P_PARAM_COM_GETI	0x00
#define P_PARAM_COM_GETF	0x40

// to pack for delivery:
// word 1: (param >> 8) & command
// word 2: param >> 8
// word 3: param & 0xffff