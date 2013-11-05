/* sin_fr32.c 
   audio
   aleph

   12th-degree polynomial sine approximation in fract32
   
 */

#include "sin_fr32.h"
//#include <fract_math.h>
//#include <fract.h>

#define SIN_ENTRY 7

fract32 sin_fr32 ( fract32 _x ) {
    static const fract32 sin_coeff[SIN_ENTRY] = {
        (fract32) 0x000001d4, (fract32) 0xffff875d, (fract32) 0x0015076d,
        (fract32) 0xfd9a5a6f, (fract32) 0x28cd78cd, (fract32) 0xad510c67,
        (fract32) 0x6487ed51
    };

    fract32 x = _x;
    fract32 x_square, poly_prod, poly_sum, temp_prod, sin_val;

    int i;

    x_square = multr_fr1x32x32 (x, x);

    /* p0 * x^2 + p2 */
    poly_prod = multr_fr1x32x32 (sin_coeff[0], x_square);
    poly_sum  = add_fr321x32 (poly_prod, sin_coeff[1]);

    /* (((p0 * x^2 + p2) * x^2 + p4) * x^2 + p6) * x^2 + p8 */
    for (i = 2; i < 5 ; i++) {
        poly_prod = multr_fr1x32x32 (poly_sum, x_square);
        poly_sum  = add_fr321x32 (poly_prod, sin_coeff[i]);
    }

    /* apply rounding */
    poly_sum  = add_fr321x32 (poly_sum, (fract32) 0x00000002);
    poly_sum  = shr_fr1x32 (poly_sum, 2);

    /* ((((p0 * x^2 + p2) * x^2 + p4) * x^2 + p6) * x^2 + p8) * x^2 + p10 */
    poly_prod = multr_fr1x32x32 (x_square, poly_sum);
    poly_sum  = add_fr321x32 (poly_prod, sin_coeff[5]);

    /*
       (((((p0*x^2 + p2)*x^2 + p4)*x^2 + p6)*x^2 + p8)*x^2 + p10)*x^3 +
                                                      p12/2*x + p12/2*x
    */
    poly_prod = multr_fr1x32x32 (poly_sum, x_square);
    poly_prod = multr_fr1x32x32 (poly_prod, x);

    temp_prod = multr_fr1x32x32 (sin_coeff[6], x);

    poly_sum  = add_fr321x32 (poly_prod, temp_prod);
    sin_val   = add_fr321x32 (poly_sum, temp_prod);

    return sin_val;
}
