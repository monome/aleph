#include "net_protected.h"
#include "op_harry.h"

//-------------------------------------------------
//----- static function declaration
static void op_harry_in_x(op_harry_t* harry, const io_t v);
static void op_harry_in_y(op_harry_t* harry, const io_t v);
static void op_harry_in_bang(op_harry_t* harry, const io_t v);
static void op_harry_in_size(op_harry_t* harry, const io_t v);

// pickle / unpickle
static u8* op_harry_pickle(op_harry_t* op, u8* dst);
static const u8* op_harry_unpickle(op_harry_t* op, const u8* src);


//-------------------------------------------------
//----- static vars
static op_in_fn op_harry_in_fn[4] = {
  (op_in_fn)&op_harry_in_x,
  (op_in_fn)&op_harry_in_y,
  (op_in_fn)&op_harry_in_bang,
  (op_in_fn)&op_harry_in_size
};

static const char* op_harry_instring  = "X\0      Y\0      BONK\0   SIZE\0  ";
static const char* op_harry_outstring = "HZ\0    ";
static const char* op_harry_opstring  = "HARRY";

#define NUM_DIAMONDS 8
static const u8 diamond_sizes[NUM_DIAMONDS] = {1, 2, 3, 4, 5, 6, 7, 8};

static const io_t limit_1[1][1] = {{0}};

static const io_t limit_3[2][2] = {{0, 1797},
				   {1275, 0}};

static const io_t limit_5[3][3] = {{0, 989, 1797},
				   {2083, 0, 808},
				   {1275, 2264, 0}};

static const io_t limit_7[4][4] = {{0, 989, 1797, 2480},
				   {2083, 0, 808, 1491},
				   {1275, 2264, 0, 683},
				   {592, 1581, 2389, 0}};

static const io_t limit_9[5][5] = {{0, 522, 989, 1797, 2480},
				   {2550, 0, 467, 1275, 1958},
				   {2083, 2605, 0, 808, 1491},
				   {1275, 1797, 2264, 0, 683},
				   {592, 1114, 1581, 2389, 0}};

static const io_t limit_11[6][6] = {{0, 522, 989, 1411, 1797, 2480},
				    {2550, 0, 467, 889, 1275, 1958},
				    {2083, 2605, 0, 422, 808, 1491},
				    {1661, 2183, 2650, 0, 386, 1069},
				    {1275, 1797, 2264, 2686, 0, 683},
				    {592, 1114, 1581, 2003, 2389, 0}};

static const io_t limit_13[7][7] = {{0, 522, 989, 1411, 1797, 2152, 2480},
				    {2550, 0, 467, 889, 1275, 1630, 1958},
				    {2083, 2605, 0, 422, 808, 1163, 1491},
				    {1661, 2183, 2650, 0, 386, 740, 1069},
				    {1275, 1797, 2264, 2686, 0, 355, 683},
				    {920, 1442, 1909, 2332, 2717, 0, 328},
				    {592, 1114, 1581, 2003, 2389, 2744, 0}};

static const io_t limit_15[8][8] = {{0, 522, 989, 1411, 1797, 2152, 2480, 2786},
				    {2550, 0, 467, 889, 1275, 1630, 1958, 2264},
				    {2083, 2605, 0, 422, 808, 1163, 1491, 1797},
				    {1661, 2183, 2650, 0, 386, 740, 1069, 1375},
				    {1275, 1797, 2264, 2686, 0, 355, 683, 989},
				    {920, 1442, 1909, 2332, 2717, 0, 328, 634},
				    {592, 1114, 1581, 2003, 2389, 2744, 0, 306},
				    {286, 808, 1275, 1697, 2083, 2438, 2766, 0}};

static const void *diamonds[NUM_DIAMONDS] = {&limit_1, &limit_3, &limit_5, &limit_7, &limit_9, &limit_11, &limit_13, &limit_15};

//-------------------------------------------------
//----- external function definitions
void op_harry_init(void* mem) {
  op_harry_t* harry = (op_harry_t*)mem;
  harry->super.numInputs = 4;
  harry->super.numOutputs = 1;
  harry->outs[0] = -1;

  harry->super.in_fn = op_harry_in_fn;
  harry->super.pickle = (op_pickle_fn) (&op_harry_pickle);
  harry->super.unpickle = (op_unpickle_fn) (&op_harry_unpickle);

  harry->super.in_val = harry->in_val;
  harry->super.out = harry->outs;
  harry->super.opString = op_harry_opstring;
  harry->super.inString = op_harry_instring;
  harry->super.outString = op_harry_outstring;
  harry->super.type = eOpHarry;

  harry->in_val[0] = &(harry->x);
  harry->in_val[1] = &(harry->y);
  harry->in_val[2] = &(harry->bang);
  harry->in_val[3] = &(harry->size);

  harry->x = 0;
  harry->y = 1;
  harry->bang = 0;
  harry->size = 8;
}

//-------------------------------------------------
//----- static function definitions

static void clip_var_to_limit(volatile io_t *var, io_t limit) {
  if(*var >= limit) {
    *var = limit - 1;
  }
  else if (*var < 0) {
    *var = 0;
  }
}

// set X position on the tonality diamond
static void op_harry_in_x(op_harry_t* harry, const io_t v) {
  harry->x = v;
}

// set Y position on the tonality diamond
static void op_harry_in_y(op_harry_t* harry, const io_t v) {
    harry->y = v;
}

static io_t calculate_pitch(op_harry_t *harry) {
  io_t idx = harry->size - 1;
  clip_var_to_limit(&(idx), NUM_DIAMONDS);
  clip_var_to_limit(&(harry->x), diamond_sizes[idx]);
  clip_var_to_limit(&(harry->y), diamond_sizes[idx]);
  io_t *arr = (io_t *)diamonds[idx];
  return arr[harry->x + harry->y * diamond_sizes[idx]];
}

// Output a pitch
static void op_harry_in_bang(op_harry_t* harry, const io_t v) {
  u8 idx = harry->size - 1;
  if ( (harry->x >=0) &&
       (harry->x < diamond_sizes[idx]) &&
       (harry->y >=0) &&
       (harry->y < diamond_sizes[idx]) ) {
    net_activate(harry, 0, calculate_pitch(harry));
  }
  else {
    net_activate(harry, 0, 0);
  }
}

static void op_harry_in_size(op_harry_t* harry, const io_t v) {
  if(v < 1) {
    harry->size = 1;
  }
  else if (v > NUM_DIAMONDS) {
    harry->size = NUM_DIAMONDS;
  }
  else {
    harry->size = v;
  }
  clip_var_to_limit(&(harry->x), diamond_sizes[harry->size-1]);
  clip_var_to_limit(&(harry->y), diamond_sizes[harry->size-1]);
}

// pickle / unpickle
u8* op_harry_pickle(op_harry_t* op, u8* dst) {
  dst = pickle_io(op->x, dst);
  dst = pickle_io(op->y, dst);
  dst = pickle_io(op->size, dst);
  return dst;
}

const u8* op_harry_unpickle(op_harry_t* op, const u8* src ) {
  src = unpickle_io(src, &(op->x));
  src = unpickle_io(src, &(op->y));
  src = unpickle_io(src, &(op->size));
  return src;
}
