//standard library
#include <string.h>

//print debug
#include "print_funcs.h"

//aleph-avr32
#include "memory.h"
#include "scale.h"

//static variables
static f32* noteBuf;
static f32* transposeBuf;
static f32* tuningBuf;


static f32 note[SCALE_NOTE_SIZE] = {
#include "scale_note.c"
};

static f32 transpose[SCALE_RATIO_SIZE] = {
#include "scale_transpose.c"
};

static f32 tuning[SCALE_TUNE_SIZE] = {
#include "scale_tuning.c"
};

//external functions
void scale_init (void) {
    noteBuf = (f32*)alloc_mem(SCALE_NOTE_SIZE * (sizeof(f32)));
    noteBuf = &note[0]; //try to set a better startup value

    transposeBuf = (f32*)alloc_mem(SCALE_RATIO_SIZE * (sizeof(f32)));
    transposeBuf = &transpose[0]; //try to set a better startup value
    
    tuningBuf = (f32*)alloc_mem(SCALE_TUNE_SIZE * (sizeof(f32)));
    tuningBuf = &tuning[0];
}

f32 note_lookup(s32 val) {
    return note[val];
}

f32 transpose_lookup(s32 ratio) {
    return transpose[ratio];
}
