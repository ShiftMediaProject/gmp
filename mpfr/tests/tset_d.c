/* Test file for mpfr_set_d and mpfr_get_d.

Copyright (C) 1999 PolKA project, Inria Lorraine and Loria

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
#include <time.h>
#ifdef IRIX64
#include <sys/fpu.h>
#endif
#include "gmp.h"
#include "mpfr.h"
#include "mpfr-impl.h"

extern int isnan();

int
main(int argc, char **argv)
{
  fprintf(stderr, "Test case tset_d disabled\n"); 
  exit(0);			/* THIS TEST CASE IS NOT WORKING */

#if 0
  mpfr_t x,y,z; unsigned long k,n; double d, dd;
#ifdef IRIX64
    /* to get denormalized numbers on IRIX64 */
    union fpc_csr exp;
    exp.fc_word = get_fpc_csr();
    exp.fc_struct.flush = 0;
    set_fpc_csr(exp.fc_word);
#endif

  mpfr_init2(z, 32);
  mpfr_set_d(z, 1.0, 0);
  if (mpfr_get_d(z) != 1.0) {
    mpfr_print_raw(z); putchar('\n');
    printf("Error: 1.0 != 1.0\n"); exit(1);
  }
  mpfr_init2(x, 53); mpfr_init2(y, 53);
  mpfr_set_d(x, d=-1.08007920352320089721e+150, 0);
  if (mpfr_get_d(x) != d) {
    mpfr_print_raw(x); putchar('\n');
    printf("Error: get_d o set_d <> identity for d = %1.20e %1.20e\n",d,
	   mpfr_get_d(x)); exit(1);
  }
  srand48(time(NULL)); 
  mpfr_set_d(x, 8.06294740693074521573e-310, 0); 
  d = -6.72658901114033715233e-165;
  mpfr_set_d(x, d, 0);
  if (d != mpfr_get_d(x)) {
    mpfr_print_raw(x); putchar('\n');
    printf("Error: get_d o set_d <> identity for d = %1.20e %1.20e\n",d,
	   mpfr_get_d(x)); exit(1);
  }
  n = (argc==1) ? 1000000 : atoi(argv[1]);
  for (k = 1; k <= n; k++)
    {      
      d = drand();
      mpfr_set_d(x, d, 0); 
      dd = mpfr_get_d(x);
      if (d != dd && (!isnan(d) || !isnan(dd)))
	{ 
	  fprintf(stderr, 
		  "Mismatch on : %1.18g != %1.18g\n", d, mpfr_get_d(x)); 
	  mpfr_print_raw(x); putchar('\n');
	} 
    }

  mpfr_clear(x); mpfr_clear(y); mpfr_clear(z);
  exit (0); 
#endif /* 0 */
}

