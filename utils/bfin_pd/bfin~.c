#include <stdlib.h>
#include "m_pd.h"
#include "module.h"
#include "fract2float_conv.h"
#include "fix.h"
#include "fract_math.h"
#include "params.h"
#include "param_scaler.h"

static t_class *bfin_tilde_class;

typedef struct _bfin_tilde {
  t_object  x_obj;
  t_sample f_pan;
  t_sample f;

  t_inlet *x_in[4];
  t_outlet *x_out[4];
} t_bfin_tilde;

t_int *bfin_tilde_perform(t_int *w) {
  t_bfin_tilde *x = (t_bfin_tilde *)(w[1]);
  t_sample  *in0 =    (t_sample *)(w[2]);
  t_sample  *in1 =    (t_sample *)(w[3]);
  t_sample  *in2 =    (t_sample *)(w[4]);
  t_sample  *in3 =    (t_sample *)(w[5]);
  t_sample  *out0 =    (t_sample *)(w[6]);
  t_sample  *out1 =    (t_sample *)(w[7]);
  t_sample  *out2 =    (t_sample *)(w[8]);
  t_sample  *out3 =    (t_sample *)(w[9]);
  int          n =           (int)(w[10]);
  /* t_sample f_pan = (x->f_pan<0)?0.0:(x->f_pan>1)?1.0:x->f_pan;   */

  while (n--) {
    in[0] = float_to_fr32(*in0++);
    in[1] = float_to_fr32(*in1++);
    in[2] = float_to_fr32(*in2++);
    in[3] = float_to_fr32(*in3++);
    module_process_frame();
    *out0++ = fr32_to_float(out[0]);
    *out1++ = fr32_to_float(out[1]);
    *out2++ = fr32_to_float(out[2]);
    *out3++ = fr32_to_float(out[3]);
  }

  return (w+11);
}

void bfin_tilde_dsp(t_bfin_tilde *x, t_signal **sp) {
  dsp_add(bfin_tilde_perform, 10, x,
	  sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec,
	  sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec, sp[7]->s_vec,
	  sp[0]->s_n);
}

void bfin_tilde_free(t_bfin_tilde *x) {
  int i;
  for(i=0; i < 4; i++) {
    inlet_free(x->x_in[i]);
    outlet_free(x->x_out[i]);
  }
}

ParamDesc* desc = NULL;
void *bfin_tilde_new(t_floatarg f) {
  SDRAM_ADDRESS = malloc(SDRAM_SIZE);
  module_init();
  desc = malloc(1024*1024);
  if(desc) {
    fill_param_desc(desc);
  }
  t_bfin_tilde *x = (t_bfin_tilde *)pd_new(bfin_tilde_class);

  x->f_pan = f;

  int i;
  x->x_out[0]=outlet_new(&x->x_obj, &s_signal);
  for(i=1; i < 4; i++) {
    x->x_in[i]=inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    x->x_out[i]=outlet_new(&x->x_obj, &s_signal);
  }

  return (void *)x;
}

void bfin_tilde_handle_message(t_bfin_tilde *x, t_symbol *s, int argc, t_atom *argv) {
  int i;
  /* printf("symbolarg: %s, ", s->s_name); */
  /* for(i=0; i < argc; i++) { */
  /*   if(atom_getfloat(&argv[i])) { */
  /*     printf("floatarg: %d, ", (int)atom_getfloat(&argv[i])); */
  /*   } */
  /*   else if(atom_getsymbol(&argv[i])) { */
  /*     printf("symbolarg: %s, ", atom_getsymbol(&argv[i])->s_name); */
  /*   } */
  /* } */
  /* printf("\n"); */

  if(s == gensym("param") && argc >= 2){
    /* printf("setting param: %d, %d\n", (int)atom_getfloat(&argv[0]), (int)atom_getfloat(&argv[1])); */
    int paramIdx = (int)atom_getfloat(&argv[0]);
    io_t ioVal = (io_t)atom_getfloat(&argv[1]);
    ParamScaler tmp = {.desc = &(desc[paramIdx])};
    tmp.inMin = scaler_get_in(&tmp, desc[paramIdx].min);
    tmp.inMax = scaler_get_in(&tmp, desc[paramIdx].max);
    if (ioVal > tmp.inMax) {
      ioVal = tmp.inMax;
    }
    else if (ioVal < desc[paramIdx].min) {
      ioVal = tmp.inMin;
    }
    s32 bfin_val = scaler_get_value(&tmp, ioVal);
    module_set_param(paramIdx, bfin_val);
    char paramString[256] = {0};
    scaler_get_str(paramString, &tmp, ioVal);
    char mess[256];
    strcpy(mess, desc[paramIdx].label);
    strcat(mess, paramString);
    post(mess);
  }
  if(s == gensym("list")) {
    for(i=0; i < eParamNumParams; i++) {
      printf("param %d: %s\n", i, desc[i].label);
    }
  }
}

void bfin_tilde_setup(void) {
  bfin_tilde_class = class_new(gensym("bfin~"),
			       (t_newmethod)bfin_tilde_new,
			       0, sizeof(t_bfin_tilde),
			       CLASS_DEFAULT,
			       A_DEFFLOAT, 0);

  class_addmethod(bfin_tilde_class,
		  (t_method)bfin_tilde_dsp, gensym("dsp"), 0);
  class_addanything(bfin_tilde_class, bfin_tilde_handle_message);
  CLASS_MAINSIGNALIN(bfin_tilde_class, t_bfin_tilde, f);
}
