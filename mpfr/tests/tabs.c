/* Test file for mpfr_abs.

Copyright (C) 2000 Free Software Foundation.

This file is part of the MPFR Library.

The MPFR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The MPFR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
License for more details.

You should have received a copy of the GNU Library General Public License
along with the MPFR Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
MA 02111-1307, USA. */

#include <stdio.h>
#include <stdlib.h>
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-test.h"

#define Infp (1/0.)

extern int isnan();

int main(int argc, char *argv[])
{
   mpfr_t x; int n, k, rnd; double d, dd;
#ifdef __mips
    /* to get denormalized numbers on IRIX64 */
    union fpc_csr exp;
    exp.fc_word = get_fpc_csr();
    exp.fc_struct.flush = 0;
    set_fpc_csr(exp.fc_word);
#endif

   mpfr_init2(x, 53);

   mpfr_set_d(x, 1.0, GMP_RNDN);
   mpfr_abs(x, x, GMP_RNDN);
   if (mpfr_get_d(x) != 1.0) {
     fprintf(stderr, "Error in mpfr_abs(1.0)\n"); exit(1);
   }

   mpfr_set_d(x, -1.0, GMP_RNDN);
   mpfr_abs(x, x, GMP_RNDN);
   if (mpfr_get_d(x) != 1.0) {
     fprintf(stderr, "Error in mpfr_abs(-1.0)\n"); exit(1);
   }

   mpfr_set_d(x, -6/-0., GMP_RNDN); 
   mpfr_abs(x, x, GMP_RNDN); 
   if (mpfr_get_d(x) != Infp) { 
     fprintf(stderr, "Error in mpfr_abs(Inf).\n"); exit(1); 
   }

   mpfr_set_d(x, 2/-0., GMP_RNDN); 
   mpfr_abs(x, x, GMP_RNDN); 
   if (mpfr_get_d(x) != Infp) { 
     fprintf(stderr, "Error in mpfr_abs(-Inf).\n"); exit(1); 
   }


   n = (argc==1) ? 1000000 : atoi(argv[1]);
   for (k = 1; k <= n; k++)
     {      
       d = drand();
       rnd = rand() % 4;
       mpfr_set_d(x, d, 0);
       mpfr_abs(x, x, rnd);
       dd = mpfr_get_d(x);
       if (dd != fabs(d) && !isnan(d))
	 { 
	   fprintf(stderr, 
		   "Mismatch on d = %1.18g\n", d); 
	   mpfr_print_raw(x); putchar('\n');
	   exit(1);
	 } 
     }

   mpfr_clear(x);
   return 0;
}
