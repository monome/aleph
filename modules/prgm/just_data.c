//just_data.c
//aleph-bfin

#include <stdio.h>
#include <stdlib.h>


int main()
{
    int i, j, n = 0;
    
    float ratio[] = { 1, 1.5, 1.33333, 1.66667, 1.25, 1.75, 1.2, 1.4,
        1.6, 1.8, 1.6667, 1.83333, 1.125, 1.375, 1.625, 1.875,
        1.11111, 1.22222, 1.44444, 1.55556, 1.77778, 1.88889, 1.1, 1.3,
        1.7, 1.9, 1.06667, 1.13333, 1.26667, 1.46667, 1.53333, 1.73333,
        1.86667, 1.93333, 1.0625, 1.1875, 1.3125, 1.4375, 1.5625, 1.6875,
        1.8125, 1.9375, 1.05556, 1.16667, 1.27778, 1.38889, 1.61111, 1.72222,
        1.83333, 1.94444, 1.05, 1.15, 1.35, 1.45, 1.55, 1.65,
        1.85, 1.95, 1.48148, 1.40625, 1.26562, 1.05469, 1, 1 };

    float just[1023];
    for (j = 0; j < 1023; j++) {
        just[j] = 0;
    }
    
    float *ip;
    ip = &ratio[0];
    
    float *jp;
    jp = &just[0];
    
    j = 0;
    
    for (n = 1; n < 17; n++) {
    i = 0;
    while(i < 64) {
        *(jp + j) = *(ip +i) * n;
        j++;
        i++;
        }
    }
    FILE *fp = fopen("/Aleph/aleph-dev/modules/prgm/just_data.txt", "w");

    for (j = 0; j < 1023; j++) { fprintf(fp, "%f" ", ", just[j]); }
    fprintf(fp, "%f", just[1023]);
    
    fclose(fp);
}








#include <fix.h>

abs_fr1x16	abs_fr1x16	(fract16 f1)	fract16	 Returns the 16-bit value that is the absolute value of the input parameter. Where the input is 0x8000, saturation occurs and 0x7fff is returned.

compose_fr2x16		(fract16 f1, fract16 f2)	fract2x16	 Takes two fract16 values, and returns a fract2x16 value.

float_to_fr16		float	fract16	 Converts a float to fract16



typedef int FIX_16;

#define FRACT_BITS 16
#define FIXED_POINT_16 (16 << FRACT_BITS)
#define MAKE_FLOAT_FIXED(x) ((FIX_16)((x) * FIXED_POINT_16))



#define FIXED_ONE (1 << FRACT_BITS)
#define FIXED_16 (16 << FRACT_BITS)
#define INT2FIXED(x) ((x) << FRACT_BITS)
#define FLOAT2FIXED(x) ((int)((x) * (1 << FRACT_BITS))) 
#define FIXED2INT(x) ((x) >> FRACT_BITS)
#define FIXED2DOUBLE(x) (((double)(x)) / (1 << FRACT_BITS))

#define FRACT_BITS_D2 8
#define MULT(x, y) ( ((x) >> FRACT_BITS_D2) * ((y)>> FRACT_BITS_D2) )

static inline fix16_t fix16_from_float(float a)
{
    float temp = a * fix16_one;
#ifndef FIXMATH_NO_ROUNDING
    temp += (temp >= 0) ? 0.5f : -0.5f;
#endif
    return (fix16_t)temp;
}



// convert to 16.16.
v = f.collect({ |x, i|
	var xint, xfr, y;
    
	xint = x.floor;
	xfr = x - xint;
	xint = xint.asInteger;
    
	y = (xint << 16) | (0xffff & ((xfr * 0x10000).floor.asInteger));
});
v.do({ arg y; y.asHexString.postln; });




//void array_to_1616 (float 

#include <stdio.h>
typedef int Fixed;

#define FRACT_BITS 16
#define FRACT_BITS_D2 8
#define FIXED_ONE (1 << FRACT_BITS)
#define FIXED_16 (16 << FRACT_BITS)
#define INT2FIXED(x) ((x) << FRACT_BITS)
#define FLOAT2FIXED(x) ((int)((x) * (1 << FRACT_BITS))) 
#define FIXED2INT(x) ((x) >> FRACT_BITS)
#define FIXED2DOUBLE(x) (((double)(x)) / (1 << FRACT_BITS))
#define MULT(x, y) ( ((x) >> FRACT_BITS_D2) * ((y)>> FRACT_BITS_D2) )


typedef int FIX_16;

#define FRACT_BITS 16
#define FIXED_POINT_16 (16 << FRACT_BITS)
#define MAKE_FLOAT_FIXED(x) ((FIX_16)((x) * FIXED_POINT_16))


#define MAKE_INT_FIXED(x) ((x) << FRACT_BITS)
#define MAKE_FIXED_INT(x) ((x) >> FRACT_BITS)
#define MAKE_FIXED_FLOAT(x) (((float)(x)) / FIXED_POINT_ONE)

#define FIXED_MULT(x, y) ((x)*(y) >> FRACT_BITS)
#define FIXED_DIV(x, y) (((x)<<FRACT_BITS) / (y))

/* tests */
int main()
{
    int_fixed fixed_x = MAKE_FLOAT_FIXED( 4.5f );
    int_fixed fixed_y = MAKE_INT_FIXED( 2 );
    
    int_fixed fixed_result = FIXED_MULT( fixed_x, fixed_y );
    printf( "%.1f\n", MAKE_FIXED_FLOAT( fixed_result ) );
    
    fixed_result = FIXED_DIV( fixed_result, fixed_y );
    printf( "%.1f\n", MAKE_FIXED_FLOAT( fixed_result ) );
    
    return 0;
}
Which writes out

9.0
4.5
