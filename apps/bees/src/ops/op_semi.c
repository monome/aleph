#include "pickle.h"
#include "op_semi.h"
#include "net.h"

//-------------------------------------------------
//----- descriptor
static const char* op_semi_instring =  "NOTE\0   ";
static const char* op_semi_outstring = "HZ\0     TUNE\0   CV\0     ";
static const char* op_semi_opstring =  "SEMI";

//-------------------------------------------------
//----- static function declaration


// set inputs
static void op_semi_in_note(op_semi_t* semi, const io_t v);
// pickle / unpickle
static u8* op_semi_pickle(op_semi_t* semi, u8* dst);
static u8* op_semi_unpickle(op_semi_t* semi, const u8* src);

// array of input functions 
static op_in_fn op_semi_in[1] = {
  (op_in_fn)&op_semi_in_note,
};

// common lisp code to generate lookup tables
/* (let ((freqs (loop for i below 128 */
/* 			 collect (* (expt 2.0 (/ i 12)) */
/* 				    8.1757989156 )))) */
/* 	   (format t "static const io_t Hz[128] = {~{~a~^,~^ ~}}~%;" (mapcar #'round freqs)) */
/* 	   (format t "static const io_t tune[128] = {~{~a~^,~^ ~}}~%;" (mapcar (lambda (freq) */
/* 						  (round (* (expt 2 (- 15 2)) */
/* 							    (/ freq (round freq))))) */
/* 						freqs)) */
/* 	   (format t "static const io_t CV[128] = {~{~a~^,~^ ~}}~%;" */
/* 		   (loop for i below 128 */
/* 			 collect (round (/ (* i (/ 4096 10)) */
/* 					   12))))) */
static const io_t Hz[128] = {8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};
static const io_t tune[128] = {8372, 7884, 8353, 7965, 8438, 8127, 7893, 8363, 8178, 8046, 7956, 8429, 8372, 8348, 8353, 8384, 8037, 8127, 8236, 8363, 8178, 8046, 8230, 8157, 8118, 8110, 8127, 8169, 8233, 8127, 8236, 8192, 8178, 8192, 8230, 8157, 8243, 8227, 8239, 8169, 8233, 8221, 8236, 8192, 8178, 8192, 8160, 8223, 8180, 8168, 8183, 8169, 8183, 8174, 8192, 8192, 8178, 8192, 8195, 8190, 8180, 8197, 8183, 8195, 8183, 8197, 8192, 8192, 8198, 8192, 8195, 8190, 8196, 8197, 8197, 8195, 8195, 8197, 8192, 8192, 8188, 8192, 8195, 8190, 8188, 8190, 8190, 8189, 8189, 8191, 8192, 8192, 8193, 8192, 8190, 8190, 8192, 8194, 8193, 8192, 8192, 8191, 8192, 8192, 8193, 8192, 8193, 8192, 8192, 8192, 8191, 8192, 8192, 8191, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192, 8192};
static const io_t CV[128] = {0, 34, 68, 102, 137, 171, 205, 239, 273, 307, 341, 375, 410, 444, 478, 512, 546, 580, 614, 649, 683, 717, 751, 785, 819, 853, 887, 922, 956, 990, 1024, 1058, 1092, 1126, 1161, 1195, 1229, 1263, 1297, 1331, 1365, 1399, 1434, 1468, 1502, 1536, 1570, 1604, 1638, 1673, 1707, 1741, 1775, 1809, 1843, 1877, 1911, 1946, 1980, 2014, 2048, 2082, 2116, 2150, 2185, 2219, 2253, 2287, 2321, 2355, 2389, 2423, 2458, 2492, 2526, 2560, 2594, 2628, 2662, 2697, 2731, 2765, 2799, 2833, 2867, 2901, 2935, 2970, 3004, 3038, 3072, 3106, 3140, 3174, 3209, 3243, 3277, 3311, 3345, 3379, 3413, 3447, 3482, 3516, 3550, 3584, 3618, 3652, 3686, 3721, 3755, 3789, 3823, 3857, 3891, 3925, 3959, 3994, 4028, 4062, 4096, 4130, 4164, 4198, 4233, 4267, 4301, 4335};


/// initialize
void op_semi_init(void* op) {
  op_semi_t* semi = (op_semi_t*)op;

  // superclass functions
  semi->super.in_fn = op_semi_in;
  semi->super.pickle = (op_pickle_fn) (&op_semi_pickle);
  semi->super.unpickle = (op_unpickle_fn) (&op_semi_unpickle);
  
  // superclass state
  semi->super.numInputs = 1;
  semi->super.numOutputs = 3;
  semi->outs[0] = -1;
  semi->outs[1] = -1;
  semi->outs[2] = -1;
 
  semi->super.in_val = semi->in_val;
  semi->in_val[0] = &(semi->note);
  semi->note = 0;

  semi->super.out = semi->outs;
  semi->super.opString = op_semi_opstring;
  semi->super.inString = op_semi_instring;
  semi->super.outString = op_semi_outstring;
  semi->super.type = eOpSemi;

}

//-------------------------------------------------
//----- static function definition

//===== operator input

// input state
static void op_semi_in_note(op_semi_t* semi, const io_t v) {
  if (v < 0) {
    semi->note = 0;
  }
  else if (v >= 128) {
    semi->note = 127;
  }
  else {
    semi->note = v;
  }
  net_activate(semi, 0, Hz[semi->note]);
  net_activate(semi, 1, tune[semi->note]);
  net_activate(semi, 2, CV[semi->note]);
}

// serialization
u8* op_semi_pickle(op_semi_t* semi, u8* dst) {
  return dst;
}

u8* op_semi_unpickle(op_semi_t* semi, const u8* src) {
   return (u8*)src;
}
