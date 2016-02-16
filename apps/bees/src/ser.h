#include "types.h"
#ifndef _SER_H_
#define _SER_H_
#include "op.h"
#include "op_math.h"
#include "types.h"
#include "ser.h"

void serial_process(s32 data);

//--- op_serial_t : comparisons
typedef struct op_serial_struct {
  op_t super;

  volatile io_t a;
  volatile io_t b;
  volatile io_t* in_val[2];

  op_out_t outs[];
} op_serial_t;

void serial_init(void);
void op_serial_out (op_serial_t* op, io_t v);
void serial_putc(char c);
void serial_puts(const char *str);
void serial_framedPutc (char x);
void serial_debug(const char *str);
void proto_debug(const char *c);
void serial_startTx(void);
void serial_endTx(void);
char hiByte (int x);
char loByte (int x);
void serial_insDump (void);
void serial_paramsDump (void);
void serial_outVal (int addr, int data);
void serial_bfinProgEcho (volatile u8* buf, int len);
void serial_triggerParam (s16 idx, s16 data);
void serial_triggerIn (s16 idx, s16 data);
void serial_inVal (s16 idx);
void serial_paramVal (s16 idx);
void serial_storePreset (s16 idx);
void serial_recallPreset (s16 idx);
s16 charsToS16 (char hi, char lo);
void serial_bfinProgStart(void);
void serial_bfinHexChunk(char* c, int len);
void serial_bfinDscChunk(char* c, int len);
void serial_bfinProgEnd(void);

void processMessage (char* c, int len);
void recv_char (char c);

void serial_disconnect (s16 idx);
void serial_deleteOp (s16 idx);
void serial_newOp (s16 idx);
void serial_connect (s16 outIdx, s16 inIdx);
void serial_opDescriptionsDump (void);
void serial_connectionsDump (void);
void serial_outputsDump (void);
void serial_opsDump (void);
// typedef void(*process_serial_t)(u16);

// extern void serial_param_num(s32 data);
// extern void serial_param_info(s32 data);
// extern void serial_param_get(s32 data);
// extern void serial_param_set(s32 data);


// incoming serial:

// debug(int state)
// req_num_params(void);
// req_param_info(u8 index)
// set_param_val(u8 index, u32 val)
// get param_val(u8 index)

// outgoing serial:
// debug
// report_num_params(u8 num)
// report_param_info(u8 index, "name")
// report_param_val(u8 index, u32 val)
#endif // header guard
