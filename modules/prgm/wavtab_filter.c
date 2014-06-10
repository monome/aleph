//
//  prgm_filter.c
//  main
//
//  Created by Staffan Jansson on 5/20/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>

/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
 Command line: /www/usr/fisher/helpers/mkfilter -Bu -Lp -o 2 -a 1.6666666667e-01 0.0000000000e+00 -l */

#define NZEROS 2
#define NPOLES 2
#define GAIN   6.449489743e+00

static float xv[NZEROS+1], yv[NPOLES+1];

static void filterloop()
{ for (;;)
{ xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = next input value / GAIN;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] =   (xv[0] + xv[2]) + 2 * xv[1]
    + ( -0.2404082058 * yv[0]) + (  0.6202041029 * yv[1]);
    next output value = yv[2];
}
}
