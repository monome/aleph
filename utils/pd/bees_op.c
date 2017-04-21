#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "m_pd.h"
#include "net.h"
#include "op_mul.h"
#include "lo/lo.h"
#include "monome.h"
#include "net_monome.h"
#include "event_types.h"
#include "app.h"
#include "timers.h"

/* #include "op_mul.c" */

typedef struct _bees_pd_class {
  t_object  x_obj;  
  op_t *bees_op;
  t_outlet *f_out[16];
} t_bees_pd_class;

static t_class *bees_pd_classes[numOpClasses];
op_t *dummy_ops[numOpClasses];
char charDump[numOpClasses*256];
char *pd_op_names[numOpClasses];

static t_class *bees_op_class;
// pointer to puredata bees object
/* t_bees_pd_class *bees_pd_object_to_bang; */
// XXX FIXME this fails when triggering up/down a complex routing
// network...

void net_activate(void *op_void, s16 outIdx, const io_t val) {
  op_t *op = (op_t*) op_void;
  t_bees_pd_class *bees_pd_object_to_bang = op->bees_pd_object;
  /* printf("activate da %dth output of op@%d & %d with %d\n", */
  /* 	 outIdx, op, bees_pd_object_to_bang, val); */
  outlet_float(bees_pd_object_to_bang->f_out[outIdx], (float) val);
}

void bees_op_in_0(t_bees_pd_class *x, t_floatarg f) {
  x->bees_op->bees_pd_object = x;
  /* printf("input 0 of op@%d & %d banged !!\n", x, x->bees_op); */
  x->bees_op->in_fn[0](x->bees_op, (io_t) f);
}
void bees_op_in_1(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 1 of op@%d banged !!\n", x); */
  x->bees_op->bees_pd_object = x;
  x->bees_op->in_fn[1](x->bees_op, (io_t) f);
}
void bees_op_in_2(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 2 of op@%d banged !!\n", x); */
  x->bees_op->bees_pd_object = x;
  x->bees_op->in_fn[2](x->bees_op, (io_t) f);
}
void bees_op_in_3(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 3 of op@%d banged !!\n", x); */
  x->bees_op->in_fn[3](x->bees_op, (io_t) f);
}
void bees_op_in_4(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 4 of op@%d banged !!\n", x); */
  x->bees_op->in_fn[4](x->bees_op, (io_t) f);
}
void bees_op_in_5(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 5 of op@%d banged !!\n", x); */
  x->bees_op->in_fn[5](x->bees_op, (io_t) f);
}
void bees_op_in_6(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 6 of op@%d banged !!\n", x); */
  x->bees_op->in_fn[6](x->bees_op, (io_t) f);
}
void bees_op_in_7(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 7 of op@%d banged !!\n", x); */
  x->bees_op->in_fn[7](x->bees_op, (io_t) f);
}
void bees_op_in_8(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 8 of op@%d banged !!\n", x); */
  x->bees_op->in_fn[8](x->bees_op, (io_t) f);
}
void bees_op_in_9(t_bees_pd_class *x, t_floatarg f) {
  /* printf("input 9 of op@%d banged !!\n", x); */
  x->bees_op->in_fn[9](x->bees_op, (io_t) f);
}

typedef void (*beesbang_func_t)(t_bees_pd_class *x, t_floatarg f);

beesbang_func_t beesbang_func[] = {
  bees_op_in_0,
  bees_op_in_1,
  bees_op_in_2,
  bees_op_in_3,
  bees_op_in_4,
  bees_op_in_5,
  bees_op_in_6,
  bees_op_in_7,
  bees_op_in_8,
  bees_op_in_9
};
void *bees_op_free(t_bees_pd_class *bees_pd_obj) {
  op_deinit(bees_pd_obj->bees_op);
  free(bees_pd_obj->bees_op);
  return NULL;
}

void *bees_op_new(t_symbol *s, int argc, t_atom *argv) {
  int i, j;
  /* char op_name_lower[256]; */
  if (s == gensym("bees_op")) {
    t_bees_pd_class *x = (t_bees_pd_class *)pd_new(bees_op_class);
    return (void *)x;
  }

  for(i=0; i < numOpClasses; i++) {
    /* strcpy(op_name_lower,  */
    if(s == gensym(pd_op_names[i])) {
      t_bees_pd_class *x = (t_bees_pd_class *)pd_new(bees_pd_classes[i]);
      x->bees_op = malloc(op_registry[i].size);
      op_init(x->bees_op, i);
      x->bees_op->bees_pd_object = (void*)x;

      /* printf("mwahahaha we godda %s @ %d & %d with %d inputs & %d outputs\n", */
      /* 	     pd_op_names[i], */
      /* 	     x->bees_op->bees_pd_object, */
      /* 	     x->bees_op, */
      /* 	     x->bees_op->numInputs, */
      /* 	     x->bees_op->numOutputs); */
      for(j = 0; j < x->bees_op->numInputs; j++) {
	inlet_new(&x->x_obj, &x->x_obj.ob_pd,
		  &s_float, gensym(x->bees_op->inString+j*8));
      }
      for(j = 0; j < x->bees_op->numOutputs; j++) {
	/* printf("making an outlet...\n"); */
	x->f_out[j] = outlet_new(&x->x_obj, &s_float);
      }
      return (void *)x;
    }
  }
  return NULL;
}

lo_server_thread monome_server;

void monome_server_error(int num, const char *m, const char *path);
int monome_key_handler(const char *path, const char *types, lo_arg ** argv,
		       int argc, void *data, void *user_data);
void monome_testmap (int x, int y);
void monome_testbang (void);
pthread_t monome_updater;
pthread_t timer_poller;

void* always_update_128_grid( void* argument );
void* always_poll_timers( void* argument );
void monome_send_quadrant (int x, int y, int *testdata);
void monome_update_128_grid ();

void bees_op_setup(void) {
  bees_op_class = class_new(gensym("bees_op"),
			    (t_newmethod)bees_op_new,
			    0, sizeof(t_bees_pd_class),
			    CLASS_DEFAULT,
			    A_GIMME, 0);
  int i, j;
  void *mem;
  op_t *new_op;
  for(i=0; i < numOpClasses; i++) {
    // XXX hack... there's no way to read the metadata of a BEES op
    // without instantiating it, so we instantiate one of each op, save
    // the metadata for later
    mem = malloc(op_registry[i].size); /// ops get heap alloc-ed
    op_registry[i].init(mem);
    new_op = (op_t *) mem;
    pd_op_names[i] = malloc(256);
    char *new_op_name = pd_op_names[i];
    strcpy(new_op_name, "BEES_");
    strcat(new_op_name, new_op->opString);
    /* tolower(new_op_name); */
    /* printf("new op: %s\n", new_op_name); */
    if (op_registry[i].deinit) {
      op_registry[i].deinit(mem);
    }
    dummy_ops[i] = new_op;
    bees_pd_classes[i] = class_new(gensym(new_op_name),
				   (t_newmethod)bees_op_new,
				   (t_method)bees_op_free,
				   sizeof(t_bees_pd_class),
				   CLASS_DEFAULT,
				   A_GIMME, 0);
    for(j=0; j < new_op->numInputs; j++) {
      /* printf("adding method %s\n", new_op->inString+j*8); */
      class_addmethod(bees_pd_classes[i],
		      (t_method)beesbang_func[j], gensym(new_op->inString+j*8),
		      A_DEFFLOAT, 0);
    }

    //  XXX FIXME add some reasonable help symbols...
    /* class_sethelpsymbol(bees_op_class[0], gensym("help-bees_op_mul")); */

  }

  // XXX FIXME - for now just assume a monome 128 grid is connected
  // & hardwire the monome driver accordingly
  set_monome_device_desc_osc_dummy(16, 8);
  monome_testbang();
  for(i=0; i<16; i++) {
    for(j=0; j<8; j++) {
      dummyLedBuffer[i+j*16] = 15-(i/2 + j);
    }
  }
  monome_update_128_grid();
  lo_address a = lo_address_new(NULL, "13188");
  lo_send(a, "/sys/port", "i", 6001);

  monome_server = lo_server_thread_new("6001", monome_server_error);

  /* add method that will match any path and args */
  lo_server_thread_add_method(monome_server, "/monome/grid/key", "iii",
			      monome_key_handler, NULL);
  lo_server_thread_start(monome_server);
  pthread_create( &monome_updater, NULL, always_update_128_grid, NULL);
  pthread_create( &timer_poller, NULL, always_poll_timers, NULL);
}

void check_events(void) {
  static event_t e;
  if( event_next(&e) ) {
    (app_event_handlers)[e.type](e.data);
  }
}

void* always_poll_timers( void* argument ) {
  while(1) {
    usleep(1000);
    process_timers();

    /* check_events(); */
    // we have no event loop in BEES pd external - in order to poll
    // timers, added a hack to app_custom_event_callback, instead of
    // calling check_events();
  }
  return NULL;
}

void* always_update_128_grid( void* argument ) {
  while(1) {
    usleep(20000);
    monome_update_128_grid();
  }
  return NULL;
}
void monome_testbang () {
  lo_address a = lo_address_new(NULL, "13188");
  int i, j;
  for(i=0; i<16; i++) {
    for(j=0; j<8; j++) {
      lo_send(a, "/monome/grid/led/level/set", "iii", i, j, i/2+j);
    }
  }
}
void monome_update_128_grid () {
  int i, j;
  int testdata[64];
  if(!monomeFrameDirty) {
    return;
  }
  monomeFrameDirty = 0;
  for(i=0; i<8; i++) {
    for(j=0; j<8; j++) {
      testdata[i+j*8] = (int)monomeLedBuffer[i+j*16];
    }
  }
  monome_send_quadrant(0, 0, testdata);
  for(i=0; i<8; i++) {
    for(j=0; j<8; j++) {
      testdata[i+j*8] = (int)monomeLedBuffer[i+8+j*16];
    }
  }
  monome_send_quadrant(8, 0, testdata);
}

void monome_send_quadrant (int x, int y, int *testdata) {
  lo_address a = lo_address_new(NULL, "13188");
  // XXX hack - this is pretty gross, sorry!  monome quadrant
  // represented in osc by 66 ints - x, y & 64 intensities
  lo_send(a, "/monome/grid/led/level/map", "iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii", x, y, testdata[0], testdata[1], testdata[2], testdata[3], testdata[4], testdata[5], testdata[6], testdata[7], testdata[8], testdata[9], testdata[10], testdata[11], testdata[12], testdata[13], testdata[14], testdata[15], testdata[16], testdata[17], testdata[18], testdata[19], testdata[20], testdata[21], testdata[22], testdata[23], testdata[24], testdata[25], testdata[26], testdata[27], testdata[28], testdata[29], testdata[30], testdata[31], testdata[32], testdata[33], testdata[34], testdata[35], testdata[36], testdata[37], testdata[38], testdata[39], testdata[40], testdata[41], testdata[42], testdata[43], testdata[44], testdata[45], testdata[46], testdata[47], testdata[48], testdata[49], testdata[50], testdata[51], testdata[52], testdata[53], testdata[54], testdata[55], testdata[56], testdata[57], testdata[58], testdata[59], testdata[60], testdata[61], testdata[62], testdata[63]);
}

u32 monome_grid_key_encode_event(u8 x, u8 y, u8 val) {
  u32 data;
  u8 *bdata = (u8*)(&data);
  bdata[0] = x;
  bdata[1] = y;
  bdata[2] = val;
  return data;
}

int monome_key_handler(const char *path, const char *types, lo_arg ** argv,
		       int argc, void *data, void *user_data) {
    int i;

    if(argv[2] > 0) {
      /* printf("keydown: %d,%d\n", (int)argv[0]->i, (int)argv[1]->i); */
    } else {
      /* printf("keyup: %d,%d\n", (int)argv[0]->i, (int)argv[1]->i); */
    }
    u32 aleph_grid_key_data =
      monome_grid_key_encode_event((u8) argv[0]->i,
				   (u8) argv[1]->i,
				   (u8) argv[2]->i);
    (*monome_grid_key_handler)(monomeOpFocus, aleph_grid_key_data);
    return 1;
}

void monome_server_error(int num, const char *msg, const char *path)
{
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}
